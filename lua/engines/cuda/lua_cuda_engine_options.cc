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

#include "ppl/nn/engines/cuda/engine_options.h"
#include "luacpp/luacpp.h"
#include <memory>
using namespace std;
using namespace luacpp;

namespace ppl { namespace nn { namespace lua {

void RegisterCudaEngineOptions(const shared_ptr<LuaState>& lstate, const shared_ptr<LuaTable>& lmodule) {
    auto lclass = lstate->CreateClass<cuda::EngineOptions>()
        .DefConstructor()
        .DefMember<uint32_t>("device_id",
                             [](const cuda::EngineOptions* options) -> uint32_t {
                                 return options->device_id;
                             },
                             [](cuda::EngineOptions* options, uint32_t v) -> void {
                                 options->device_id = v;
                             })
        .DefMember<uint32_t>("mm_policy",
                             [](const cuda::EngineOptions* options) -> uint32_t {
                                 return options->mm_policy;
                             },
                             [](cuda::EngineOptions* options, uint32_t v) -> void {
                                 options->mm_policy = v;
                             });
    lmodule->Set("EngineOptions", lclass);

    lmodule->SetInteger("MM_COMPACT", cuda::MM_COMPACT);
    lmodule->SetInteger("MM_BEST_FIT", cuda::MM_BEST_FIT);
}

}}}
