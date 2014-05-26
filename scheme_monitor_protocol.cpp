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

#include "scheme_monitor_protocol.h"

#include <functional>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "scheme_common.h"
#include "scheme_errors.h"
#include "string_encoder.h"

namespace asio = boost::asio;

scheme_monitor_protocol::scheme_monitor_protocol(Server &server, const tcp::endpoint &endpoint) :
    ClientProxy(server),
    io_service_(server.ioService()),
    acceptor_(server.ioService(), endpoint),
    socket_(server.ioService())
{}

void scheme_monitor_protocol::preload()
{
    // Make 1st asynchronous call
    acceptor_.async_accept(
        socket_,
        std::bind(&scheme_monitor_protocol::accept_handler, this, std::placeholders::_1)
    );
}

void scheme_monitor_protocol::accept_handler(boost::system::error_code ec)
{
    if (!ec)
        read_next();
    else {
        preload();
    }
}

void scheme_monitor_protocol::read_next()
{
    // Connection accepted, start async reads
    asio::async_read_until(
        socket_,
        inbuf_,
        '\n',
        std::bind(&scheme_monitor_protocol::read_handler, this, std::placeholders::_1, std::placeholders::_2)
    );
}

void scheme_monitor_protocol::read_handler(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (error)
        throw std::runtime_error(error.message());

    // First copy bytes_transferred bytes from the streambuf to a
    // std::string. This is one of the uglyest things in boost::asio and only
    // properly documented in stackoverflow:
    // http://stackoverflow.com/questions/877652/copy-a-streambufs-contents-to-a-string.
    boost::asio::streambuf::const_buffers_type buffer = inbuf_.data();
    std::string str(boost::asio::buffers_begin(buffer), boost::asio::buffers_begin(buffer) + bytes_transferred);
    inbuf_.consume(bytes_transferred);

    // Trim and tokenize the request string
    boost::algorithm::trim(str);
    std::vector<std::string> tokens;
    boost::algorithm::split(tokens, str, boost::algorithm::is_space(), boost::algorithm::token_compress_on);

    std::stringstream ss;
    try {
        if (tokens.size() <= 1)
            throw parse_error();

        // Lower-case first substring
        std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

        if (tokens[0] == "subscribe") {
            //observe(tokens[1], boost::bind(&scheme_monitor_protocol::notify, this, _1, _2));
            observe(tokens[1], std::bind(&scheme_monitor_protocol::notify, this, std::placeholders::_1, std::placeholders::_2));
        } else if (tokens[0] == "unsubscribe") {
            unobserve(tokens[1]);
        } else
            ss << SCHEME_UNKNOWN_OPERATION_ERROR << std::endl;
    } catch (access_denied_error) {
        ss << SCHEME_ACCESS_DENIED_ERROR << std::endl;
    } catch (invalid_parameter_error) {
        ss << SCHEME_INVALID_PARAMETER_ERROR << std::endl;
    } catch (wrong_type_error) {
        ss << SCHEME_WRONG_TYPE_ERROR << std::endl;
    } catch (parse_error) {
        ss << SCHEME_PARSE_ERROR << std::endl;
    } catch (invalid_value_error) {
        ss << SCHEME_INVALID_VALUE_ERROR << std::endl;
    } catch (...) {
        ss << SCHEME_UNKNOWN_ERROR << std::endl;
    }

    write_next(ss.str());
    read_next();
}

void scheme_monitor_protocol::write_next(std::string str)
{
    std::ostream os(&outbuf_);
    os << str;

    asio::async_write(
        socket_,
        outbuf_,
        std::bind(&scheme_monitor_protocol::write_handler, this, std::placeholders::_1, std::placeholders::_2)
    );
}

void scheme_monitor_protocol::write_handler(const boost::system::error_code &error, std::size_t)
{
    if (error)
        throw std::runtime_error(error.message());
}

void scheme_monitor_protocol::notify(const std::string &uri, const boost::any &any_value) noexcept
{
    write_next(uri + ": " + string_encoder::encode(any_value) + "\n");
}
