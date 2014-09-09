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

#ifndef DECOF_H
#define DECOF_H

#include "decof/event.h"
#include "decof/exceptions.h"
#include "decof/external_readonly_parameter.h"
#include "decof/external_readwrite_parameter.h"
#include "decof/object_dictionary.h"
#include "decof/managed_readwrite_parameter.h"
#include "decof/managed_readonly_parameter.h"
#include "decof/tree_element.h"
#include "decof/types.h"

#define DECOF_DECLARE_EVENT(name)                                           \
    struct name##_event : public decof::event                               \
    {                                                                       \
        using event::event;                                                 \
        virtual void signal() override final;                               \
    }

#endif // DECOF_H
