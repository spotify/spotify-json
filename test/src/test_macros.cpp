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

#include <limits>

#include <boost/test/unit_test.hpp>

#include <spotify/json/detail/macros.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

/*
 * json_haszero_n
 */

BOOST_AUTO_TEST_CASE(json_haszero_1_should_only_only_find_zeros) {
  for (uint8_t i = 0; i < std::numeric_limits<uint8_t>::max(); i++) {
    BOOST_CHECK_EQUAL(!!json_haszero_1(i), i == 0);
  }
}

BOOST_AUTO_TEST_CASE(json_haszero_2_should_only_only_find_zeros) {
  union { uint8_t c[2]; uint16_t i; } u;
  for (u.i = 0; u.i < std::numeric_limits<uint16_t>::max(); u.i++) {
    const bool has_zero = !u.c[0] || !u.c[1];
    BOOST_CHECK_EQUAL(!!json_haszero_2(u.i), has_zero);
  }
}

BOOST_AUTO_TEST_CASE(json_haszero_4_should_only_only_find_zeros) {
  BOOST_CHECK(json_haszero_4(0x00000000));
  BOOST_CHECK(json_haszero_4(0x001188FF));
  BOOST_CHECK(json_haszero_4(0x110088FF));
  BOOST_CHECK(json_haszero_4(0x118800FF));
  BOOST_CHECK(json_haszero_4(0x1188FF00));
  BOOST_CHECK(!json_haszero_4(0x1188FF77));
  BOOST_CHECK(!json_haszero_4(0x01020304));
}

BOOST_AUTO_TEST_CASE(json_haszero_8_should_only_only_find_zeros) {
  BOOST_CHECK(json_haszero_8(0x0000000000000000ULL));
  BOOST_CHECK(json_haszero_8(0x001188FF22334455ULL));
  BOOST_CHECK(json_haszero_8(0x110088FF22334455ULL));
  BOOST_CHECK(json_haszero_8(0x118800FF22334455ULL));
  BOOST_CHECK(json_haszero_8(0x1188FF0022334455ULL));
  BOOST_CHECK(json_haszero_8(0x1188FF2200334455ULL));
  BOOST_CHECK(json_haszero_8(0x1188FF2233004455ULL));
  BOOST_CHECK(json_haszero_8(0x1188FF2233440055ULL));
  BOOST_CHECK(json_haszero_8(0x1188FF2233445500ULL));
  BOOST_CHECK(!json_haszero_8(0x1188FF7722334455ULL));
  BOOST_CHECK(!json_haszero_8(0x0102030405060708ULL));
}

/*
 * json_haschar_n
 */

BOOST_AUTO_TEST_CASE(json_haschar_1_should_only_only_find_character) {
  for (uint8_t i = 0; i < std::numeric_limits<uint8_t>::max(); i++) {
    BOOST_CHECK(json_haschar_1(i, i));
    BOOST_CHECK(!json_haschar_1(~i, i));
  }
}

BOOST_AUTO_TEST_CASE(json_haschar_2_should_only_only_find_character) {
  union { uint8_t c[2]; uint16_t i; } u;
  for (u.i = 0; u.i < std::numeric_limits<uint16_t>::max(); u.i++) {
    BOOST_CHECK(json_haschar_2(u.i, u.c[0]));
    BOOST_CHECK(json_haschar_2(u.i, u.c[1]));
    if (u.c[0] && u.c[1]) {
      BOOST_CHECK(!json_haschar_2(0x0000, u.c[0]));
      BOOST_CHECK(!json_haschar_2(0x0000, u.c[1]));
    }
  }
}

BOOST_AUTO_TEST_CASE(json_haschar_4_should_only_only_find_character) {
  BOOST_CHECK( json_haschar_4(0x00000000, 0x00));
  BOOST_CHECK(!json_haschar_4(0x00000000, 0x01));
  BOOST_CHECK( json_haschar_4(0x117788FF, 0x11));
  BOOST_CHECK(!json_haschar_4(0x117788FF, 0x12));
  BOOST_CHECK( json_haschar_4(0x117788FF, 0x77));
  BOOST_CHECK(!json_haschar_4(0x117788FF, 0x78));
  BOOST_CHECK( json_haschar_4(0x117788FF, 0x88));
  BOOST_CHECK(!json_haschar_4(0x117788FF, 0x89));
  BOOST_CHECK( json_haschar_4(0x117788FF, 0xFF));
  BOOST_CHECK(!json_haschar_4(0x117788FF, 0xEF));
}

BOOST_AUTO_TEST_CASE(json_haschar_8_should_only_only_find_character) {
  BOOST_CHECK( json_haschar_8(0x0000000000000000, 0x00));
  BOOST_CHECK(!json_haschar_8(0x0000000000000000, 0x01));
  BOOST_CHECK( json_haschar_8(0x117788FF22334455, 0x11));
  BOOST_CHECK(!json_haschar_8(0x117788FF22334455, 0x12));
  BOOST_CHECK( json_haschar_8(0x117788FF22334455, 0x77));
  BOOST_CHECK(!json_haschar_8(0x117788FF22334455, 0x78));
  BOOST_CHECK( json_haschar_8(0x117788FF22334455, 0x88));
  BOOST_CHECK(!json_haschar_8(0x117788FF22334455, 0x89));
  BOOST_CHECK( json_haschar_8(0x117788FF22334455, 0xFF));
  BOOST_CHECK(!json_haschar_8(0x117788FF22334455, 0xEF));
  BOOST_CHECK( json_haschar_8(0x117788FF22334455, 0x22));
  BOOST_CHECK(!json_haschar_8(0x117788FF22334455, 0x23));
  BOOST_CHECK( json_haschar_8(0x117788FF22334455, 0x33));
  BOOST_CHECK(!json_haschar_8(0x117788FF22334455, 0x34));
  BOOST_CHECK( json_haschar_8(0x117788FF22334455, 0x44));
  BOOST_CHECK(!json_haschar_8(0x117788FF22334455, 0x45));
  BOOST_CHECK( json_haschar_8(0x117788FF22334455, 0x55));
  BOOST_CHECK(!json_haschar_8(0x117788FF22334455, 0x56));
}

/*
 * json_unaligned_n
 */

BOOST_AUTO_TEST_CASE(json_unaligned_1) {
  BOOST_CHECK(!json_unaligned_1(intptr_t(0)));
  BOOST_CHECK(!json_unaligned_1(intptr_t(1)));
  BOOST_CHECK(!json_unaligned_1(intptr_t(2)));
  BOOST_CHECK(!json_unaligned_1(intptr_t(3)));
}

BOOST_AUTO_TEST_CASE(json_unaligned_2) {
  BOOST_CHECK(!json_unaligned_2(intptr_t(0)));
  BOOST_CHECK( json_unaligned_2(intptr_t(1)));
  BOOST_CHECK(!json_unaligned_2(intptr_t(2)));
  BOOST_CHECK( json_unaligned_2(intptr_t(3)));
}

BOOST_AUTO_TEST_CASE(json_unaligned_4) {
  BOOST_CHECK(!json_unaligned_4(intptr_t(0)));
  BOOST_CHECK( json_unaligned_4(intptr_t(1)));
  BOOST_CHECK( json_unaligned_4(intptr_t(2)));
  BOOST_CHECK( json_unaligned_4(intptr_t(3)));
  BOOST_CHECK(!json_unaligned_4(intptr_t(4)));
  BOOST_CHECK( json_unaligned_4(intptr_t(5)));
  BOOST_CHECK( json_unaligned_4(intptr_t(6)));
  BOOST_CHECK( json_unaligned_4(intptr_t(7)));
}

BOOST_AUTO_TEST_CASE(json_unaligned_8) {
  BOOST_CHECK(!json_unaligned_8(intptr_t(0x0)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0x1)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0x2)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0x3)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0x4)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0x5)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0x6)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0x7)));
  BOOST_CHECK(!json_unaligned_8(intptr_t(0x8)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0x9)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0xA)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0xB)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0xC)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0xD)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0xE)));
  BOOST_CHECK( json_unaligned_8(intptr_t(0xF)));
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
