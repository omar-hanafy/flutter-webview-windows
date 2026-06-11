#pragma once

#include <windows.h>

#include <string>

namespace util {

// Returns the Flutter engine cursor name (see
// shell/platform/windows/flutter_window_win32.cc) for the given Win32 cursor
// handle, or "basic" for unknown handles.
const std::string& GetCursorName(HCURSOR cursor);

}  // namespace util
