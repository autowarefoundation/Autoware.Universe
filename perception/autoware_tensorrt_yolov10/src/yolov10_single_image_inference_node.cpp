// Copyright 2022 Tier IV, Inc.
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

#include <autoware/tensorrt_yolov10/tensorrt_yolov10.hpp>
#include <rclcpp/rclcpp.hpp>

#if (defined(_MSC_VER) or (defined(__GNUC__) and (7 <= __GNUC_MAJOR__)))
#include <filesystem>
namespace fs = ::std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = ::std::experimental::filesystem;
#endif

#include <memory>
#include <string>

// using namespace std;

#define PRINT_DEBUG_INFO printf("line:%d\n", __LINE__);

namespace autoware::tensorrt_yolov10
{
class Yolov10SingleImageInferenceNode : public rclcpp::Node
{
public:
  explicit Yolov10SingleImageInferenceNode(const rclcpp::NodeOptions & node_options)
  : Node("yolov10_single_image_inference", node_options)
  {
    const auto image_path = declare_parameter("image_path", "");
    const auto model_path = declare_parameter("model_path", "");
    const auto precision = declare_parameter("precision", "fp16");
    const auto save_image = declare_parameter("save_image", true);

    printf("image_path:%s\n", image_path.c_str());
    auto p = fs::path(image_path);
    const auto ext = p.extension().string();
    p.replace_extension("");
    const auto output_image_path =
      declare_parameter("output_image_path", p.string() + "_detect" + ext);

    printf("model_path:%s,output_image_path:%s\n", model_path.c_str(), output_image_path.c_str());
    auto trt_yolov10 = std::make_unique<tensorrt_yolov10::TrtYolov10>(model_path, precision);
    auto image = cv::imread(image_path);
    tensorrt_yolov10::ObjectArrays objects;

    trt_yolov10->doInference({image}, objects);
    for (const auto & object : objects[0]) {
      const auto left = object.x_offset;
      const auto top = object.y_offset;
      const auto right = std::clamp(left + object.width, 0, image.cols);
      const auto bottom = std::clamp(top + object.height, 0, image.rows);
      cv::rectangle(
        image, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 0, 255), 3, 8, 0);

      std::string cls_id = std::to_string(object.type);
      cv::putText(
        image, cls_id, cv::Point(left, top), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 255),
        1, 8, 0);
    }
    if (!save_image) {
      cv::imshow("inference image", image);
      cv::waitKey(0);
      rclcpp::shutdown();
    }
    cv::imwrite(output_image_path, image);
    rclcpp::shutdown();
  }
};
}  // namespace autoware::tensorrt_yolov10

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::tensorrt_yolov10::Yolov10SingleImageInferenceNode)
