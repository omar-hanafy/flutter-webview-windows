## 0.5.0

First release of this fork. Everything below ships as one version on top of
upstream `0.4.0` (including all upstream changes up to `ed81bbe`: scroll
fixes, HTML `select` fix, CMake modernization). The two headline areas are
the window focus fix tracked in
[jnschulze/flutter-webview-windows#230](https://github.com/jnschulze/flutter-webview-windows/issues/230)
and a full 2026 modernization.

**Focus handling (upstream #230)**

* **Fix host window deactivation on click.** The WebView2 browser windows are
  now reparented into the Flutter view's window tree (`put_ParentWindow`)
  instead of living under a message-only window. Clicking the webview no
  longer deactivates the Flutter window (gray title bar, dead shortcuts).
* **Automatic focus return.** Clicking anywhere in Flutter that is not a
  webview now hands Win32 keyboard focus back to the Flutter view instantly
  (new `reclaimFocus` plugin method driven by a global pointer listener). The
  handover is driven by which widget actually received the press, so it stays
  correct when Flutter UI (dialogs, menus) is painted over a webview or the
  webview is transformed.
* **Tab out of the webview.** Tabbing past the page's last focusable element
  now returns keyboard focus to Flutter instead of cycling inside the page
  (WebView2 `MoveFocusRequested`).
* Add `WebviewController.focus()` to give the webview keyboard focus
  programmatically (WebView2 `MoveFocus`).
* Add `WebviewController.releaseFocus()` to return keyboard focus to Flutter
  programmatically.
* Add `WebviewController.onFocusChanged` stream and
  `WebviewController.hasNativeFocus` reflecting the WebView2 native focus
  state.

**Breaking changes**

* The minimum SDK is now Dart 3.12 / Flutter 3.44.
* All `WebviewController` event streams are now **broadcast** streams
  (previously `url`, `title`, `historyChanged`, `securityStateChanged`,
  `webMessage` and `onLoadError` were single-subscription). Multiple
  listeners are now allowed; events emitted while nobody listens are dropped.
* `WebErrorStatus` constants are renamed to standard Dart `lowerCamelCase`
  and the redundant prefix is gone: `WebErrorStatusTimeout` ->
  `WebErrorStatus.timeout`, `WebErrorStatusUnknown` ->
  `WebErrorStatus.unknown`, and so on. Indices (the native wire format) are
  unchanged.
* Calling controller methods before `initialize()` completes now throws a
  `StateError` with an actionable message instead of crashing with a
  `LateInitializationError` in release builds (debug builds used to assert).
* `initialize()` now throws a `StateError` when called on a disposed
  controller, and completes with the underlying error for *any* failure
  (previously a `MissingPluginException` could leave the controller - and a
  later `dispose()` - hanging forever).
* The top-level `getButton` helper is no longer exported; it was an internal
  input-mapping detail.

**Improvements**

* `initialize()` is now re-entrant: concurrent calls join the in-flight
  initialization (previously they raced a second native instance into
  existence), and a failed attempt can be retried.
* `WebviewController.ready` can no longer hang forever: it completes on
  successful initialization and when the controller is disposed early.
* `Webview` now accepts a widget `key`.
* `dispose()` awaits an in-flight initialization, so a concurrently created
  native instance can no longer be orphaned.
* Full dartdoc coverage of the public API.

**Native fixes**

* Fix a COM reference leak of `ICoreWebView2PointerInfo` on every touch
  pointer event.
* Fix the environment-creation completion handler stealing a borrowed COM
  reference (latent use-after-free) and publishing it after unblocking the
  waiting thread (data race).
* Keep a real COM reference on permission-request event args across the
  async round trip to Dart (latent use-after-free).
* Make all UTF-16 -> UTF-8 conversions of nullable COM strings null-safe
  (constructing a `std::wstring_view` from null is undefined behavior).
* Fix `dispose` silently failing for texture ids that fit in 32 bits (the
  standard codec encodes small Dart ints as `int32`; the native side only
  matched `int64`), which leaked the entire native webview instance.
* Fix a `CoTaskMem` string leak in `getWebViewVersion`.
* Fix non-ASCII `additionalArguments` being mangled by char-by-char
  widening; they are now converted as UTF-8.
* Channel handlers no longer use throwing variant access; malformed channel
  arguments now produce an error result instead of terminating the process.
* Guard the download-started handler against a failed
  `get_DownloadOperation`.

**Toolchain**

* WebView2 SDK 1.0.1210.39 -> 1.0.3967.48, WIL 1.0.220914.1 -> 1.0.260126.7,
  NuGet fallback 5.10.0 -> 7.6.0 (SHA-256 verified).
* The plugin builds as C++23 (now `PRIVATE`, so the consuming app's language
  level is no longer forced up by the plugin); CMake minimum is 3.20.
* The example targets the latest installed Windows SDK instead of
  force-pinning 10.0.22000.
* `flutter_lints` 6.0 with a zero-warning codebase; CI rewritten (analyze,
  format, Dart unit tests, native C++ unit tests, Windows release build with
  bundle verification).

**Correctness & hardening (carried over from the focus work)**

* Fix inverted success reporting for `setVirtualHostNameMapping` /
  `clearVirtualHostNameMapping` (an `HRESULT` was returned as a `bool`, so
  success read as failure).
* Fix native string leaks on URL and document-title changes
  (`wil::unique_cotaskmem_string`).
* Use `try_query` instead of the throwing `query` for optional WebView2
  interfaces in suspend/resume and virtual-host mapping.
* Drop an unused download-event deferral that was never completed.
* Release WinRT activation factories instead of leaking them, and only call
  `RoUninitialize` when this instance actually initialized the apartment.
* Close every `WebviewController` stream controller on dispose and tear the
  native bridge down in an order that can't emit through a freed event sink.
* Surface webview creation failures instead of returning a silent, dead
  instance.
* Dispose is now safe before/after a failed `initialize()`, and `dispose()` is
  idempotent.
* Restore native (Win32) keyboard focus updates via `View.of(context)` rather
  than the deprecated global `window`.
* Install NuGet dependencies at configure time so the imported `.targets`
  exist before the build system is generated.
* `onLoadError` statuses reported by a newer WebView2 runtime than this
  package knows degrade to `WebErrorStatus.unknown` instead of crashing the
  event handler with a `RangeError`.

**Tests**

* Dart unit test suite covering the channel contract of every controller
  method, the controller lifecycle (concurrent/failed/retried initialization,
  disposal in every state), all event types, widget input forwarding (mouse,
  touch, scroll wheel, trackpad panning), permission round trips, cursor
  updates, and focus coordination.
* Enum wire-format contract is enforced twice: `static_assert`s pin the
  native enum values at compile time and Dart tests pin the enum orders, so
  the two sides cannot drift apart silently.
* Native C++ unit tests (GoogleTest, run in CI on `windows-latest`) covering
  UTF-8/UTF-16 conversion (including invalid input and null tolerance),
  cursor-name mapping, and mouse virtual-key state tracking.

## 0.4.0

* Enable MSVC coroutine support ([#278](https://github.com/jnschulze/flutter-webview-windows/pull/278))
* Enable scrolling with trackpad ([#274](https://github.com/jnschulze/flutter-webview-windows/pull/274))

## 0.3.0

* Add full-screen support ([#189](https://github.com/jnschulze/flutter-webview-windows/pull/189))
* Make `loadingState` a broadcast stream ([#193](https://github.com/jnschulze/flutter-webview-windows/pull/193))
* Add `getWebViewVersion()` method ([#197](https://github.com/jnschulze/flutter-webview-windows/pull/197))
* Fix string casting of paths that may contain Unicode characters ([#199](https://github.com/jnschulze/flutter-webview-windows/pull/199))
* Add high-DPI screen support ([#203](https://github.com/jnschulze/flutter-webview-windows/pull/203))
* Add `setZoomFactor` ([#214](https://github.com/jnschulze/flutter-webview-windows/pull/214))
* Fix example ([#215](https://github.com/jnschulze/flutter-webview-windows/pull/215))
* Fix Visual Studio 17.6 builds ([#252](https://github.com/jnschulze/flutter-webview-windows/pull/252))

## 0.2.2

* Remove `libfmt` dependency in favor of C++20 `std::format`
* Enable D3D texture bridge by default
* Make `executeScript` return the script's result

## 0.2.1

* Add `WebviewController.addScriptToExecuteOnDocumentCreated` and `WebviewController.removeScriptToExecuteOnDocumentCreated`
* Add `WebviewController.onLoadError` stream
* Change `WebviewController.webMessage` stream type from `Map<dynamic, dynamic>` to `dynamic`
* Add virtual hostname mapping support 
* Add multi-touch support

## 0.2.0

* Fix Flutter 3.0 null safety warning in example
* Bump WebView2 SDK version to `1.0.1210.3`
* Add an option for limiting the FPS
* Change data directory base path from `RoamingAppData` to `LocalAppData`

## 0.1.9

* Fix Flutter 3.0 compatibility

## 0.1.8

* Prefix CMake build target names to prevent collisions with other plugins

## 0.1.7

* Add method for opening DevTools
* Update `TextureBridgeGpu`
* Update `libfmt` dependency

## 0.1.7-dev.2

* Ensure Flutter apps referencing `webview_windows` still work on Windows 8.

## 0.1.7-dev.1

* Remove windowsapp.lib dependency

## 0.1.6

* Improve WebView creation error handling

## 0.1.5

* Fix a potential crash during WebView creation

## 0.1.4

* Improve error handling for Webview environment creation

## 0.1.3

* Stability fixes

## 0.1.2

* Unregister method channel handlers upon WebView destruction

## 0.1.1

* Fix unicode string conversion in ExecuteScript and LoadStringContent
* Load CoreMessaging.dll on demand

## 0.1.0

* Fix a string conversion issue
* Add an option for controlling popup window behavior
* Update Microsoft.Web.WebView2 and Microsoft.Windows.ImplementationLibrary

## 0.0.9

* Fix resizing issues
* Add preliminary GpuSurfaceTexture support

## 0.0.8

* Don't rely on AVX2 support
* Add history controls
* Add suspend/resume support
* Add support for disabling cache, clearing cookies etc.

## 0.0.7

* Add support for handling permission requests
* Allow setting the background color
* Automatically download nuget

## 0.0.6

* Fix mousewheel event handling
* Make text selection work
* Add method for setting the user agent
* Add support for JavaScript injection
* Add support for JSON message passing between Dart and JS
* Fix WebView disposal

## 0.0.5

* Fix input field focus issue

## 0.0.4

* Minor cleanup

## 0.0.3

* Add support for additional cursor types

## 0.0.2

* Add support for loading string content

## 0.0.1

* Initial release
