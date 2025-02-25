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

#include "ppl/kernel/riscv/common/relation/relation_eltwise_common.h"
#include "ppl/kernel/riscv/common/relation/relation_broadcast_ndarray_common.h"
#include "ppl/kernel/riscv/common/relation/relation_broadcast_nbcx_common.h"

namespace ppl { namespace kernel { namespace riscv {

template <relation_op_type_t op, int32_t vlen>
ppl::common::RetCode relation_int64(
    const ppl::nn::TensorShape *src0_shape,
    const ppl::nn::TensorShape *src1_shape,
    const ppl::nn::TensorShape *dst_shape,
    const int64_t *src0,
    const int64_t *src1,
    uint8_t *dst)
{
    const bool is_eltwise = src0_shape->GetElementsExcludingPadding() == dst_shape->GetElementsExcludingPadding() &&
                            src1_shape->GetElementsExcludingPadding() == dst_shape->GetElementsExcludingPadding();

    if (is_eltwise) {
        return relation_eltwise_scalar_common<op, int64_t, vlen>(dst_shape, src0, src1, dst);
    } else if (ppl::common::DATAFORMAT_NDARRAY == src0_shape->GetDataFormat()) {
        return relation_broadcast_ndarray_scalar_common<op, int64_t, vlen>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
    } else if (ppl::common::DATAFORMAT_N8CX == src0_shape->GetDataFormat()) {
        return relation_broadcast_nbcx_common<op, int64_t, vlen>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
    }

    return ppl::common::RC_UNSUPPORTED;
}

ppl::common::RetCode greater_int64(
    const ppl::nn::TensorShape *src0_shape,
    const ppl::nn::TensorShape *src1_shape,
    const ppl::nn::TensorShape *dst_shape,
    const int64_t *src0,
    const int64_t *src1,
    uint8_t *dst)
{
    return relation_int64<RELATION_GREATER, 128>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
}

ppl::common::RetCode greater_or_equal_int64(
    const ppl::nn::TensorShape *src0_shape,
    const ppl::nn::TensorShape *src1_shape,
    const ppl::nn::TensorShape *dst_shape,
    const int64_t *src0,
    const int64_t *src1,
    uint8_t *dst)
{
    return relation_int64<RELATION_GREATER_OR_EQUAL, 128>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
}

ppl::common::RetCode less_int64(
    const ppl::nn::TensorShape *src0_shape,
    const ppl::nn::TensorShape *src1_shape,
    const ppl::nn::TensorShape *dst_shape,
    const int64_t *src0,
    const int64_t *src1,
    uint8_t *dst)
{
    return relation_int64<RELATION_LESS, 128>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
}

ppl::common::RetCode less_or_equal_int64(
    const ppl::nn::TensorShape *src0_shape,
    const ppl::nn::TensorShape *src1_shape,
    const ppl::nn::TensorShape *dst_shape,
    const int64_t *src0,
    const int64_t *src1,
    uint8_t *dst)
{
    return relation_int64<RELATION_LESS_OR_EQUAL, 128>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
}

ppl::common::RetCode equal_int64(
    const ppl::nn::TensorShape *src0_shape,
    const ppl::nn::TensorShape *src1_shape,
    const ppl::nn::TensorShape *dst_shape,
    const int64_t *src0,
    const int64_t *src1,
    uint8_t *dst)
{
    return relation_int64<RELATION_EQUAL, 128>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
}

ppl::common::RetCode not_equal_int64(
    const ppl::nn::TensorShape *src0_shape,
    const ppl::nn::TensorShape *src1_shape,
    const ppl::nn::TensorShape *dst_shape,
    const int64_t *src0,
    const int64_t *src1,
    uint8_t *dst)
{
    return relation_int64<RELATION_NOT_EQUAL, 128>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
}

}}}; // namespace ppl::kernel::riscv
