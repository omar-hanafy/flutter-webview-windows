#pragma once

#include <string>

namespace util {
std::string Utf8FromUtf16(std::wstring_view utf16_string);
std::wstring Utf16FromUtf8(std::string_view utf8_string);

// Null-tolerant overloads for COM out-parameters: many WebView2 getters leave
// the string pointer null on failure, and constructing a std::basic_string_view
// from a null pointer is undefined behavior. Routing every nullable native
// string through these overloads makes that entire bug class unreachable.
inline std::string Utf8FromUtf16(const wchar_t* utf16_string) {
  return utf16_string ? Utf8FromUtf16(std::wstring_view(utf16_string))
                      : std::string();
}
inline std::wstring Utf16FromUtf8(const char* utf8_string) {
  return utf8_string ? Utf16FromUtf8(std::string_view(utf8_string))
                     : std::wstring();
}
}  // namespace util
