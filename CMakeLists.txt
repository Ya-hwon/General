cmake_minimum_required(VERSION 3.14)
project(general_test)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  hprtree_test
  hprtree_test.cpp
)
target_link_libraries(
  hprtree_test
  GTest::gtest_main
)

add_executable(
  removelist_test
  removelist_test.cpp
)
target_link_libraries(
  removelist_test
  GTest::gtest_main
)



include(GoogleTest)
gtest_discover_tests(hprtree_test)
gtest_discover_tests(removelist_test)