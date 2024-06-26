// MIT License
//
// Copyright (c) 2024 Tim Whisonant
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#pragma once

#include <gtest/gtest.h>

#include <string>
#include <type_traits>

#define ASSERT_NONNULL(x) ASSERT_NE(nullptr, x)
#define ASSERT_NULL ASSERT_EQ(nullptr, x)
#define EXPECT_NONNULL(x) EXPECT_NE(nullptr, x)
#define EXPECT_NULL(x) EXPECT_EQ(nullptr, x)

void GTestCaseName(std::string &TestCase, std::string &Test);

template <typename E>
constexpr std::underlying_type_t<E> EnumToUnsigned(E enumerator) noexcept {
  return static_cast<std::underlying_type_t<E>>(enumerator);
}
