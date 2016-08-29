/*
 * Copyright (c) 2016 Spotify AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include <boost/test/unit_test.hpp>

#include <spotify/json/detail/stack.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

namespace {

template <typename stack_type>
void push_many(stack_type &stack, int count) {
  for (int i = 0; i < count; i++) {
    stack.push(i);
  }
}

template <typename stack_type>
void pop_many_and_verify(stack_type &stack, int count) {
  for (int i = count - 1; i >= 0; i--) {
    BOOST_REQUIRE_EQUAL(stack.pop(), i);
  }
}

template <typename stack_type>
void push_pop_many_and_verify(stack_type &stack, int count) {
  push_many(stack, count);
  pop_many_and_verify(stack, count);
}

}  // namespace

BOOST_AUTO_TEST_CASE(stack_should_push_and_pop_within_inline_size) {
  stack<int, 32> stack;
  push_pop_many_and_verify(stack, 32);
}

BOOST_AUTO_TEST_CASE(stack_should_push_and_pop_outside_inline_size) {
  stack<int, 32> stack;
  push_pop_many_and_verify(stack, 1024);
}

BOOST_AUTO_TEST_CASE(stack_should_push_and_pop_more_than_once) {
  stack<int, 32> stack;
  push_pop_many_and_verify(stack, 32);
  push_pop_many_and_verify(stack, 64);
  push_pop_many_and_verify(stack, 96);
  push_pop_many_and_verify(stack, 64);
  push_pop_many_and_verify(stack, 32);
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
