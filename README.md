# ShockChainStudy

An Unreal Engine 5 C++ first-person combat prototype developed from the First Person template. The project explores modular combat systems, sequential chain lightning, enemy pursuit, and Blueprint-driven visual feedback.

## Gameplay demo

[Watch or download the gameplay recording](https://github.com/Josie-XX/ShockChainStudy/releases/tag/v0.1-showcase).

The original recording is provided as an MP4 release attachment, without recompression.

## Implemented features

- **First-person shooting:** Enhanced Input actions and hitscan tracing connect weapon input to the damage pipeline.
- **Modular combat:** reusable `HealthComponent`, `ShieldComponent`, and `ShockChainComponent` separate responsibilities. Shields apply a shock-specific multiplier and pass remaining raw damage to health.
- **Chain lightning:** sphere-overlap queries gather nearby targets; deduplication and distance sorting determine a fixed sequence from the initial hit. Timers apply delayed secondary hits, using weak actor references.
- **Enemy AI:** Character-based enemies use AIController and NavMesh pursuit, with Blueprint animation and death feedback.
- **Enemy generator:** configurable Single, Count, and Loop spawning modes, rotating spawn points, live-enemy tracking, and death-event subscriptions.
- **Presentation hooks:** delegates and Blueprint events connect combat state to dynamic shield materials, Niagara hit/break effects, and spline-based lightning.
- **Environment integration:** the local prototype integrates third-party sci-fi environment and character assets.

## Source map

| Area | Source |
| --- | --- |
| Player input and shooting | `Source/ShockChainStudy/ShockChainStudyCharacter.*` |
| Health, shields, chain lightning | `Source/ShockChainStudy/Components/` |
| Shock damage type | `Source/ShockChainStudy/Combat/DamageTypes/` |
| Enemy behavior | `Source/ShockChainStudy/ShockEnemyCharacter.*`, `ShockEnemyAIController.*` |
| Spawning | `Source/ShockChainStudy/ShockEnemyGenerator.*` |
| Template variants retained from UE | `Source/ShockChainStudy/Variant_Horror/`, `Variant_Shooter/` |

## Repository scope and setup

This is a **source showcase**, not a standalone playable build. It contains C++ source, sanitized default configuration, and the `.uproject` descriptor. It does **not** contain the local `Content/` directory: that directory includes Blueprint classes, input assets, maps, Niagara systems, and third-party materials/models. Without those assets, the original gameplay and maps cannot be reproduced by cloning this repository alone.

The local project uses **Unreal Engine 5.8**. To inspect or compile the C++ module, install the matching UE version and its supported C++ toolchain, generate project files from `ShockChainStudy.uproject`, and build the Editor target. To reproduce the demonstrated game, restore the corresponding assets into `Content/`, including Blueprint subclasses and their input/VFX references, then open the configured map and rebuild navigation as needed.

Generated build files, caches, logs, editor user settings, and the Android File Server debug token are excluded. The original project on the author's machine is unchanged.

## Credits and status

Gameplay extensions were developed in C++ with Blueprint integration. UE First Person template code and its variant examples remain in the source tree; they are distinguished from the custom `Shock*` classes and combat components above. The demo uses third-party scene, character, and visual assets, which are not redistributed here.

This is a learning and portfolio prototype. The recording demonstrates the integrated local project; no packaged release or automated performance benchmark is claimed.

No license grant for third-party or Unreal Engine template content is implied by this repository.
