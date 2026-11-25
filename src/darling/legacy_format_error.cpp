//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// DARLING: std::format_error's destructor, vtable and type info. Up to LLVM 15
// the destructor was the key function of the class and was defined in the
// dylib (src/format.cpp), so binaries built against those headers bind to the
// vtable and type info exported from libc++.1.dylib. The current headers
// define the destructor inline, and with it every translation unit emits its
// own weak copies; this file keeps the dylib exporting them. It declares the
// class as LLVM 15 did and therefore must not include <format>.

#include <__config>
#include <stdexcept>

_LIBCPP_BEGIN_NAMESPACE_STD

class _LIBCPP_EXPORTED_FROM_ABI format_error : public runtime_error {
public:
  explicit format_error(const char* __s);
  ~format_error() noexcept override;
};

format_error::~format_error() noexcept = default;

_LIBCPP_END_NAMESPACE_STD
