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

#ifndef _ST_HPC_PPL_NN_PARAMS_ONNX_PAD_PARAM_H_
#define _ST_HPC_PPL_NN_PARAMS_ONNX_PAD_PARAM_H_

#include "ppl/nn/ir/attr.h"
#include <stdint.h>
#include <vector>

namespace ppl { namespace nn { namespace onnx {

#define PAD_PARAM_MAX_DIM_SIZE 5

struct PadParam final : public ir::TypedAttr<PadParam> {
    typedef uint32_t pad_mode_t;
    enum { PAD_MODE_CONSTANT = 0, PAD_MODE_REFLECT = 1, PAD_MODE_EDGE = 2 };

    pad_mode_t mode = PAD_MODE_CONSTANT;
    std::vector<int32_t> pads;
    float value;

    bool operator==(const PadParam& p) const {
        return this->mode == p.mode && this->value == p.value && this->pads == p.pads;
    }
};

}}} // namespace ppl::nn::onnx

#endif
