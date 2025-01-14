# autoware_tensorrt_common

This package provides a high-level API to work with TensorRT. This library simplifies the process of loading, building, and executing TensorRT inference engines using ONNX models. It also includes utilities for profiling and managing TensorRT execution contexts, making it easier to integrate TensorRT-based packages in Autoware.

## Usage

Here is an example usage of the library. For the full API documentation, please refer to the doxygen documentation (see header [file](include/autoware/tensorrt_common/tensorrt_common.hpp)).

```c++
#include <autoware/tensorrt_common/tensorrt_common.hpp>

#include <memory>
#include <utility>
#include <vector>

using autoware::tensorrt_common::TrtCommon;
using autoware::tensorrt_common::TrtCommonConfig;
using autoware::tensorrt_common::TensorInfo;
using autoware::tensorrt_common::NetworkIO;
using autoware::tensorrt_common::ProfileDims;

std::unique_ptr<TrtCommon> trt_common_;
```

### Create a tensorrt common instance and setup engine

- With minimal configuration.

```c++
trt_common_ = std::make_unique<TrtCommon>(TrtCommonConfig("/path/to/onnx/model.onnx"));
trt_common_->setup();
```

- With full configuration.

```c++
trt_common_ = std::make_unique<TrtCommon>(TrtCommonConfig("/path/to/onnx/model.onnx", "fp16", "/path/to/engine/model.engine", (1ULL << 30U), -1, false));

std::vector<NetworkIO> network_io{
  NetworkIO("sample_input", {3, {-1, 64, 512}}), NetworkIO("sample_output", {1, {50}})};
std::vector<ProfileDims> profile_dims{
  ProfileDims("sample_input", {3, {1, 64, 512}}, {3, {3, 64, 512}}, {3, {9, 64, 512}})};

auto network_io_ptr = std::make_unique<std::vector<NetworkIO>>(network_io);
auto profile_dims_ptr = std::make_unique<std::vector<ProfileDims>>(profile_dims);

trt_common_->setup(std::move(profile_dims_ptr), std::move(network_io_ptr));
```

By defining network IO names and dimensions, an extra shapes validation will be performed after building / loading engine. This is useful to ensure the model is compatible with current code for preprocessing as it might consists of operations dependent on tensor shapes.

Profile dimension is used to specify the min, opt, and max dimensions for dynamic shapes.

Network IO or / and profile dimensions can be omitted if not needed.

### Setting input and output tensors

```c++
bool success = true;
success &= trt_common_->setTensor("sample_input", sample_input_d_.get(), nvinfer1::Dims{3, {var_size, 64, 512}});
success &= trt_common_->setTensor("sample_output", sample_output_d_.get());
return success;
```

### Execute inference

```c++
auto success = trt_common_->enqueueV3(stream_);
return success;
```
