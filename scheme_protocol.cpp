/*
 * Copyright (c) 2014 Florian Behrens
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "scheme_protocol.h"

#include <algorithm>
#include <iostream>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>

#include "errors.h"
#include "exceptions.h"
#include "object_dictionary.h"
#include "basic_parameter.h"
#include "scheme_common.h"
#include "string_encoder.h"
#include "tree_element.h"

namespace asio = boost::asio;

namespace {

const std::string prompt("> ");

struct ws_separated_quotable_list
{
    ws_separated_quotable_list() {
        reset();
    }

    template <typename InputIterator, typename Token>
    bool operator()(InputIterator& next, InputIterator end, Token& tok) {
        tok = Token();

        for (; next != end; ++next) {
            // Ignore whitespaces in between tokens
            if (start_) {
                if (::iswspace(*next))
                    continue;
                else
                    start_ = false;
            }

            // If in quotation, only search for quotation mark
            if (quote_) {
                if (*next == '"') {
                    ++next;
                    return true;
                } else {
                    tok += *next;
                    continue;
                }
            } else {
                // Find whitespace or quote
                if (::iswspace(*next)) {
                    start_ = true;
                    return true;
                }

                if (*next == '"') {
                    quote_ = true;
                    continue;
                }

                tok += *next;
            }
        }

        if (tok.size() > 0)
            return true;

        return false;
    }

    void reset() {
        start_ = true;
        quote_ = false;
    }

private:
    bool start_;
    bool quote_;
};

} // anonymous namespace

namespace decof
{

scheme_protocol::scheme_protocol(server &server, const tcp::endpoint &endpoint)
  : client_proxy(server),
    io_service_(server.ioService()),
    acceptor_(server.ioService(), endpoint),
    socket_(server.ioService())
{}

void scheme_protocol::preload()
{
    if (socket_.is_open())
        socket_.close();

    // Make 1st asynchronous call
    acceptor_.async_accept(
        socket_,
        std::bind(&scheme_protocol::accept_handler, this, std::placeholders::_1)
    );
}

void scheme_protocol::accept_handler(const boost::system::error_code &error)
{
    if (!error) {
        // Prepare for data reception
        write_next(prompt);
        read_next();
    } else
        throw std::runtime_error(error.message());
}

void scheme_protocol::read_next()
{
    // Connection accepted, start async reads
    asio::async_read_until(
        socket_,
        inbuf_,
        '\n',
        std::bind(&scheme_protocol::read_handler, this, std::placeholders::_1, std::placeholders::_2)
    );
}

void scheme_protocol::read_handler(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error) {
        // First copy bytes_transferred bytes from the streambuf to a
        // std::string. This is one of the uglyest things in boost::asio and only
        // properly documented in stackoverflow:
        // http://stackoverflow.com/questions/877652/copy-a-streambufs-contents-to-a-string.
        boost::asio::streambuf::const_buffers_type buffer = inbuf_.data();
        std::string str(boost::asio::buffers_begin(buffer), boost::asio::buffers_begin(buffer) + bytes_transferred);
        inbuf_.consume(bytes_transferred);

        // Trim (whitespace as in std::is_space() and parantheses) and tokenize the request string
        boost::algorithm::trim_if(str, boost::is_any_of(" \f\n\r\t\v()"));
        std::vector<std::string> tokens;
        boost::tokenizer<ws_separated_quotable_list> tokenizer(str);

        for (boost::tokenizer<ws_separated_quotable_list>::iterator beg = tokenizer.begin(); beg != tokenizer.end(); ++beg) {
            tokens.push_back(*beg);
        }

        std::stringstream ss;
        try {
            if (tokens.size() <= 1)
                throw parse_error();

            // Lower-case first two substrings
            std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);
            std::transform(tokens[1].begin(), tokens[1].end(), tokens[1].begin(), ::tolower);

            // Remove optional "'" from parameter name
            if (tokens[1][0] == '\'')
                tokens[1].erase(0, 1);

            if (tokens[0] == "get" || tokens[0] == "param-ref") {
                if (tree_element *te = server_.objectDictionary().find_object(tokens[1]))
                    ss << string_encoder::encode(te->any_value()) << std::endl;
                else
                    throw invalid_parameter_error();
            } else if (tokens[0] == "set" || tokens[0] == "param-set!") {
                if (tokens.size() == 3) {
                    set_parameter(tokens[1], tokens[2]);
                    ss << "OK" << std::endl;
                }
                else
                    throw parse_error();
            } else
                throw unknown_operation_error();
        } catch (runtime_error& ex) {
            ss << "ERROR " << ex.code() << ": " << ex.what() << std::endl;
        }

        ss << prompt;
        write_next(ss.str());
        read_next();
    } else if (error.value() == asio::error::eof) {
        // Connection was closed by peer
        preload();
    } else
        throw std::runtime_error(error.message());
}

void scheme_protocol::write_next(std::string str)
{
    std::ostream os(&outbuf_);
    os << str;

    asio::async_write(
        socket_,
        outbuf_,
        std::bind(&scheme_protocol::write_handler, this, std::placeholders::_1, std::placeholders::_2)
    );
}

void scheme_protocol::write_handler(const boost::system::error_code &error, std::size_t)
{
    if (error.value() == asio::error::eof) {
        // Connection was closed by peer
        preload();
    } else if (error)
        throw std::runtime_error(error.message());
}

} // namespace decof
