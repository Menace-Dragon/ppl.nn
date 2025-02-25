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

#include "py_pmx_runtime_builder.h"
#include "../../engines/py_engine.h"
#include "ppl/nn/models/pmx/runtime_builder_factory.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
using namespace std;

namespace ppl { namespace nn { namespace python {

class PyPmxRuntimeBuilderFactory final {
public:
    static PyPmxRuntimeBuilder Create() {
        return PyPmxRuntimeBuilder(pmx::RuntimeBuilderFactory::Create());
    }
};

void RegisterPmxRuntimeBuilderFactory(pybind11::module* m) {
    pybind11::class_<PyPmxRuntimeBuilderFactory>(*m, "RuntimeBuilderFactory")
        .def_static("Create", &PyPmxRuntimeBuilderFactory::Create);
}

}}} // namespace ppl::nn::python
