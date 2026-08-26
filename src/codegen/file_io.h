/**
 * @file        codegen/file_io.h
 * @brief       Content-addressed output writing shared by the codegen writers
 *
 * @copyright   Copyright (c) 2026 Tom Clay <tomc@tctechstuff.com>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#pragma once

#include <cstdio>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <rex/filesystem.h>

namespace rex::codegen {

inline void NormalizeGeneratedText(std::string& text) {
  std::string normalized;
  normalized.reserve(text.size());
  size_t read = 0;

  while (read < text.size()) {
    const size_t lineEnd = text.find('\n', read);
    const size_t contentEnd = lineEnd == std::string::npos ? text.size() : lineEnd;
    size_t trimmedEnd = contentEnd;
    while (trimmedEnd > read && (text[trimmedEnd - 1] == ' ' || text[trimmedEnd - 1] == '\t')) {
      --trimmedEnd;
    }

    normalized.append(text, read, trimmedEnd - read);

    if (lineEnd == std::string::npos)
      break;

    normalized.push_back('\n');
    read = lineEnd + 1;
  }

  while (!normalized.empty() && normalized.back() == '\n') {
    normalized.pop_back();
  }
  if (!normalized.empty()) {
    normalized.push_back('\n');
  }
  text = std::move(normalized);
}

inline std::optional<std::string> ReadFileBytes(const std::filesystem::path& path) {
  FILE* f = rex::filesystem::OpenFile(path, "rb");
  if (!f)
    return std::nullopt;
  std::string buf;
  char chunk[64 * 1024];
  size_t got = 0;
  while ((got = fread(chunk, 1, sizeof(chunk), f)) > 0) {
    buf.append(chunk, got);
  }
  fclose(f);
  return buf;
}

inline bool WriteFileBytes(const std::filesystem::path& path, std::string_view content) {
  FILE* f = rex::filesystem::OpenFile(path, "wb");
  if (!f)
    return false;
  size_t wrote = fwrite(content.data(), 1, content.size(), f);
  return fclose(f) == 0 && wrote == content.size();
}

enum class WriteOutcome { Failed, Unchanged, Written };

/// Leaves the file untouched when it already holds `content`, so an unchanged
/// output keeps its mtime and does not retrigger a build.
inline WriteOutcome WriteIfChanged(const std::filesystem::path& path, std::string_view content) {
  if (auto existing = ReadFileBytes(path); existing && *existing == content)
    return WriteOutcome::Unchanged;
  return WriteFileBytes(path, content) ? WriteOutcome::Written : WriteOutcome::Failed;
}

}  // namespace rex::codegen
