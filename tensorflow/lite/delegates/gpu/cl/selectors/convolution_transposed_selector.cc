/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/delegates/gpu/cl/selectors/convolution_transposed_selector.h"

#include "absl/memory/memory.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/convolution_transposed.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/convolution_transposed_3x3.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/convolution_transposed_3x3_thin.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/convolution_transposed_4x4.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/convolution_transposed_thin.h"
#include "tensorflow/lite/delegates/gpu/common/task/tensor_desc.h"

namespace tflite {
namespace gpu {
namespace cl {
namespace {

std::unique_ptr<GPUOperation> SelectConvolutionTransposedAdreno(
    const ConvolutionTransposedAttributes& attr, const GpuInfo& gpu_info,
    const OperationDef& op_def) {
  if (IsConvolutionTransposedThinSupported(attr)) {
    ConvolutionTransposedThin conv =
        CreateConvolutionTransposedThin(gpu_info, op_def, attr);
    return absl::make_unique<ConvolutionTransposedThin>(std::move(conv));
  } else if (IsConvolutionTransposed3x3ThinSupported(attr)) {
    ConvolutionTransposed3x3Thin conv =
        CreateConvolutionTransposed3x3Thin(gpu_info, op_def, attr);
    return absl::make_unique<ConvolutionTransposed3x3Thin>(std::move(conv));
  } else {
    ConvolutionTransposed conv =
        CreateConvolutionTransposed(gpu_info, op_def, attr);
    return absl::make_unique<ConvolutionTransposed>(std::move(conv));
  }
}

std::unique_ptr<GPUOperation> SelectConvolutionTransposedPowerVR(
    const ConvolutionTransposedAttributes& attr, const GpuInfo& gpu_info,
    const OperationDef& op_def) {
  if (IsConvolutionTransposedThinSupported(attr)) {
    ConvolutionTransposedThin conv =
        CreateConvolutionTransposedThin(gpu_info, op_def, attr);
    return absl::make_unique<ConvolutionTransposedThin>(std::move(conv));
  } else if (IsConvolutionTransposed3x3ThinSupported(attr)) {
    ConvolutionTransposed3x3Thin conv =
        CreateConvolutionTransposed3x3Thin(gpu_info, op_def, attr);
    return absl::make_unique<ConvolutionTransposed3x3Thin>(std::move(conv));
  } else if (IsConvolutionTransposed3x3Supported(op_def, attr)) {
    ConvolutionTransposed3x3 conv =
        CreateConvolutionTransposed3x3(gpu_info, op_def, attr);
    return absl::make_unique<ConvolutionTransposed3x3>(std::move(conv));
  } else if (IsConvolutionTransposed4x4Supported(op_def, attr)) {
    ConvolutionTransposed4x4 conv =
        CreateConvolutionTransposed4x4(gpu_info, op_def, attr);
    return absl::make_unique<ConvolutionTransposed4x4>(std::move(conv));
  } else {
    ConvolutionTransposed conv =
        CreateConvolutionTransposed(gpu_info, op_def, attr);
    return absl::make_unique<ConvolutionTransposed>(std::move(conv));
  }
}

std::unique_ptr<GPUOperation> SelectConvolutionTransposedMali(
    const ConvolutionTransposedAttributes& attr, const GpuInfo& gpu_info,
    const OperationDef& op_def) {
  ConvolutionTransposed conv =
      CreateConvolutionTransposed(gpu_info, op_def, attr);
  return absl::make_unique<ConvolutionTransposed>(std::move(conv));
}
}  // namespace

std::unique_ptr<GPUOperation> SelectConvolutionTransposed(
    const ConvolutionTransposedAttributes& attr, const GpuInfo& gpu_info,
    const OperationDef& op_def) {
  if (gpu_info.IsAdreno()) {
    return SelectConvolutionTransposedAdreno(attr, gpu_info, op_def);
  } else if (gpu_info.IsPowerVR() || gpu_info.IsAMD() ||
             gpu_info.IsNvidia() || gpu_info.IsIntel()) {
    return SelectConvolutionTransposedPowerVR(attr, gpu_info, op_def);
  } else if (gpu_info.IsMali()) {
    return SelectConvolutionTransposedMali(attr, gpu_info, op_def);
  } else {
    return SelectConvolutionTransposedAdreno(attr, gpu_info, op_def);
  }
}

}  // namespace cl
}  // namespace gpu
}  // namespace tflite
