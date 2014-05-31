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

#include "exceptions.h"

namespace decof
{

access_denied_error::access_denied_error()
 : std::runtime_error("Access denied error")
{}

invalid_parameter_error::invalid_parameter_error()
 : std::runtime_error("Invalid parameter error")
{}

wrong_type_error::wrong_type_error()
 : std::runtime_error("Wrong type error")
{}

invalid_value_error::invalid_value_error()
 : std::runtime_error("Invalid value error")
{}

} // namespace decof
