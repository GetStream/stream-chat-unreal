// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Misc/EngineVersionComparison.h"

/**
 * @brief The key type used by FJsonObject for its attributes
 *
 * UE 5.8 changed these keys from FString to UE::FSharedString, so that keys recurring throughout a
 * json structure can share a single allocation. Aliasing the type here keeps the rest of the module
 * free of engine version checks.
 *
 * Note that a key is not an FString on every engine, so it has no FString member functions. Wrap it
 * in MakeStringView() to compare or search it, which works for both underlying types.
 *
 * @ingroup StreamChatJson
 */
#if UE_VERSION_OLDER_THAN(5, 8, 0)
using FJsonAttributeKey = FString;
#else
using FJsonAttributeKey = FJsonObject::FStringType;
#endif

/**
 * @brief The attributes of a json object, as stored in FJsonObject::Values
 * @ingroup StreamChatJson
 */
using FJsonAttributeMap = TMap<FJsonAttributeKey, TSharedPtr<FJsonValue>>;
