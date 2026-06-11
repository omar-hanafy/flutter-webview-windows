#include "util/string_converter.h"

#include <gtest/gtest.h>

#include <string>
#include <string_view>

namespace util {
namespace {

TEST(Utf8FromUtf16, ConvertsAscii) {
  EXPECT_EQ(Utf8FromUtf16(std::wstring_view(L"hello, webview!")),
            "hello, webview!");
}

TEST(Utf8FromUtf16, ConvertsBmpCharacters) {
  // "Gruesse" spelled with u-umlaut (U+00FC) and sharp s (U+00DF).
  EXPECT_EQ(Utf8FromUtf16(std::wstring_view(L"Gr\u00FC\u00DF"
                                            L"e")),
            "Gr\xC3\xBC\xC3\x9F"
            "e");
}

TEST(Utf8FromUtf16, ConvertsSupplementaryPlaneCharacters) {
  // U+1F600 (grinning face), a surrogate pair in UTF-16.
  EXPECT_EQ(Utf8FromUtf16(std::wstring_view(L"\xD83D\xDE00")),
            "\xF0\x9F\x98\x80");
}

TEST(Utf8FromUtf16, PreservesEmbeddedNulCharacters) {
  constexpr wchar_t source[] = {L'a', L'\0', L'b'};
  const auto result = Utf8FromUtf16(std::wstring_view(source, 3));
  EXPECT_EQ(result, std::string("a\0b", 3));
}

TEST(Utf8FromUtf16, ReturnsEmptyForEmptyInput) {
  EXPECT_EQ(Utf8FromUtf16(std::wstring_view(L"")), "");
}

TEST(Utf8FromUtf16, ReturnsEmptyForNullPointer) {
  EXPECT_EQ(Utf8FromUtf16(static_cast<const wchar_t*>(nullptr)), "");
}

TEST(Utf8FromUtf16, ReturnsEmptyForLoneSurrogate) {
  // An unpaired high surrogate is invalid UTF-16; the converter must fail
  // closed instead of emitting replacement garbage.
  constexpr wchar_t source[] = {0xD800};
  EXPECT_EQ(Utf8FromUtf16(std::wstring_view(source, 1)), "");
}

TEST(Utf16FromUtf8, ConvertsAscii) {
  EXPECT_EQ(Utf16FromUtf8(std::string_view("hello, webview!")),
            L"hello, webview!");
}

TEST(Utf16FromUtf8, ConvertsBmpCharacters) {
  EXPECT_EQ(Utf16FromUtf8(std::string_view("Gr\xC3\xBC\xC3\x9F"
                                           "e")),
            L"Gr\u00FC\u00DF"
            L"e");
}

TEST(Utf16FromUtf8, ConvertsSupplementaryPlaneCharacters) {
  EXPECT_EQ(Utf16FromUtf8(std::string_view("\xF0\x9F\x98\x80")),
            L"\xD83D\xDE00");
}

TEST(Utf16FromUtf8, PreservesEmbeddedNulCharacters) {
  constexpr char source[] = {'a', '\0', 'b'};
  const auto result = Utf16FromUtf8(std::string_view(source, 3));
  EXPECT_EQ(result, std::wstring(L"a\0b", 3));
}

TEST(Utf16FromUtf8, ReturnsEmptyForEmptyInput) {
  EXPECT_EQ(Utf16FromUtf8(std::string_view("")), L"");
}

TEST(Utf16FromUtf8, ReturnsEmptyForNullPointer) {
  EXPECT_EQ(Utf16FromUtf8(static_cast<const char*>(nullptr)), L"");
}

TEST(Utf16FromUtf8, ReturnsEmptyForInvalidUtf8) {
  // 0xFF can never appear in well-formed UTF-8.
  EXPECT_EQ(Utf16FromUtf8(std::string_view("\xFF\xFE")), L"");
}

TEST(Utf16FromUtf8, ReturnsEmptyForTruncatedSequence) {
  // First byte of a 4-byte sequence with no continuation bytes.
  EXPECT_EQ(Utf16FromUtf8(std::string_view("\xF0")), L"");
}

TEST(StringConverter, RoundTripsMixedContent) {
  const std::wstring original =
      L"path C:\\Users\\\u00E9\u00DF \xD83D\xDE80 end";
  EXPECT_EQ(Utf16FromUtf8(std::string_view(
                Utf8FromUtf16(std::wstring_view(original)))),
            original);
}

}  // namespace
}  // namespace util
