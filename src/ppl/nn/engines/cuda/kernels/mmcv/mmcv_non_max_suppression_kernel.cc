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

#include "ppl/nn/engines/cuda/kernels/mmcv/mmcv_non_max_suppression_kernel.h"
#include "ppl/nn/utils/destructor.h"
#include "cudakernel/nn/mmcv_nms.h"

namespace ppl { namespace nn { namespace cuda {

uint64_t MMCVNonMaxSuppressionKernel::CalcTmpBufferSize(const KernelExecContext& ctx) const {
    auto boxes = ctx.GetInput<TensorImpl>(0);
    return PPLMMCVNMSGetTempBufferSize(boxes->GetShape());
}

ppl::common::RetCode MMCVNonMaxSuppressionKernel::DoExecute(KernelExecContext* ctx) {
    BufferDesc tmp_buffer_desc;
    auto tmp_buffer_bytes = CalcTmpBufferSize(*ctx);
    auto status = GetCudaDevice()->AllocTmpBuffer(tmp_buffer_bytes, &tmp_buffer_desc);
    if (status != ppl::common::RC_SUCCESS) {
        LOG(ERROR) << "alloc tmp buffer size[" << tmp_buffer_bytes << "] for kernel[" << GetName()
                   << "] failed: " << ppl::common::GetRetCodeStr(status);
        return status;
    }
    utils::Destructor __tmp_buffer_guard([this, &tmp_buffer_desc]() -> void {
        GetCudaDevice()->FreeTmpBuffer(&tmp_buffer_desc);
    });
    auto tmp_buffer = tmp_buffer_desc.addr;

    auto boxes = ctx->GetInput<TensorImpl>(0);
    auto scores = ctx->GetInput<TensorImpl>(1);
    auto output = ctx->GetOutput<TensorImpl>(0);

    int device_id = GetCudaDevice()->GetDeviceId();
    status = PPLCUDAMMCVNMSForwardImp(GetStream(), boxes->GetShape(), boxes->GetBufferPtr(), scores->GetShape(),
                                      scores->GetBufferPtr(), output->GetShape(), output->GetBufferPtr<int64_t>(),
                                      tmp_buffer, tmp_buffer_bytes, device_id, param_->iou_threshold, param_->offset);

    return status;
}

}}} // namespace ppl::nn::cuda
