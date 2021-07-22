cmake_minimum_required(VERSION 3.18)
project(timecobot)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
add_definitions("-std=c++20")
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
    TLS_VERIFY ON)
endif ()
include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(REQUIRES
  cpr/1.6.2
  threadpool/20140926
  simdjson/0.9.6
  catch2/2.13.6
  cereal/1.3.0
  libnop/cci.20200728
  spdlog/1.8.5
  icu/69.1 GENERATORS cmake_find_package)
conan_cmake_autodetect(settings)
conan_cmake_install(PATH_OR_REFERENCE . BUILD missing REMOTE conan-center SETTINGS ${settings})

find_package(Catch2)
find_package(ICU)
find_package(cereal)
find_package(cpr)
find_package(spdlog)
find_package(simdjson)
find_package(ThreadPool)

file(GLOB proj_sources src/*.cpp)
set(MAKE_FLAG)
add_executable(app ${proj_sources})

target_link_libraries(app PRIVATE
  ThreadPool::ThreadPool
  cpr::cpr
  simdjson::simdjson
  cereal::cereal
  spdlog::spdlog
  ICU::io
  ICU::i18n
  ICU::uc
  ICU::data)

#[[Changing the entry point for tests]]
list(FILTER proj_sources EXCLUDE REGEX ".*Main.cpp$")
file(GLOB test_sources test/*.cpp)
add_executable(test ${proj_sources} ${test_sources})

target_link_libraries(test PRIVATE
  ThreadPool::ThreadPool
  Catch2::Catch2
  cereal::cereal
  spdlog::spdlog
  cpr::cpr
  simdjson::simdjson
  cereal::cereal
  ICU::test
  ICU::io
  ICU::i18n
  ICU::uc
  ICU::data)



