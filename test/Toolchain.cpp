/*
 *  test/Toolchain.cpp
 *  Copyright 2020-2021 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#include <filesystem>
#include <fstream>
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "SMCE/Toolchain.hpp"
#include "defs.hpp"

TEST_CASE("Toolchain valid resource", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    REQUIRE(tc.resource_dir() == SMCE_PATH);
    REQUIRE_FALSE(tc.cmake_path().empty());
}

TEST_CASE("Toolchain empty resource", "[Toolchain]") {
    const auto path = SMCE_TEST_DIR "/empty_dir";
    std::filesystem::create_directory(path);
    smce::Toolchain tc{path};
    std::cout << tc.check_suitable_environment();
    REQUIRE(tc.check_suitable_environment() == smce::toolchain_error::resdir_empty);
    REQUIRE(tc.resource_dir() == path);
}

TEST_CASE("Toolchain resource does not exist", "[Toolchain]") {
    const auto path = SMCE_TEST_DIR "/no_dir";
    smce::Toolchain tc{path};
    REQUIRE(tc.check_suitable_environment() == smce::toolchain_error::resdir_absent);
    REQUIRE(tc.resource_dir() == path);
}

TEST_CASE("Toolchain resource is a file", "[Toolchain]") {
    std::ofstream outfile(SMCE_TEST_DIR "/file.txt");
    outfile.close();
    const auto path = SMCE_TEST_DIR "/file.txt";
    smce::Toolchain tc{path};
    REQUIRE(tc.check_suitable_environment() == smce::toolchain_error::resdir_file);
    REQUIRE(tc.resource_dir() == path);
}

TEST_CASE("Toolchain valid sketch", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    // clang-format off
    smce::Sketch sk{SKETCHES_PATH "sd_fs", {
        .fqbn = "arduino:avr:nano",
        .legacy_preproc_libs = { {"SD"} }
    }};
    // clang-format on
    REQUIRE(!tc.compile(sk));
}

TEST_CASE("Toolchain invalid sketch", "[Toolchain]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    smce::Sketch sk{SKETCHES_PATH "sd_fs", {}};
    REQUIRE(tc.compile(sk) == smce::toolchain_error::sketch_invalid);
    // clang-format off
    smce::Sketch sk2{"", {
       .fqbn = "arduino:avr:nano"
   }};
    // clang-format on
    REQUIRE(tc.compile(sk) == smce::toolchain_error::sketch_invalid);
}
