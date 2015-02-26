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

#include "textproto_pubsub.h"

#include <string>
#include <vector>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "connection.h"
#include "exceptions.h"
#include "textproto_encoder.h"

namespace decof
{

std::string textproto_pubsub::connection_type() const
{
    return connection_->type();
}

std::string textproto_pubsub::remote_endpoint() const
{
    return connection_->remote_endpoint();
}

void textproto_pubsub::preload()
{
    // Connect to signals of connection class
    connection_->read_signal.connect(std::bind(&textproto_pubsub::read_handler, this, std::placeholders::_1));
    connection_->async_read_until('\n');
}

void textproto_pubsub::read_handler(const std::string &cstr)
{
    // Trim (whitespace as in std::is_space() and parantheses) and tokenize the request string
    std::string str(cstr);
    boost::algorithm::trim_if(str, boost::is_any_of(" \f\n\r\t\v()"));
    std::vector<std::string> tokens;
    boost::algorithm::split(tokens, str, boost::algorithm::is_space(), boost::algorithm::token_compress_on);

    try {
        if (tokens.size() <= 1)
            throw parse_error();

        // Lower-case first substring
        std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

        // Remove optional "'" from parameter name
        if (tokens[1][0] == '\'')
            tokens[1].erase(0, 1);

        if (tokens[0] == "subscribe" || tokens[0] == "add") {
            //observe(tokens[1], boost::bind(&scheme_monitor_protocol::notify, this, _1, _2));
            observe(tokens[1], std::bind(&textproto_pubsub::notify, this, std::placeholders::_1, std::placeholders::_2));
        } else if (tokens[0] == "unsubscribe" || tokens[0] == "remove") {
            unobserve(tokens[1]);
        } else
            throw unknown_operation_error();
    } catch (runtime_error& ex) {
        std::stringstream ss;
        ss << "ERROR " << ex.code() << ": " << ex.what() << std::endl;
        connection_->async_write(ss.str());
    }

    connection_->async_read_until('\n');
}

void textproto_pubsub::notify(const std::string &uri, const boost::any &any_value)
{
    // Get current time in textual representation
    const size_t max_length = 25;
    char time_str[max_length];
    std::time_t now = std::time(nullptr);
    std::strftime(time_str, sizeof(time_str), "%FT%T.000Z", std::localtime(&now));

    std::stringstream ss;
    ss << "(" << time_str << " '" << uri << " ";
    textproto_encoder().encode_any(ss, any_value);
    ss << ")\n";
    connection_->async_write(ss.str());
}

} // namespace decof
