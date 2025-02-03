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

#ifndef AUTOWARE__TENSORRT_PLUGINS__PLUGIN_REGISTRATION_HPP_
#define AUTOWARE__TENSORRT_PLUGINS__PLUGIN_REGISTRATION_HPP_

#include <NvInferRuntime.h>

#include <cstdint>

// These are the functions that TensorRT library will call at the runtime.

extern "C" void setLoggerFinder(nvinfer1::ILoggerFinder * finder);

extern "C" nvinfer1::IPluginCreatorInterface * const * getPluginCreators(
  std::int32_t & num_creators);

#endif  // AUTOWARE__TENSORRT_PLUGINS__PLUGIN_REGISTRATION_HPP_
