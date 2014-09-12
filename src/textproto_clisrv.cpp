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

#include "textproto_clisrv.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/any.hpp>
#include <boost/tokenizer.hpp>

#include "connection.h"
#include "exceptions.h"
#include "object_dictionary.h"
#include "string_codec.h"
#include "textproto_visitor.h"
#include "tree_element.h"
#include "xml_visitor.h"

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

            if (quote_) {
                // If in quotation, only search for quotation mark
                tok += *next;
                if (*next == '"') {
                    ++next;
                    return true;
                } else
                    continue;
            } else if (array_) {
                // If in array, only search for closing bracket
                tok += *next;
                if (*next == ']') {
                    ++next;
                    return true;
                } else
                    continue;
            } else {
                // Find whitespace, quote or opening bracket
                if (::iswspace(*next)) {
                    start_ = true;
                    return true;
                }

                if (*next == '"')
                    quote_ = true;
                else if (*next == '[')
                    array_ = true;

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
        array_ = false;
    }

private:
    bool start_;
    bool quote_;
    bool array_;
};

} // anonymous namespace

namespace decof
{

void textproto_clisrv::preload()
{
    // Connect to signals of connection class
    connection_->read_signal.connect(std::bind(&textproto_clisrv::read_handler, this, std::placeholders::_1));
    connection_->disconnect_signal.connect(std::bind(&textproto_clisrv::disconnect_handler, this));

    connection_->async_write("DeCoF command line\n" + prompt);
    connection_->async_read_until('\n');
}

void decof::textproto_clisrv::read_handler(const std::string& cstr)
{
    // Trim (whitespace as in std::is_space() and parantheses) and tokenize the request string
    std::string str(cstr);
    boost::algorithm::trim_if(str, boost::is_any_of(" \f\n\r\t\v()"));
    std::vector<std::string> tokens;
    boost::tokenizer<ws_separated_quotable_list> tokenizer(str);

    for (boost::tokenizer<ws_separated_quotable_list>::iterator beg = tokenizer.begin(); beg != tokenizer.end(); ++beg) {
        tokens.push_back(*beg);
    }

    std::stringstream ss;
    try {
        if (tokens.size() == 0)
            throw parse_error();

        // Lower-case first substring
        std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

        if (tokens.size() >= 2) {
            // Lower-case second substring
            std::transform(tokens[1].begin(), tokens[1].end(), tokens[1].begin(), ::tolower);

                // Remove optional "'" from parameter name
                if (tokens[1][0] == '\'')
                    tokens[1].erase(0, 1);

            // Add optional 'root' parameter name base
            if (tokens[1] != "root" && !boost::algorithm::starts_with(tokens[1], "root:"))
                tokens[1] = std::string("root:") + tokens[1];
        }

        if ((tokens[0] == "get" || tokens[0] == "param-ref") && tokens.size() == 2) {
            boost::any any_value = get_parameter(tokens[1]);
            ss << string_codec::encode(any_value) << std::endl;
        } else if ((tokens[0] == "set" || tokens[0] == "param-set!") && tokens.size() == 3) {
            boost::any any_value = string_codec::decode(tokens[2]);
            set_parameter(tokens[1], any_value);
            ss << "0\n";
        } else if ((tokens[0] == "signal" || tokens[0] == "exec") && tokens.size() == 2) {
            signal_event(tokens[1]);
            ss << "()\n";
        } else if (tokens[0] == "browse" || tokens[0] == "param-disp") {
            // This command is for compatibility reasons with legacy DeCoF
            std::string root_uri("root");
            if (tokens.size() == 2)
                root_uri = tokens[1];
            else if (tokens.size() > 2)
                throw parse_error();

            std::stringstream temp_ss;
            textproto_visitor visitor(temp_ss);
            browse(root_uri, &visitor);
            ss << temp_ss.str();
        } else if (tokens[0] == "disc") {
            std::string root_uri("root");
            if (tokens.size() == 2)
                root_uri = tokens[1];
            else if (tokens.size() > 2)
                throw parse_error();

            std::stringstream temp_ss;
            {
                xml_visitor visitor(temp_ss);
                browse(root_uri, &visitor);
            }
            ss << temp_ss.str();
        } else
            throw parse_error();
    } catch (runtime_error& ex) {
        ss << "ERROR " << ex.code() << ": " << ex.what() << std::endl;
    }

    ss << prompt;
    connection_->async_write(ss.str());
    connection_->async_read_until('\n');
}

void textproto_clisrv::disconnect_handler()
{
    // Remove this client context from object dictionary. Because it is a
    // shared pointer, it gets deleted after leaving function scope.
    auto sptr = shared_from_this();
    object_dictionary_.remove_context(sptr);
}

} // namespace decof
