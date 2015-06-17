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

#include "clisrv_context.h"

#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/any.hpp>

#include "connection.h"
#include "exceptions.h"
#include "object.h"
#include "object_dictionary.h"
#include "parser.h"
#include "encoder.h"
#include "visitor.h"

namespace
{

const std::string prompt("> ");

} // anonymous namespace

namespace decof
{

namespace cli
{

std::string clisrv_context::connection_type() const
{
    return connection_->type();
}

std::string clisrv_context::remote_endpoint() const
{
    return connection_->remote_endpoint();
}

void clisrv_context::preload()
{
    // Connect to signals of connection class
    connection_->read_signal.connect(std::bind(&clisrv_context::read_handler, this, std::placeholders::_1));
    connection_->disconnect_signal.connect(std::bind(&clisrv_context::disconnect_handler, this));

    connection_->async_write("DeCoF command line\n" + prompt);
    connection_->async_read_until('\n');
}

void clisrv_context::read_handler(const std::string& cstr)
{
    // Trim (whitespace as in std::is_space() and parantheses) and tokenize the request string
    std::string str(cstr);
    boost::algorithm::trim_if(str, boost::is_any_of(" \f\n\r\t\v()"));

    // Read and lower-case operation and uri
    std::string op, uri;
    std::stringstream ss_in(str);
    ss_in >> op >> uri;
    std::transform(op.begin(), op.end(), op.begin(), ::tolower);
    std::transform(uri.begin(), uri.end(), uri.begin(), ::tolower);

    // Remove optional "'" from parameter name
    if (!uri.empty() && uri[0] == '\'')
            uri.erase(0, 1);

    // Parse optional value string using flexc++/bisonc++ parser
    boost::any any_value;
    if (ss_in.peek() != std::stringstream::traits_type::eof()) {
        parser parser(ss_in);
        parser.parse();
        any_value = parser.result();
    }

    std::stringstream ss_out;
    try {
        if ((op == "get" || op == "param-ref") && !uri.empty() && any_value.empty()) {
            boost::any any_value = get_parameter(uri);
            encoder().encode_any(ss_out, any_value);
            ss_out << std::endl;
        } else if ((op == "set" || op == "param-set!") && !uri.empty() && !any_value.empty()) {
            set_parameter(uri, any_value);
            ss_out << "0\n";
        } else if ((op == "signal" || op == "exec") && !uri.empty() && any_value.empty()) {
            signal_event(uri);
            ss_out << "()\n";
        } else if ((op == "browse" || op == "param-disp") && any_value.empty()) {
            // This command is for compatibility reasons with legacy DeCoF
            std::string root_uri(object_dictionary_.name());
            if (!uri.empty())
                root_uri = uri;
            std::stringstream temp_ss;
            visitor visitor(temp_ss);
            browse(&visitor, root_uri);
            ss_out << temp_ss.str();
        } else
            throw parse_error();
    } catch (runtime_error& ex) {
        ss_out << "ERROR " << ex.code() << ": " << ex.what() << std::endl;
    }

    ss_out << prompt;
    connection_->async_write(ss_out.str());
    connection_->async_read_until('\n');
}

void clisrv_context::disconnect_handler()
{
    // Remove this client context from object dictionary. Because it is a
    // shared pointer, it gets deleted after leaving function scope.
    auto sptr = shared_from_this();
    object_dictionary_.remove_context(sptr);
}

} // namespace cli

} // namespace decof