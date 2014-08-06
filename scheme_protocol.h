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

#ifndef SCHEME_PROTOCOL_H
#define SCHEME_PROTOCOL_H

#include <memory>

#include <boost/asio.hpp>

#include "protocol.h"

namespace decof
{

class object_dictionary;

class scheme_protocol : public protocol
{
public:
    explicit scheme_protocol(object_dictionary& object_dictionary, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    static void handle_connect(object_dictionary& object_dictionary, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

private:
    virtual void read_handler(const boost::system::error_code &error, std::size_t bytes_transferred) override;
    virtual void write_handler(const boost::system::error_code &error, std::size_t) override;

    void read_next();
    void write_next(std::string str);
};

} // namespace decof

#endif // SCHEME_PROTOCOL_H
