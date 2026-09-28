# Install the custom macOS 27 build

Use `ishankunam/InstantSpaceSwitcher`, branch `fix/macos27-local-integration`.
The separate `fix/macos27-event-hardening` branch is the focused contribution
in [upstream PR #104](https://github.com/jurplel/InstantSpaceSwitcher/pull/104).
The fork's `main` stays aligned with upstream.

## Build and check

```sh
git switch fix/macos27-local-integration
bash dist/test-events.sh
bash dist/build.sh
codesign --verify --deep --strict --verbose=2 build/InstantSpaceSwitcher.app
lipo -archs build/InstantSpaceSwitcher.app/Contents/MacOS/InstantSpaceSwitcher
lipo -archs build/InstantSpaceSwitcher.app/Contents/MacOS/ISSCli
/usr/libexec/PlistBuddy -c 'Print :GitCommitHash' build/InstantSpaceSwitcher.app/Contents/Info.plist
```

The bundle contains both `arm64` and `x86_64` binaries. It is signed ad hoc.
Full Xcode is required for `swift test`. Command Line Tools can build the app
and run `bash dist/test-events.sh`, which exercises the same C assertions with
undefined-behavior and float-cast-overflow sanitizers.

The GitHub Actions workflow runs XCTest, C assertions, the universal build,
and signature verification on `macos-26` and `xcode-27`.

## Back up and install

Create a fresh directory under the ignored `local-backups/` directory before
each installation. Keep it outside `build/`, which `dist/build.sh --clean`
can delete.

```sh
iss_backup_dir="$PWD/local-backups/$(date +%Y%m%d-%H%M%S)"
mkdir -p "$iss_backup_dir"
ditto /Applications/InstantSpaceSwitcher.app "$iss_backup_dir/InstantSpaceSwitcher.app"
defaults export com.interversehq.InstantSpaceSwitcher "$iss_backup_dir/preferences.plist"
```

Quit InstantSpaceSwitcher through its menu. Replace the application in
`/Applications` with the verified bundle, then launch that installed copy.
Avoid launching the build or backup copies at the same time.

The bundle identifier and preference keys are unchanged. If macOS requests
Accessibility authorization, re-enable the installed app in System Settings.
On macOS 27 this may appear under Privacy & Security → Device Control and
Data Access. Authorization was required for the first replacement on this Mac.

Control-H/Control-L, trackpad override, and the saved settings are preserved.
On macOS 27 the speed control displays Instant and is disabled because this
event path uses a fixed final fling. The stored speed is preserved for the
older macOS event path.

The Homebrew cask receipt remains at version 2.0. A future Homebrew upgrade or
reinstall can replace the custom app. Check upstream compatibility before
upgrading, or rebuild this branch afterward.

## Roll back

1. Quit the custom app.
2. Move it into a separate recovery directory.
3. Copy the backed-up `InstantSpaceSwitcher.app` to `/Applications`.
4. Restore preferences while the app is stopped:

   ```sh
   defaults import com.interversehq.InstantSpaceSwitcher /absolute/path/to/backup/preferences.plist
   ```

5. Launch the restored app and reauthorize Accessibility if requested.

The original v2.0 app has the original macOS 27 switching limitation.

## Integration provenance

- Upstream `main`: `fd37e7fed62ad862ec6326aa7dac9b7bc6b413e5`.
- Upstream `macos-27`: `bf32cf9732c706119e8307d2d70ae795b2b11c1c`.
- Serializer layout: the upstream compatibility implementation and
  [mgbowen's research](https://gist.github.com/mgbowen/5548f18ada2e37b23c9e86a8d80b71dc).
- Paired gestures and release-build fling: [#102](https://github.com/jurplel/InstantSpaceSwitcher/pull/102)
  by markokovac16 and [#103](https://github.com/jurplel/InstantSpaceSwitcher/pull/103)
  by MathieuSlt, building on the work discussed in #72 and #97.
- Prediction window: [#101](https://github.com/jurplel/InstantSpaceSwitcher/pull/101)
  by VictorHaine and #103. This integration expires predictions per display
  and additionally invalidates changed Space order and display topology.
- CLI run-loop drain: [#88](https://github.com/jurplel/InstantSpaceSwitcher/pull/88)
  by EmblemWu. Natural-scrolling refresh also follows the approach discussed
  in [#100](https://github.com/jurplel/InstantSpaceSwitcher/pull/100).

The integration uses explicit markers instead of an event-count exemption.
It prepares every phase before posting and rebuilds the HID payload of
consumed hardware terminal events. These integration changes are separate
from the focused upstream PR. Existing license notices remain intact.
