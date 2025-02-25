// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "ppl/nn/engines/cuda/optimizer/ops/onnx/concat_op.h"

#include "ppl/nn/common/logger.h"
#include "ppl/nn/engines/cuda/kernels/onnx/concat_kernel.h"
#include "ppl/nn/oputils/onnx/reshape_concat.h"

using namespace std;
using namespace ppl::common;
using namespace ppl::nn::onnx;

namespace ppl { namespace nn { namespace cuda {

RetCode ConcatOp::Init(const OptKernelOptions& options) {
    auto status = GenericLoadParam<ConcatParam>(options, &param_.param);
    if (status != RC_SUCCESS) {
        LOG(ERROR) << "load param failed: " << GetRetCodeStr(status);
        return status;
    }

    infer_type_func_ = [](InputOutputInfo* info, std::vector<CudaTensorQuant>* quant, datatype_t type) -> RetCode {
        ppl::common::RetCode status;
        if (type == DATATYPE_UNKNOWN) {
            status = InferHighestType(info);
        } else if (type == DATATYPE_INT8) {
            status = UnifyToOutputQuant(info, quant);
        } else {
            status = InferDefaultType(info, type);
        }
        return status;
    };

    infer_dims_func_ = [this](InputOutputInfo* info) -> RetCode {
        return onnx::ReshapeConcat(info, &param_);
    };

    infer_unsafe_dims_func_ = [](InputOutputInfo* info, std::set<uint32_t>* illegal_inputs) -> RetCode {
        auto in_shape0 = info->GetInput<TensorImpl>(0)->GetShape();
        for (uint32_t i = 0; i < info->GetInputCount(); ++i) {
            if (illegal_inputs->find(i) != illegal_inputs->end()) {
                in_shape0 = info->GetInput<TensorImpl>(i)->GetShape();
                break;
            }
        }

        for (uint32_t i = 0; i < info->GetOutputCount(); ++i) {
            auto out_shape = info->GetOutput<TensorImpl>(i)->GetShape();
            out_shape->Reshape(in_shape0->GetDims(), in_shape0->GetRealDimCount());
        }
        return ppl::common::RC_SUCCESS;
    };

    return RC_SUCCESS;
}

RetCode ConcatOp::Finalize(const OptKernelOptions& options) {
    auto status = SetCommonParam(options);
    if (status != RC_SUCCESS) {
        LOG(ERROR) << "load common param failed: " << GetRetCodeStr(status);
        return status;
    }

    return RC_SUCCESS;
}

KernelImpl* ConcatOp::CreateKernelImpl() const {
    return CreateKernelImplWithParam<ConcatKernel>(&param_);
}

}}} // namespace ppl::nn::cuda
