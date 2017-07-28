/*
 * Copyright (c) 2017 Florian Behrens
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

#include <cassert>
#include <ostream>

#include <boost/asio/buffers_iterator.hpp>
#include <boost/lexical_cast.hpp>

#include <beast/websocket/error.hpp>
#include <beast/websocket/option.hpp>

#include <decof/exceptions.h>
#include <decof/object_dictionary.h>
#include <decof/websocket/websocket_context.h>

#include "error.h"
#include "request.h"
#include "response.h"

namespace decof
{

namespace websocket
{

std::string websocket_context::connection_type() const
{
    return "ws";
}

std::string websocket_context::remote_endpoint() const
{
    boost::system::error_code ec;
    return boost::lexical_cast<std::string>(stream_.next_layer().remote_endpoint(ec));
}

void websocket_context::preload()
{
    auto self(std::dynamic_pointer_cast<websocket_context>(shared_from_this()));

    // Receive and perform WebSocket Upgrade request
    stream_.async_accept([self](const beast::error_code& error) {
        if (error) {
            self->shutdown(error);
            return;
        }

        self->async_read_message();
    });
}

void websocket_context::preload_upgraded()
{
    async_read_message();
}

void websocket_context::async_read_message()
{
    assert(reading_active_ == false);
    assert(response_pending_ == false);

    reading_active_ = true;

    auto self(std::dynamic_pointer_cast<websocket_context>(shared_from_this()));
    stream_.async_read(inbuf_, [self](const beast::error_code& error) {
        self->read_handler(error);
    });
}

void websocket_context::async_write_message()
{
    writing_active_ = true;

    auto self(std::dynamic_pointer_cast<websocket_context>(shared_from_this()));
    stream_.async_write(outbuf_.data(), [self](const beast::error_code& error) {
        self->write_handler(error);
    });
}

void websocket_context::read_handler(const beast::error_code& error)
{
    assert(reading_active_);
    assert(response_pending_ == false);

    reading_active_ = false;

    if (error) {
        shutdown(error);
        return;
    }

    if (writing_active_) {
        response_pending_ = true;
        return;
    }

    process_request();
}

void websocket_context::write_handler(const beast::error_code &error)
{
    assert(writing_active_);

    writing_active_ = false;
    outbuf_.consume(outbuf_.size());

    if (error) {
        shutdown(error);
        return;
    }

    preload_writing();

    if (! reading_active_)
        async_read_message();
}

void websocket_context::process_request()
{
    assert(writing_active_ == false);

    request req;
    response resp;

    writing_active_ = true;
    response_pending_ = false;

    try {
        using std::placeholders::_1;
        using std::placeholders::_2;
        using boost::asio::buffers_begin;
        using boost::asio::buffers_end;

        if (stream_.got_binary())
            throw bad_request_error();

        req.parse(buffers_begin(inbuf_.data()), buffers_end(inbuf_.data()));

        resp = response::from_request(req);

        if (req.method == "set") {
            set_parameter(req.uri, req.value, '.');
        } else if (req.method == "subscribe") {
            observe(req.uri, std::bind(&websocket_context::notify, this, _1, _2), '.');
        } else if (req.method == "unsubscribe") {
            unobserve(req.uri, '.');
        } else if (req.method == "get") {
            resp.result = get_parameter(req.uri, '.');
        } else
            throw unknown_operation_error();
    } catch (decof_error& ex) {
        resp.error_code = make_error_code(error_code_from_exception(ex));
    } catch (...) {
        resp.error_code = make_error_code(error::internal_error);
    }

    inbuf_.consume(inbuf_.size());

    beast::ostream(outbuf_) << resp;

    async_write_message();
}

void websocket_context::preload_writing()
{
    assert(writing_active_ == false);

    if (response_pending_) {
        process_request();
        return;
    }

    if (! pending_updates_.empty()) {
        update_container::key_type uri;
        update_container::time_point time;
        boost::any any_value;

        std::tie(uri, any_value, time) = pending_updates_.pop_front();

        std::replace(uri.begin(), uri.end(), ':', '.');
        request req{ "publish", uri, any_value };
        beast::ostream(outbuf_) << req;

        async_write_message();
    }
}

void websocket_context::notify(const std::string& uri, const boost::any& any_value)
{
    pending_updates_.push(uri, any_value);

    if (! writing_active_)
        preload_writing();
}

void websocket_context::shutdown(const beast::error_code& error)
{
    if (error != beast::websocket::error::closed)
        stream_.async_close(beast::websocket::close_code::none, [](const beast::error_code&) {});

    // Remove this client context from object dictionary. Because it is a
    // shared pointer, it gets deleted after leaving function scope.
    auto sptr = shared_from_this();
    object_dictionary_.remove_context(sptr);
}

} // namespace websocket

} // namespace decof
