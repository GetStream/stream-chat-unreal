<p align="center">
  <img src="ReadmeAssets/256px.png"/>
</p>

<p align="center">
    The official Unreal SDK for <a href="https://getstream.io/chat/sdk/unreal/">Stream Chat</a>.
</p>

<p align="center">
    <a href="https://getstream.io/chat/sdk/unreal/">Website</a>
    |
    <a href="https://getstream.io/chat/unreal/tutorial/">Tutorial</a>
    |
    <a href="https://getstream.io/chat/docs/unreal/">SDK Documentation</a>
    |
    <a href="https://getstream.github.io/stream-chat-unreal/">C++ Documentation</a>
    |
    <a href="https://getstream.io/chat/trial/">Register for API Key</a>
</p>

<p align="center">
    <a href="https://github.com/GetStream/stream-chat-unreal/actions/workflows/ci.yml">
        <img src="https://github.com/GetStream/stream-chat-unreal/actions/workflows/ci.yml/badge.svg" alt="CI">
    </a>
    &nbsp;
    <a href="https://github.com/GetStream/stream-chat-unreal/releases/latest">
        <img alt="GitHub release (latest by date)" src="https://img.shields.io/github/v/release/GetStream/stream-chat-unreal">
    </a>
    &nbsp;
    <img alt="Unreal Engine 5.5 | 5.7 | 5.8" src="https://img.shields.io/badge/Unreal%20Engine-5.5%20%7C%205.7%20%7C%205.8-blue">
</p>

---

The **Stream Chat SDK** is the official Unreal SDK for [Stream Chat](https://getstream.io/chat/sdk/unreal/), a service for building chat and messaging into games and applications.

> [!NOTE]
> This SDK is in beta. Its surface may still change between releases, C++ and Blueprint coverage can
> differ by operation, and some Stream Chat features are not yet available in Unreal — see [Feature
> support](#feature-support).

## Requirements

| | |
|---|---|
| **Unreal Engine** | 5.5, 5.7, 5.8 |
| **Platforms** | Windows (Win64), macOS, Linux, Android, iOS |
| **Language** | C++ and Blueprint — some lower-level and pagination APIs are C++ only |

Older engine versions are not supported by the current release. If you are on UE 4.27, 5.0, or 5.1,
use [v1.3.0](https://github.com/GetStream/stream-chat-unreal/releases/tag/v1.3.0).
Only versions exercised by the maintained CI and release matrix are listed as supported; UE 5.6 is not
currently part of that matrix.

## Getting started

1. Download the [latest release](https://github.com/GetStream/stream-chat-unreal/releases/latest) and copy it into the `Plugins` directory of your project.
2. Launch Unreal Engine and ensure the **Stream Chat** plugin is enabled in the Plugins configuration page.
3. Start integrating Chat into your project — the [Tutorial](https://getstream.io/chat/unreal/tutorial/) walks through a first integration.

If you previously acquired the plugin through the Epic Games Launcher, it remains available from your
Vault. New installs should use the GitHub releases above.

## Feature support

Implemented:

- **Messaging** — send, edit, fetch, full-text search, soft delete through the channel API, and hard
  delete through the lower-level C++ API, with C++ pagination in both directions
- **Channels** — query, watch, create, update, truncate, hide/show, freeze, and member management
- **Reactions** — send with score and enforce-unique semantics, remove, and paginate from C++
- **Read state** — mark read, mark all read, unread counts, and per-user read tracking
- **Typing indicators** — keystroke debouncing and stop events
- **Moderation** — ban, shadow ban, mute users, mute channels, flag messages and users, query banned users
- **Slow mode** — enable and disable with a configurable cooldown
- **Push notifications** — device registration across multiple push providers
- **Presence** — online state, watchers, and channel own-capabilities
- **Real-time events** — WebSocket transport with health checks and reconnection, plus typed event subscription from C++ and Blueprint
- **UI widgets** — an early preview of channel list, message list, composer, reactions, and avatar widgets

Not yet available in the Unreal SDK — [open an issue](https://github.com/GetStream/stream-chat-unreal/issues)
if you need one of these:

- Attachments and file/image uploads
- Sending threaded replies and the thread list
- Quoted messages, mentions, and pinning messages
- Offline persistence and optimistic sending
- User blocking, channel archiving, and channel pinning
- Polls, draft messages, message reminders, and location sharing

## Free for Makers

Stream is free for most side and hobby projects. You can use Stream Chat for free if you have fewer
than five team members and no more than $10,000 in monthly revenue.

## Contributing

The SDK is fully open source — the complete implementation is in this repository.

- Build the plugin: `just build`
- Run the unit tests: `just test`
- Switch engine versions: `just set-engine 5.8`
- Format C++ before committing: `just format`

## We are hiring

Our APIs are used by more than a billion end-users, and you'll have a chance to make a huge impact on
the product within a team of the strongest engineers all over the world.
Check out our current openings and apply via [Stream's website](https://getstream.io/team/#jobs).

---

## Sample Asset License

Some of the assets used in the sample projects in this repository use free assets originally obtained
from the Unreal Engine Marketplace, which Epic replaced with [Fab](https://www.fab.com/) in October
2024. Please refer to the licensing terms published on Fab for their usage rights.
