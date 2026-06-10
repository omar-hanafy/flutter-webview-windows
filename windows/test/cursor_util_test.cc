#include "util/cursor_util.h"

#include <gtest/gtest.h>

#include <set>
#include <string>

namespace util {
namespace {

HCURSOR SystemCursor(const wchar_t* id) { return LoadCursor(nullptr, id); }

class CursorUtilTest : public ::testing::Test {
 protected:
  void SetUp() override {
    if (!SystemCursor(IDC_ARROW)) {
      GTEST_SKIP() << "System cursors are unavailable in this session.";
    }
  }
};

TEST_F(CursorUtilTest, MapsUnambiguousSystemCursors) {
  EXPECT_EQ(GetCursorName(SystemCursor(IDC_ARROW)), "basic");
  EXPECT_EQ(GetCursorName(SystemCursor(IDC_HAND)), "click");
  EXPECT_EQ(GetCursorName(SystemCursor(IDC_HELP)), "help");
  EXPECT_EQ(GetCursorName(SystemCursor(IDC_CROSS)), "precise");
  EXPECT_EQ(GetCursorName(SystemCursor(IDC_APPSTARTING)), "progress");
  EXPECT_EQ(GetCursorName(SystemCursor(IDC_IBEAM)), "text");
  EXPECT_EQ(GetCursorName(SystemCursor(IDC_WAIT)), "wait");
}

TEST_F(CursorUtilTest, MapsSharedHandlesToOneOfTheirSynonyms) {
  // Several Flutter cursor names intentionally share one Win32 cursor; the
  // map keeps a single name per handle, so any synonym is acceptable.
  const std::set<std::string> size_all = {"allScroll", "move"};
  EXPECT_TRUE(size_all.contains(GetCursorName(SystemCursor(IDC_SIZEALL))));

  const std::set<std::string> no = {"forbidden", "noDrop"};
  EXPECT_TRUE(no.contains(GetCursorName(SystemCursor(IDC_NO))));

  const std::set<std::string> size_we = {"resizeColumn", "resizeLeft",
                                         "resizeLeftRight", "resizeRight"};
  EXPECT_TRUE(size_we.contains(GetCursorName(SystemCursor(IDC_SIZEWE))));

  const std::set<std::string> size_ns = {"resizeDown", "resizeRow", "resizeUp",
                                         "resizeUpDown"};
  EXPECT_TRUE(size_ns.contains(GetCursorName(SystemCursor(IDC_SIZENS))));

  const std::set<std::string> size_nwse = {"resizeDownRight", "resizeUpLeft",
                                           "resizeUpLeftDownRight"};
  EXPECT_TRUE(size_nwse.contains(GetCursorName(SystemCursor(IDC_SIZENWSE))));

  const std::set<std::string> size_nesw = {"resizeDownLeft", "resizeUpRight",
                                           "resizeUpRightDownLeft"};
  EXPECT_TRUE(size_nesw.contains(GetCursorName(SystemCursor(IDC_SIZENESW))));
}

TEST_F(CursorUtilTest, FallsBackToBasicForUnknownHandle) {
  EXPECT_EQ(GetCursorName(reinterpret_cast<HCURSOR>(0xDEADBEEF)), "basic");
}

TEST_F(CursorUtilTest, FallsBackToBasicForNullHandle) {
  EXPECT_EQ(GetCursorName(nullptr), "basic");
}

TEST_F(CursorUtilTest, ReturnsStableReferences) {
  // The returned reference aliases storage with static lifetime; callers keep
  // const references to it (webview_bridge.cc), so it must stay valid and
  // identical across calls.
  const std::string& first = GetCursorName(SystemCursor(IDC_IBEAM));
  const std::string& second = GetCursorName(SystemCursor(IDC_IBEAM));
  EXPECT_EQ(&first, &second);
}

}  // namespace
}  // namespace util
