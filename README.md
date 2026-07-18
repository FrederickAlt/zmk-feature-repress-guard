# ZMK Repress Guard

A ZMK feature module that suppresses an implausibly fast re-press of the same
key position without delaying normal key events.

## Behavior

For each key position independently:

1. A normal press is forwarded immediately.
2. Its release is forwarded immediately and starts the guard window.
3. A new press of that same position less than 30 ms after the most recent
   paired physical release is dropped.
4. The release paired with that dropped press is also dropped, even when it
   arrives much later, and restarts that position's guard window.

A press exactly 30 ms after the most recent paired release is accepted. Other key
positions are unaffected and are never made to wait.

The state machine also prevents an unmatched release or duplicate press from
leaking downstream.

## Why this does not add global latency

The module subscribes to `zmk_position_state_changed` before ZMK's built-in
listeners. Accepted events return `ZMK_EV_EVENT_BUBBLE` immediately. Rejected
events return `ZMK_EV_EVENT_HANDLED`, which permanently stops only that event;
there is no timer, queue, replay, or deferred decision.

## Split keyboards

The device that owns keymap processing applies the guard. Therefore:

- a split central filters both its local events and events received from its
  peripherals before the keymap handles them;
- split peripherals forward their events without applying the guard, avoiding
  duplicate filtering and listener-order dependencies;
- a non-split keyboard filters its local events before keymap processing.

The module must be present in the central build. Keeping it in the shared
`west.yml` is sufficient for a normal ZMK split build.

## Install from a separate GitHub repository

Upload this directory as a repository named `zmk-feature-repress-guard`, then
add its remote and project to `config/west.yml` in your `zmk-config`.

```yaml
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/zmkfirmware
    - name: your-github-user
      url-base: https://github.com/YOUR_GITHUB_USERNAME

  projects:
    - name: zmk
      remote: zmkfirmware
      revision: v0.3 # Keep your existing ZMK revision here.
      import: app/west.yml

    - name: zmk-feature-repress-guard
      remote: your-github-user
      revision: main

  self:
    path: config
```

Pin `revision` for this module to a commit SHA once it works on your keyboard.
When other external modules also subscribe to `zmk_position_state_changed`, put
this module before them in `west.yml` so they cannot act on events the guard is
supposed to drop.

No keymap node is required. Adding the module enables it with a 30 ms window.

## Configuration

Optional settings in the applicable `.conf` file:

```conf
CONFIG_ZMK_REPRESS_GUARD=y
CONFIG_ZMK_REPRESS_GUARD_WINDOW_MS=30
CONFIG_ZMK_REPRESS_GUARD_EXCLUDED_POSITIONS="0 12 63"
```

The exclusion list contains keymap position numbers separated by spaces or
commas. Events from those positions bypass the guard completely. Leave it empty
to filter every position.

Set `CONFIG_ZMK_REPRESS_GUARD=n` to compile it out.

## Event examples

```text
position 12: press @ 100 ms   -> forwarded
position 12: release @ 130 ms -> forwarded
position 12: press @ 150 ms   -> dropped (20 ms after release)
position 12: release @ 400 ms -> dropped (paired with dropped press)

position 7:  press @ 151 ms   -> handled independently
```

## Trade-offs

- An intentional second tap that begins inside the configured window is also
  suppressed. That is the unavoidable semantic trade-off of an eager,
  zero-latency filter.
- This operates on already-created ZMK position events. It complements, but
  does not replace, the switch scanner's electrical debounce.
- The first release is forwarded immediately. If that release is itself false
  while the switch is still physically held, downstream behaviors will see an
  early key-up; the following re-press and final release will be suppressed.
  Preserving a continuous hold in that case would require delaying the release.
- Events outside the physical keymap position range are intentionally left
  untouched.

## Test

The portable state machine can be tested without a Zephyr toolchain:

```sh
cc -std=c11 -Wall -Wextra -Werror -pedantic \
  -Iinclude src/repress_guard_state.c tests/repress_guard_state_test.c \
  -o repress_guard_state_test
./repress_guard_state_test
```

## Publish

```sh
git init
git add .
git commit -m "Initial ZMK repress guard module"
git branch -M main
git remote add origin https://github.com/YOUR_GITHUB_USERNAME/zmk-feature-repress-guard.git
git push -u origin main
```

## Compatibility

The module uses the `zmk_position_state_changed`, event-listener, and
`ZMK_KEYMAP_LEN` APIs present in ZMK v0.3.0 and current ZMK `main` as of July
2026.
