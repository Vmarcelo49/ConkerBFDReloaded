# Host regression tests

These Linux host-side tests use synthetic RDRAM, not ROM assets, saves, or generated game code. Run `tools/setup_deps.sh` first, then run either script from any working directory. They require a C++20 compiler and GNU `timeout`; `CXX` selects the compiler. Assertions remain enabled.

## Ledge release

```sh
bash tools/testing/ledge_release.sh
```

Covers a fresh B press while hanging, B held before catching, releasing and pressing again, a new hold resetting the release latch, fall state/gravity, no enhanced re-grab before landing, normal grabs after landing, non-hanging animations, the feature being disabled, and the existing stuck-hold recovery.

Manual check with Ledge Grab enabled: catch a ledge, press the mapped in-game B button, and confirm Conker drops. Hold B before catching another ledge and confirm he stays hanging until B is released and pressed again. Check that climbing and hopping still work and that disabling Ledge Grab preserves the original controls.

## Timer shutdown

```sh
bash tools/testing/timer_shutdown.sh
```

Compiles the actual patched runtime timer worker with no-op host callbacks. Separate processes exercise an idle worker and a timer with a deadline 24 hours away. Both must shut down promptly, leave no joinable timer worker, and tolerate another sequential shutdown call. Each process has an external five-second timeout to detect hangs.

Manual shutdown matrix for the built application:

- Launcher Exit button.
- Window-manager close from the launcher (titlebar or window menu).
- Exit confirmation during an active game.
- Window-manager close during an active game, then confirm the quit prompt.
- Timed run with `--seconds 12`.

Confirm the process exits normally, not just that its window disappears. Automated harnesses do not replace the active-game and platform-specific UI checks.
