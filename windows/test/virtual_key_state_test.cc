#include <gtest/gtest.h>

#include "webview.h"

namespace {

constexpr auto kNone =
    COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS::COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_NONE;
constexpr auto kLeft = COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS::
    COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_LEFT_BUTTON;
constexpr auto kRight = COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS::
    COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_RIGHT_BUTTON;
constexpr auto kMiddle = COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS::
    COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_MIDDLE_BUTTON;

TEST(VirtualKeyState, StartsWithNoButtonsDown) {
  VirtualKeyState state;
  EXPECT_EQ(state.state(), kNone);
}

TEST(VirtualKeyState, TracksEachButtonIndependently) {
  VirtualKeyState state;

  state.set_isLeftButtonDown(true);
  EXPECT_EQ(state.state(), kLeft);

  state.set_isRightButtonDown(true);
  EXPECT_EQ(state.state(), kLeft | kRight);

  state.set_isMiddleButtonDown(true);
  EXPECT_EQ(state.state(), kLeft | kRight | kMiddle);
}

TEST(VirtualKeyState, ClearingOneButtonLeavesTheOthers) {
  VirtualKeyState state;
  state.set_isLeftButtonDown(true);
  state.set_isRightButtonDown(true);
  state.set_isMiddleButtonDown(true);

  state.set_isRightButtonDown(false);
  EXPECT_EQ(state.state(), kLeft | kMiddle);

  state.set_isLeftButtonDown(false);
  EXPECT_EQ(state.state(), kMiddle);

  state.set_isMiddleButtonDown(false);
  EXPECT_EQ(state.state(), kNone);
}

TEST(VirtualKeyState, SettingIsIdempotent) {
  VirtualKeyState state;
  state.set_isLeftButtonDown(true);
  state.set_isLeftButtonDown(true);
  EXPECT_EQ(state.state(), kLeft);

  state.set_isLeftButtonDown(false);
  state.set_isLeftButtonDown(false);
  EXPECT_EQ(state.state(), kNone);
}

}  // namespace
