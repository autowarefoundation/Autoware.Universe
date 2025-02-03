// Copyright 2025 TIER IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef AUTOWARE__TENSORRT_PLUGINS__PLUGIN_UTILS_HPP_
#define AUTOWARE__TENSORRT_PLUGINS__PLUGIN_UTILS_HPP_

#include <NvInferRuntime.h>

#include <cstdint>
#include <stdexcept>

void caughtError(std::exception const & e);

void logDebug(char const * msg);

void logInfo(char const * msg);

#define PLUGIN_ASSERT(val) reportAssertion((val), #val, __FILE__, __LINE__)
void reportAssertion(bool success, char const * msg, char const * file, std::int32_t line);

#define PLUGIN_VALIDATE(val) reportValidation((val), #val, __FILE__, __LINE__)
void reportValidation(bool success, char const * msg, char const * file, std::int32_t line);

#endif  // AUTOWARE__TENSORRT_PLUGINS__PLUGIN_UTILS_HPP_
