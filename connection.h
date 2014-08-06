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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>

namespace decof
{

class connection
{
public:
    struct observer {
        virtual void connected() = 0;
        virtual void disconnected() = 0;
        virtual void read_handler(const boost::system::error_code &error, std::size_t bytes_transferred) = 0;
        virtual void write_handler(const boost::system::error_code &error, std::size_t) = 0;
    };

public:
    virtual std::string endpoint() const = 0;

    virtual void async_read_until(boost::asio::streambuf&, char delim) = 0;

    virtual void async_write(const std::string& response) = 0;

protected:
    void notify_read_handler(const boost::system::error_code &error, std::size_t bytes_transferred);
    void notify_write_handler(const boost::system::error_code &error, std::size_t);

private:
    observer* observer_ = nullptr;
};

} // namespace decof

#endif // CONNECTION_H
