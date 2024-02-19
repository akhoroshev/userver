#pragma once

/// @file userver/logging/json_string.hpp
/// @brief @copybrief logging::JsonString

#include <string>
#include <string_view>

#include <userver/formats/json/value.hpp>

USERVER_NAMESPACE_BEGIN

namespace logging {

/// String in json format
class JsonString {
 public:
  /// @brief Constructs from provided json object.
  JsonString(const formats::json::Value& value);

  /// @brief Constructs from provided json string. It is the user's
  /// responsibility to ensure that the input json string is valid.
  explicit JsonString(std::string json) noexcept;

  /// @brief Constructs "null"
  JsonString() noexcept = default;

  JsonString(JsonString&&) noexcept = default;
  JsonString(const JsonString&) = default;

  JsonString& operator=(JsonString&&) noexcept = default;
  JsonString& operator=(const JsonString&) = default;

  /// @brief Returns view to json
  std::string_view Value() const;

 private:
  std::string json_;
};

}  // namespace logging

USERVER_NAMESPACE_END
