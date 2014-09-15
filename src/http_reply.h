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

#ifndef DECOF_HTTP_REPLY_H
#define DECOF_HTTP_REPLY_H

#include <string>
#include <vector>
#include <boost/asio.hpp>

namespace decof
{

/// A HTTP reply to be sent to a client.
class http_reply
{
public:
    /// The status of the reply.
    enum status_t
    {
        ok = 200,
        bad_request = 400,
        not_found = 404
    };

    explicit http_reply(status_t status = ok);

    void header(const std::string &name, const std::string &value);
    std::string header(const std::string &name) const;

    void content(const std::string &content, const std::string &content_type = "text/xml");
    std::string content() const;

    /// Convert the reply into a vector of buffers. The buffers do not own the
    /// underlying memory blocks, therefore the reply object must remain valid and
    /// not be changed until the write operation has completed.
    std::vector<boost::asio::const_buffer> to_buffers();

    std::string to_string() const;

    /// Get a stock reply.
    static http_reply stock_reply(status_t status);

private:
    status_t status_;

    /// The headers to be included in the reply.
    std::map<std::string, std::string> headers_;

    /// The content to be sent in the reply.
    std::string content_;
};

} // namespace decof

#endif // DECOF_HTTP_REPLY_H
