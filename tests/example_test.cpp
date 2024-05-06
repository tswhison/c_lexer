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

#include <iostream>
#include <list>
#include <string>

#include "tests/tests.h"

// https://google.github.io/googletest/reference/testing.html

// Basic test case.
TEST(example, test_1) { ASSERT_EQ(1, 0 + 1); }

// Basic test fixture.
class MyFixture : public ::testing::Test {
protected:
  MyFixture() = default;

  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(MyFixture, test_2) {
  std::string tcase;
  std::string t;

  GTestCaseName(tcase, t);
  std::cout << "Hello from " << tcase << ":" << t << std::endl;
}

// Value-parameterized fixture.
class ValueParamFixt : public ::testing::TestWithParam<std::string> {
protected:
  ValueParamFixt() = default;

  void SetUp() override {}
  void TearDown() override {}
};

TEST_P(ValueParamFixt, test_3) {
  std::string s = GetParam();

  std::cout << "Hello from ValueParamFixt(" << s << ")" << std::endl;
}

// ::testing::Range(begin, end [, step])
// ::testing::Values(v1, v2, v3)
// ::testing::ValuesIn(STL container), ::testing::ValuesIn(STL iter begin, STL
// iter end)
// ::testing::Bool()
// ::testing::Combine(g1, g2, ..., gN)
// ::testing::ConvertGenerator<T>(g)
INSTANTIATE_TEST_SUITE_P(first, ValueParamFixt,
                         ::testing::Values("a", "b", "c"));

// Typed fixture.
template <typename T> class TypedFixt1 : public ::testing::Test {
protected:
  TypedFixt1() : value_(0) {}

  void SetUp() override {}
  void TearDown() override {}

  using List = std::list<T>;
  static T shared_;
  T value_;
};

template <typename T> T TypedFixt1<T>::shared_(42);

struct MyStruct {
  int a_;
  explicit MyStruct(int a) : a_(a) {}
  MyStruct &operator+=(const MyStruct &other) {
    a_ += other.a_;
    return *this;
  }
};

using MyTypes1 = ::testing::Types<int, MyStruct>;
TYPED_TEST_SUITE(TypedFixt1, MyTypes1);

TYPED_TEST(TypedFixt1, test_4) {
  TypeParam n = this->value_;

  n += TestFixture::shared_;

  typename TestFixture::List values;

  values.push_back(n);
}

// Type-parameterized fixture.
template <typename T> class TypeParamFixt : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}

  void DoSomethingInteresting(T t) {
    std::cout << t << " interesting" << std::endl;
  }
};

TYPED_TEST_SUITE_P(TypeParamFixt);

TYPED_TEST_P(TypeParamFixt, test_5) {
  TypeParam n = 'e';

  this->DoSomethingInteresting(n);
}

TYPED_TEST_P(TypeParamFixt, HasPropertyA) {}

REGISTER_TYPED_TEST_SUITE_P(TypeParamFixt, test_5, HasPropertyA);

using MyTypes2 = ::testing::Types<char, int, unsigned int>;
INSTANTIATE_TYPED_TEST_SUITE_P(first, TypeParamFixt, MyTypes2);

// If using only one type..
// INSTANTIATE_TYPED_TEST_SUITE_P(first, TypeParamFixt, int);

// Friend tests.
class Foo {
public:
  Foo() = default;

private:
  FRIEND_TEST(FooTest, BarReturnsZeroOnNull);

  int bar(void *p) {
    if (!p)
      return 0;
    return 1;
  }
};

TEST(FooTest, BarReturnsZeroOnNull) {
  Foo f;
  EXPECT_EQ(f.bar(NULL), 0);
}
