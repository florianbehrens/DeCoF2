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

#include <decof/asio_executor/asio_executor.h>

namespace decof {

asio_executor::asio_executor(asio_executor::strand& s) :
    strand_(s)
{}

void asio_executor::close()
{
    closed_ = true;
}

bool asio_executor::closed()
{
    return closed_;
}

bool asio_executor::try_executing_one()
{
    return false;
}

} // namespace decof
