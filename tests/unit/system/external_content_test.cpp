#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include <rex/system/xam/content_manager.h>

using rex::X_RESULT;

namespace {

class ScratchDirectory {
 public:
  ScratchDirectory() {
    const auto token = std::chrono::steady_clock::now().time_since_epoch().count();
    root = std::filesystem::temp_directory_path() /
           ("rexglue_external_content_test_" + std::to_string(token));
    std::filesystem::create_directories(root);
  }

  ~ScratchDirectory() {
    std::error_code ec;
    std::filesystem::remove_all(root, ec);
  }

  std::filesystem::path root;
};

rex::system::xam::XCONTENT_AGGREGATE_DATA MakeContentData() {
  rex::system::xam::XCONTENT_AGGREGATE_DATA data{};
  data.device_id = 1;
  data.content_type = rex::system::XContentType::kPublisher;
  data.set_file_name("R000100");
  data.xuid = 0;
  data.title_id = 0x12345678;
  return data;
}

}  // namespace

TEST_CASE("External content registration validates directories", "[system][content]") {
  ScratchDirectory scratch;
  const auto external = scratch.root / "external";
  const auto regular_file = scratch.root / "regular-file";
  std::filesystem::create_directory(external);
  std::ofstream(regular_file) << "not a directory";

  rex::system::xam::ContentManager manager(nullptr, scratch.root / "managed");
  const auto data = MakeContentData();

  CHECK(manager.RegisterExternalContent(0, data, external) == X_ERROR_SUCCESS);
  CHECK(manager.ContentExists(0, data));
  CHECK(manager.RegisterExternalContent(0, data, regular_file) == X_ERROR_PATH_NOT_FOUND);
  CHECK(manager.RegisterExternalContent(0, data, scratch.root / "missing") ==
        X_ERROR_PATH_NOT_FOUND);
}

TEST_CASE("External content rejects destructive operations", "[system][content]") {
  ScratchDirectory scratch;
  const auto external = scratch.root / "external";
  std::filesystem::create_directory(external);

  rex::system::xam::ContentManager manager(nullptr, scratch.root / "managed");
  const auto data = MakeContentData();
  REQUIRE(manager.RegisterExternalContent(0, data, external) == X_ERROR_SUCCESS);

  CHECK(manager.SetContentThumbnail(0, data, {1, 2, 3}) == X_ERROR_ACCESS_DENIED);
  CHECK(manager.DeleteContent(0, data) == X_ERROR_ACCESS_DENIED);
  CHECK(manager.UnmountAndDeleteContent(0, data) == X_ERROR_ACCESS_DENIED);
  CHECK(std::filesystem::is_directory(external));
}
