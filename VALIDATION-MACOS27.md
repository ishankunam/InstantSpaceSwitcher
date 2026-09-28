# macOS 27 validation record

## Focused upstream contribution

- Branch: `fix/macos27-event-hardening`.
- Source: `4bb99aa005b09a01bd5e06d843f5d9e1c7ae3600`.
- [Draft upstream PR #104](https://github.com/jurplel/InstantSpaceSwitcher/pull/104)
  targets `macos-27`.
- [Passing fork CI](https://github.com/ishankunam/InstantSpaceSwitcher/actions/runs/36480705422).
- Both `macos-26` and `xcode-27` passed 22 XCTest tests, the sanitized C harness,
  universal release builds, and strict deep signature verification.
- The C harness also passed locally on macOS 27.0 (26A428).

These tests construct and inspect events without posting them. They cover
legacy and modern fields, packed payload bytes, signed 16.16 boundaries,
nonfinite inputs, malformed serialized data, payload replacement, and
synthetic-event identity. They do not establish that the focused branch by
itself restores switching on the final macOS 27 release.

## Installed integration

- Branch: `fix/macos27-local-integration`.
- Installed app source: `0c254bf`.
- [Passing integration CI](https://github.com/ishankunam/InstantSpaceSwitcher/actions/runs/36481919753).
- Both runners passed 26 XCTest tests, the sanitized C harness, universal
  release builds, and strict deep signature verification.
- Additional tests cover paired phase order, both natural-scrolling mappings,
  neutral terminal payloads, rapid-switch prediction retention, one-second
  expiry, independent displays, and topology changes.
- Local universal release build and signature verification passed.
- Local XCTest is unavailable because this Mac has Command Line Tools without
  the XCTest framework. XCTest results above come from the Xcode CI runners.

The installed path is `/Applications/InstantSpaceSwitcher.app`. Both the GUI
and CLI binaries contain `arm64` and `x86_64` slices. The bundle identifier is
`com.interversehq.InstantSpaceSwitcher` and the injected source hash is
`0c254bf`. The app launched successfully. Accessibility authorization is on.
The settings UI shows trackpad override enabled and Instant speed selected.
Every pre-existing preference key and value matched the rollback export
after installation.

## Hardware results, 2026-09-28

The host is an Apple M2 Pro running macOS 27.0 (26A428). Connected displays are
MSI G273 at 1920×1080 and 165 Hz, and a portrait Dell S2721QS at 1080×1920
logical points and 60 Hz.

| Scenario | Result |
| --- | --- |
| Install, signature, launch, settings retention | Verified locally. |
| Control-H/Control-L and physical trackpad swipes | User confirmed both switch instantly in the expected direction. |
| First/last Space boundaries | Automated prediction coverage. Broader live edge checks remain unverified. |
| Rapid repeats, reversals, direct Space selection | Automated coverage. Live stress testing remains unverified. |
| Natural scrolling on/off and live preference changes | Both mappings pass automated tests. Hardware toggle test remains unverified. |
| Fullscreen Spaces and Mission Control | Unverified. |
| Each connected display and external navigation | Topology/expiry tests pass. Per-display hardware matrix remains unverified. |
| Exact marker value after event posting | In-memory identity tests pass. Transport-level observation remains unverified. |
| Standalone CLI switching | Run-loop drain is included and builds pass. Live CLI invocation remains unverified. |

The user's confirmation is a hardware result for the integration build. It is
not an independently observed test of the focused upstream branch. Read-only
CGS snapshots during this session reported one native Space on each display,
so those snapshots cannot demonstrate a completed transition. The attempted
Mission Control inspection timed out. No temporary test Spaces were created
by the agent. The upstream PR remains draft pending the broader live matrix.

## Recovery

Original v2.0 app and preference backups are in the ignored directory
`local-backups/20260928-165044-v2.0/`. That directory also retains the original
installed bundle as `previous-installation.app`.

Original GUI executable SHA-256:
`b0b7ab097e4e1657f50a567713080220ba3a1c7f42a51241f2792e76f082de0a`.

Follow [the rollback procedure](INSTALL-MACOS27.md#roll-back) to restore it.
