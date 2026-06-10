# webview_windows

[![CI](https://github.com/omar-hanafy/flutter-webview-windows/actions/workflows/ci.yml/badge.svg)](https://github.com/omar-hanafy/flutter-webview-windows/actions/workflows/ci.yml)

A [Flutter](https://flutter.dev/) WebView plugin for Windows built on
[Microsoft Edge WebView2](https://learn.microsoft.com/en-us/microsoft-edge/webview2/),
rendered off-screen and composited seamlessly into the Flutter widget tree.

> **Fork notice:** This is a maintained fork of
> [jnschulze/flutter-webview-windows](https://github.com/jnschulze/flutter-webview-windows)
> (forked at `ed81bbe`). On top of upstream it fixes
> [the window focus loss issue (#230)](https://github.com/jnschulze/flutter-webview-windows/issues/230),
> modernizes the native toolchain (WebView2 SDK 1.0.3967.48, WIL
> 1.0.260126.7, C++23), hardens the COM/channel layers, and ships a real test
> suite. See [CHANGELOG.md](CHANGELOG.md) for the full list, including
> migration notes for breaking changes.

## Features

- Off-screen rendering composited as a Flutter `Texture` - no airspace
  issues, works with transforms, opacity, and widgets painted on top.
- Full keyboard focus integration: clicking the webview does **not**
  deactivate the host window, clicking back on Flutter UI restores Flutter's
  keyboard handling instantly, and `Tab` traversal leaves the page cleanly.
- Typed, broadcast event streams: URL, loading state, document title, history,
  security state, full-screen elements, downloads, load errors, web messages,
  and native focus.
- JavaScript execution, init scripts, JSON message passing, cookies/cache
  management, user agent, zoom, background color, popup policy, virtual host
  mapping, suspend/resume, and FPS limiting.
- Mouse, high-precision trackpad scrolling, and multi-touch input forwarding.
- High-DPI aware.

![image](https://user-images.githubusercontent.com/720469/116823636-d8b9fe00-ab85-11eb-9f91-b7bc819615ed.png)

## Requirements

**Target platform**

- Windows 10 1809 or newer (the off-screen compositor relies on
  `Windows.Graphics.Capture`).
- The [WebView2 Runtime](https://developer.microsoft.com/en-us/microsoft-edge/webview2/)
  must be installed. It ships with Windows 11 and current Windows 10; call
  `WebviewController.getWebViewVersion()` at startup - if it returns `null`,
  guide the user to install the runtime.

**Development**

- Flutter 3.44+ / Dart 3.12+
- Visual Studio 2022 with the *Desktop development with C++* workload
- A Windows 11 SDK (10.0.22000 or newer; the build targets the latest SDK
  installed on your machine)
- Optional: `nuget.exe` in your `PATH` (the build downloads it automatically
  if missing)

## Getting started

The `webview_windows` name on pub.dev belongs to the upstream package, so
consume this fork as a git dependency:

```yaml
dependencies:
  webview_windows:
    git:
      url: https://github.com/omar-hanafy/flutter-webview-windows.git
      ref: main
```

### Quick start

```dart
import 'package:webview_windows/webview_windows.dart';

final controller = WebviewController();

Future<void> start() async {
  await controller.initialize();
  await controller.setBackgroundColor(Colors.transparent);
  await controller.setPopupWindowPolicy(WebviewPopupWindowPolicy.deny);
  await controller.loadUrl('https://flutter.dev');
}

// In your widget tree:
Webview(controller)

// When you are done:
await controller.dispose();
```

The optional shared browser environment (custom user-data directory, browser
executable, or Chromium command line flags) can be configured once, before
any controller is initialized:

```dart
await WebviewController.initializeEnvironment(
  additionalArguments: '--show-fps-counter',
);
```

### Listening to events

All controller streams are broadcast streams:

```dart
controller.url.listen((url) => print('URL: $url'));
controller.loadingState.listen((state) => print('Loading: $state'));
controller.title.listen((title) => print('Title: $title'));
controller.onLoadError.listen((status) => print('Load error: $status'));
controller.webMessage.listen((message) => print('Message: $message'));
```

### Handling permission requests

```dart
Webview(
  controller,
  permissionRequested: (url, kind, isUserInitiated) async {
    final allowed = await askTheUserSomehow(url, kind);
    return allowed
        ? WebviewPermissionDecision.allow
        : WebviewPermissionDecision.deny;
  },
)
```

### Keyboard focus

Clicking into the webview gives it real Win32 keyboard focus; clicking any
Flutter UI hands focus back automatically. For programmatic control:

```dart
await controller.focus();                 // move keyboard focus into the page
await WebviewController.releaseFocus();   // hand it back to Flutter
controller.onFocusChanged.listen((focused) => ...);
```

## Example & tests

A complete example app lives in [`example/`](example/). The repository also
contains:

- a channel-level Dart unit test suite (`flutter test`) covering every
  controller method, the controller lifecycle, all event types, widget input
  forwarding and focus coordination,
- native C++ unit tests (GoogleTest) for the platform implementation:

  ```sh
  cmake -S windows/test -B build/native_tests
  cmake --build build/native_tests --config Release
  ctest --test-dir build/native_tests -C Release --output-on-failure
  ```

- a real-input integration test for the focus behavior
  (`cd example && flutter test integration_test/focus_test.dart -d windows`),
  which injects actual Win32 input and asserts on real window-manager state,
  so it needs an interactive Windows session.

When writing your own widget tests against a mocked `WebviewController`,
wrap `initialize()` and `dispose()` in `tester.runAsync(...)`: `testWidgets`
bodies run in a FakeAsync zone in which the event channel teardown can leave
platform-channel futures undriven, so awaiting them directly hangs the test.

## Limitations

WebView2 has no official off-screen rendering API yet. This plugin uses
`Windows.Graphics.Capture` to obtain frames, which is why Windows versions
older than Windows 10 1809 are not supported.

See:
- https://github.com/MicrosoftEdge/WebView2Feedback/issues/20
- https://github.com/MicrosoftEdge/WebView2Feedback/issues/526
- https://github.com/MicrosoftEdge/WebView2Feedback/issues/547

## Troubleshooting

- **`initialize()` fails with `environment_creation_failed`** - the WebView2
  Runtime is missing. Check `WebviewController.getWebViewVersion()`.
- **The build cannot download NuGet** - install
  [nuget.exe](https://www.nuget.org/downloads) manually and add it to `PATH`.
- **Keyboard input goes to the page after clicking it** - that is by design;
  click any Flutter widget (or call `WebviewController.releaseFocus()`) to
  return focus to Flutter.

## Credits

Created by [Niklas Schulze (jnschulze)](https://github.com/jnschulze). This
fork is maintained by [Omar Hanafy](https://github.com/omar-hanafy).
Licensed under the [BSD 3-Clause License](LICENSE).
