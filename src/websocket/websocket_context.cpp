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

#include <beast/core/to_string.hpp>
#include <beast/websocket/option.hpp>

#include <boost/asio/buffers_iterator.hpp>

#include <decof/exceptions.h>
#include <decof/object_dictionary.h>
#include <decof/websocket/websocket_context.h>

#include "errors.h"
#include "request.h"
#include "response.h"

namespace decof
{

namespace websocket
{

websocket_context::websocket_context(websocket_context::socket_type&& socket, decof::object_dictionary& od, decof::userlevel_t userlevel) :
    client_context(od, userlevel),
    stream_(std::move(socket))
{
    stream_.set_option(beast::websocket::read_message_max(1024*1024));
}

std::string websocket_context::connection_type() const
{
    return "websocket";
}

std::string websocket_context::remote_endpoint() const
{
    // TODO: Needs a fix in Beast (PR#277).
    return "";
}

void websocket_context::preload()
{
    auto self(std::dynamic_pointer_cast<websocket_context>(shared_from_this()));
    stream_.async_accept([self](const beast::error_code& error) {
        if (error) {
            self->shutdown();
            return;
        }

        self->async_read_message();
    });
}

void websocket_context::async_read_message()
{
    if (reading_active_ || response_pending_)
        return;

    // TODO
    beast::websocket::opcode opcode;
    auto self(std::dynamic_pointer_cast<websocket_context>(shared_from_this()));

    reading_active_ = true;

    stream_.async_read(opcode, inbuf_, [self](const beast::error_code& error) {
        self->read_handler(error);
    });
}

void websocket_context::async_write_message()
{
    if (writing_active_)
        return;

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
        shutdown();
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
        shutdown();
        return;
    }

    if (response_pending_) {
        process_request();
        return;
    }

    async_read_message();
}

void websocket_context::process_request()
{
    request req;
    response resp;

    response_pending_ = false;

    try {
        using std::placeholders::_1;
        using std::placeholders::_2;
        using boost::asio::buffers_begin;
        using boost::asio::buffers_end;

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

    } catch (parse_error&) {
        resp.error_code = make_error_code(error_codes::parse_error);
    } catch (bad_request_error&) {
        resp.error_code = make_error_code(error_codes::invalid_request);
    } catch (unknown_operation_error&) {
        resp.error_code = make_error_code(error_codes::method_not_found);
    } catch (invalid_params_error&) {
        resp.error_code = make_error_code(error_codes::invalid_params);
    } catch (access_denied_error&) {
        resp.error_code = make_error_code(error_codes::access_denied_error);
    } catch (...) {
        // TODO: Rework exception design.
        resp.error_code = make_error_code(error_codes::internal_error);
    }

    inbuf_.consume(inbuf_.size());

    std::ostream(&outbuf_) << resp;

    async_write_message();
}

void websocket_context::notify(const std::string& uri, const boost::any& any_value)
{
    // TODO: Update container
    if (! writing_active_) {
        request req{ "publish", uri, any_value };
        std::ostream(&outbuf_) << req;
        async_write_message();
    }
}

void websocket_context::shutdown()
{
    // Close underlying socket gracefully (if still open) in order to terminate
    // pending asynchronous operations.
//    auto& sock = stream_.next_layer();
//    boost::system::error_code ec;
//    sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
//    sock.close(ec);

    // Remove this client context from object dictionary. Because it is a
    // shared pointer, it gets deleted after leaving function scope.
    auto sptr = shared_from_this();
    object_dictionary_.remove_context(sptr);
}

} // namespace websocket

} // namespace decof
