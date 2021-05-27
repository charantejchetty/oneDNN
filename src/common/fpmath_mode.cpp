/*******************************************************************************
* Copyright 2021 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "c_types_map.hpp"
#include "dnnl.hpp"
#include "utils.hpp"

namespace dnnl {
namespace impl {
static setting_t<fpmath_mode_t> default_fpmath {fpmath_mode::strict};

void init_fpmath_mode() {
    if (!default_fpmath.initialized()) {
        const int len = 10;
        char val[len];
        if (getenv("DNNL_FPMATH_MODE", val, len) > 0) {
            if (std::strcmp(val, "STRICT") == 0)
                default_fpmath.set(fpmath_mode::strict);
            if (std::strcmp(val, "BF16") == 0)
                default_fpmath.set(fpmath_mode::bf16);
            if (std::strcmp(val, "F16") == 0)
                default_fpmath.set(fpmath_mode::f16);
            if (std::strcmp(val, "ANY") == 0)
                default_fpmath.set(fpmath_mode::any);
        }
    }
}

status_t check_fpmath_mode(fpmath_mode_t mode) {
    if (utils::one_of(mode, fpmath_mode::strict,
                fpmath_mode::bf16, fpmath_mode::f16, fpmath_mode::any))
        return status::success;
    else
        return status::invalid_arguments;
}

fpmath_mode_t get_fpmath_mode() {
    init_fpmath_mode();
    auto mode = default_fpmath.get();
    // Should always be proper, since no way to set invalid mode
    assert(check_fpmath_mode(mode) == status::success);
    return mode;
}

} // namespace impl
} // namespace dnnl

dnnl_status_t dnnl_set_default_fpmath_mode(dnnl_fpmath_mode_t mode) {
    using namespace dnnl::impl;
    auto st = check_fpmath_mode(mode);
    if (st == status::success) default_fpmath.set(mode);
    return st;
}

dnnl_status_t dnnl_get_default_fpmath_mode(dnnl_fpmath_mode_t *mode) {
    using namespace dnnl::impl;
    if (mode == nullptr) return status::invalid_arguments;

    auto m = get_fpmath_mode();
    // Should always be proper, since no way to set invalid mode
    auto st = check_fpmath_mode(m);
    if (st == status::success) *mode = m;
    return st;
}