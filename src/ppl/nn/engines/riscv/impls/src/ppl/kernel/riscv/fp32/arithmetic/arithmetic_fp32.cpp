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

#include "ppl/kernel/riscv/fp32/arithmetic/arithmetic_eltwise_fp32.h"
#include "ppl/kernel/riscv/fp32/arithmetic/arithmetic_broadcast_n4cx_fp32.h"
#include "ppl/kernel/riscv/fp32/arithmetic/arithmetic_broadcast_ndarray_fp32.h"
namespace ppl { namespace kernel { namespace riscv {

template <arithmetic_op_type_t _op, bool fuse_relu>
static ppl::common::RetCode arithmetic_fp32(
    const ppl::nn::TensorShape* src0_shape,
    const ppl::nn::TensorShape* src1_shape,
    const ppl::nn::TensorShape* dst_shape,

    const float* src0,
    const float* src1,
    float* dst)
{
    bool is_eltwise = src0_shape->GetElementsExcludingPadding() == dst_shape->GetElementsExcludingPadding() &&
                      src1_shape->GetElementsExcludingPadding() == dst_shape->GetElementsExcludingPadding();
    if (is_eltwise) {
        return arithmetic_eltwise_fp32<_op, fuse_relu>(dst_shape, src0, src1, dst);
    } else if (dst_shape->GetDataFormat() == ppl::common::DATAFORMAT_NDARRAY) {
        return arithmetic_broadcast_ndarray_fp32<_op, fuse_relu>(src0, src1, dst, src0_shape, src1_shape, dst_shape);
    } else if (dst_shape->GetDataFormat() == ppl::common::DATAFORMAT_N4CX) {
        return arithmetic_broadcast_n4cx_fp32<_op, fuse_relu>(src0, src1, dst, src0_shape, src1_shape, dst_shape, 1);
    }

    return ppl::common::RC_UNSUPPORTED;
}

ppl::common::RetCode add_fp32(
    const ppl::nn::TensorShape* src0_shape,
    const ppl::nn::TensorShape* src1_shape,
    const ppl::nn::TensorShape* dst_shape,
    const bool fuse_relu,
    const float* src0,
    const float* src1,
    float* dst)
{
    if (fuse_relu) {
        return arithmetic_fp32<ARITHMETIC_ADD, true>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
    } else {
        return arithmetic_fp32<ARITHMETIC_ADD, false>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
    }
}

ppl::common::RetCode sub_fp32(
    const ppl::nn::TensorShape* src0_shape,
    const ppl::nn::TensorShape* src1_shape,
    const ppl::nn::TensorShape* dst_shape,
    const bool fuse_relu,
    const float* src0,
    const float* src1,
    float* dst)
{
    if (fuse_relu) {
        return arithmetic_fp32<ARITHMETIC_SUB, true>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
    } else {
        return arithmetic_fp32<ARITHMETIC_SUB, false>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
    }
}

ppl::common::RetCode mul_fp32(
    const ppl::nn::TensorShape* src0_shape,
    const ppl::nn::TensorShape* src1_shape,
    const ppl::nn::TensorShape* dst_shape,
    const bool fuse_relu,
    const float* src0,
    const float* src1,
    float* dst)
{
    if (fuse_relu) {
        return arithmetic_fp32<ARITHMETIC_MUL, true>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
    } else {
        return arithmetic_fp32<ARITHMETIC_MUL, false>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
    }
}

ppl::common::RetCode div_fp32(
    const ppl::nn::TensorShape* src0_shape,
    const ppl::nn::TensorShape* src1_shape,
    const ppl::nn::TensorShape* dst_shape,
    const bool fuse_relu,
    const float* src0,
    const float* src1,
    float* dst)
{
    if (fuse_relu) {
        return arithmetic_fp32<ARITHMETIC_DIV, true>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
    } else {
        return arithmetic_fp32<ARITHMETIC_DIV, false>(src0_shape, src1_shape, dst_shape, src0, src1, dst);
    }
}

}}}; //  namespace ppl::kernel::riscv
