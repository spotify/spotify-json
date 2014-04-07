# Copyright (c) 2013 Felix Bruns <felixbruns@gmail.com>
#       _       _     _ ____                  _  _____         _
#      / \   __| | __| | __ )  ___   ___  ___| ||_   _|__  ___| |_ ___
#     / _ \ / _` |/ _` |  _ \ / _ \ / _ \/ __| __|| |/ _ \/ __| __/ __|
#    / ___ \ (_| | (_| | |_) | (_) | (_) \__ \ |_ | |  __/\__ \ |_\__ \
#   /_/   \_\__,_|\__,_|____/ \___/ \___/|___/\__||_|\___||___/\__|___/
#
# Discover and add Boost Tests to a given target.
#

include(Join)

#
# Usage:
#   ADD_BOOST_TESTS(target source1 source2 ...)
#
# Example:
#   set(TEST_SOURCES
#     test/test1.cpp
#     test/test2.cpp
#     )
#   ADD_BOOST_TESTS(test_exe ${TEST_SOURCES} "test/test3.cpp")
#
function(ADD_BOOST_TESTS TARGET)
  message(STATUS "Discovering Boost Tests...")

  #
  # Number of test cases found in total.
  #
  set(BOOST_NUM_TOTAL_TEST_CASES 0)

  foreach(SOURCE ${ARGN})
    #
    # Number of test cases found.
    #
    set(BOOST_NUM_TEST_CASES 0)

    #
    # Read source file into string and find all Boost Test macros.
    #
    file(READ ${SOURCE} CODE)
    set(BOOST_TEST_MACRO_REGEX "BOOST_(AUTO|FIXTURE)_TEST_(SUITE|CASE)(_TEMPLATE|_END)?[^\n]+")
    string(REGEX MATCHALL ${BOOST_TEST_MACRO_REGEX} BOOST_TEST_MACROS ${CODE})

    #
    # Stack of Boost Test Suite names.
    #
    set(BOOST_TEST_SUITE_STACK "")

    foreach(BOOST_TEST_MACRO ${BOOST_TEST_MACROS})
      string(FIND ${BOOST_TEST_MACRO} "BOOST_AUTO_TEST_CASE(" BOOST_AUTO_TEST_CASE_pos)
      string(FIND ${BOOST_TEST_MACRO} "BOOST_AUTO_TEST_CASE_TEMPLATE(" BOOST_AUTO_TEST_CASE_TEMPLATE_pos)
      string(FIND ${BOOST_TEST_MACRO} "BOOST_FIXTURE_TEST_CASE(" BOOST_FIXTURE_TEST_CASE_pos)
      string(FIND ${BOOST_TEST_MACRO} "BOOST_FIXTURE_TEST_CASE_TEMPLATE(" BOOST_FIXTURE_TEST_CASE_TEMPLATE_pos)

      #
      # Find Boost Test Suite start and push it on the stack.
      #
      set(BOOST_TEST_SUITE_START_REGEX "^BOOST_(AUTO|FIXTURE)_TEST_SUITE\\(([^),]+).*\\)")

      if(${BOOST_TEST_MACRO} MATCHES ${BOOST_TEST_SUITE_START_REGEX})
        string(REGEX REPLACE
          ${BOOST_TEST_SUITE_START_REGEX} "\\2"
          BOOST_TEST_SUITE_NAME ${BOOST_TEST_MACRO})

        list(APPEND BOOST_TEST_SUITE_STACK ${BOOST_TEST_SUITE_NAME})
      endif(${BOOST_TEST_MACRO} MATCHES ${BOOST_TEST_SUITE_START_REGEX})

      #
      # Find Boost Test Suite end and pop it from the stack.
      #
      set(BOOST_TEST_SUITE_END_REGEX "^BOOST_AUTO_TEST_SUITE_END\\(\\)")

      if(${BOOST_TEST_MACRO} MATCHES ${BOOST_TEST_SUITE_END_REGEX})
        list(LENGTH BOOST_TEST_SUITE_STACK LENGTH)
        math(EXPR INDEX "${LENGTH} - 1")
        list(REMOVE_AT BOOST_TEST_SUITE_STACK ${INDEX})
      endif(${BOOST_TEST_MACRO} MATCHES ${BOOST_TEST_SUITE_END_REGEX})

      #
      # Find Boost Test Case and add it to the given target.
      #
      set(BOOST_TEST_CASE_REGEX "BOOST_(AUTO|FIXTURE)_TEST_CASE(_TEMPLATE)?\\(([^),]+).*\\).*")

      if(${BOOST_TEST_MACRO} MATCHES ${BOOST_TEST_CASE_REGEX})
        string(REGEX REPLACE ${BOOST_TEST_CASE_REGEX} "\\3" BOOST_TEST_CASE_NAME ${BOOST_TEST_MACRO})

        JOIN(BOOST_TEST_CASE_NAME "/" FALSE ${BOOST_TEST_SUITE_STACK} ${BOOST_TEST_CASE_NAME})
        add_test(${BOOST_TEST_CASE_NAME} ${TARGET} "--run_test=${BOOST_TEST_CASE_NAME}")

        math(EXPR BOOST_NUM_TEST_CASES "${BOOST_NUM_TEST_CASES} + 1")
      endif(${BOOST_TEST_MACRO} MATCHES ${BOOST_TEST_CASE_REGEX})
    endforeach(BOOST_TEST_MACRO)

    message(STATUS " * Found ${BOOST_NUM_TEST_CASES} test cases in \"${SOURCE}\"")
    math(EXPR BOOST_NUM_TOTAL_TEST_CASES "${BOOST_NUM_TOTAL_TEST_CASES} + ${BOOST_NUM_TEST_CASES}")
  endforeach(SOURCE)

  message(STATUS "Found ${BOOST_NUM_TOTAL_TEST_CASES} test cases in total.")
endfunction()
