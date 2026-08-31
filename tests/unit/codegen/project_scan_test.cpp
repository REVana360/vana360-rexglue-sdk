/**
 * @file        codegen/project_scan_test.cpp
 * @brief       Tests for generated project CMake refreshes
 *
 * @copyright   Copyright (c) 2026 Tom Clay <tomc@tctechstuff.com>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

#include "rexglue/commands/project_scan.h"

using rexglue::cli::RenderRexglueCmake;

TEST_CASE("Generated CMake declares DLL sources as codegen outputs", "[project_scan]") {
  const std::string rendered =
      RenderRexglueCmake("sample", "0.10.0", "generated/entrypoint",
                         {"generated/first_module", "generated/second_module"});

  const auto first_list = rendered.find("generated/first_module/sources.cmake");
  const auto second_list = rendered.find("generated/second_module/sources.cmake");
  const auto command = rendered.find("add_custom_command(");
  const auto dll_outputs = rendered.find("${REXGLUE_DLL_GENERATED_SOURCES}", command);

  REQUIRE(first_list != std::string::npos);
  REQUIRE(second_list != std::string::npos);
  REQUIRE(command != std::string::npos);
  REQUIRE(dll_outputs != std::string::npos);
  CHECK(first_list < command);
  CHECK(second_list < command);
}
