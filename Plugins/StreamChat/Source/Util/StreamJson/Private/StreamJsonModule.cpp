// Copyright Stream.IO, Inc. All Rights Reserved.

/**
 * @defgroup StreamChatJson JSON
 * @brief This module contains helper functionality for serializing and deserializing JSON data to and from `USTRUCT`s using `UPROPERTY`s
 * @note You should use the "Transient" `UPROPERTY`-specifier to skip Serialization while still performing Deserialization.
 */

#include "Modules/ModuleManager.h"
IMPLEMENT_GAME_MODULE(FDefaultModuleImpl, StreamJson);
