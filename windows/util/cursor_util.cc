#include "cursor_util.h"

#include <map>
#include <utility>

namespace util {

const std::string& GetCursorName(const HCURSOR cursor) {
  // The cursor names correspond to the Flutter Engine names:
  // in shell/platform/windows/flutter_window_win32.cc
  static const std::string kDefaultCursorName = "basic";
  static const std::pair<std::string, const wchar_t*> mappings[] = {
      {"allScroll", IDC_SIZEALL},
      {kDefaultCursorName, IDC_ARROW},
      {"click", IDC_HAND},
      {"forbidden", IDC_NO},
      {"help", IDC_HELP},
      {"move", IDC_SIZEALL},
      {"none", nullptr},
      {"noDrop", IDC_NO},
      {"precise", IDC_CROSS},
      {"progress", IDC_APPSTARTING},
      {"text", IDC_IBEAM},
      {"resizeColumn", IDC_SIZEWE},
      {"resizeDown", IDC_SIZENS},
      {"resizeDownLeft", IDC_SIZENESW},
      {"resizeDownRight", IDC_SIZENWSE},
      {"resizeLeft", IDC_SIZEWE},
      {"resizeLeftRight", IDC_SIZEWE},
      {"resizeRight", IDC_SIZEWE},
      {"resizeRow", IDC_SIZENS},
      {"resizeUp", IDC_SIZENS},
      {"resizeUpDown", IDC_SIZENS},
      {"resizeUpLeft", IDC_SIZENWSE},
      {"resizeUpRight", IDC_SIZENESW},
      {"resizeUpLeftDownRight", IDC_SIZENWSE},
      {"resizeUpRightDownLeft", IDC_SIZENESW},
      {"wait", IDC_WAIT},
  };

  // Magic-static initialization: built exactly once, immutable afterwards,
  // and thread-safe by construction.
  static const std::map<HCURSOR, std::string> cursors = [] {
    std::map<HCURSOR, std::string> map;
    for (const auto& pair : mappings) {
      HCURSOR cursor_handle = LoadCursor(nullptr, pair.second);
      if (cursor_handle) {
        map[cursor_handle] = pair.first;
      }
    }
    return map;
  }();

  const auto it = cursors.find(cursor);
  if (it != cursors.end()) {
    return it->second;
  }
  return kDefaultCursorName;
}

}  // namespace util
