// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MessageTypeDto.generated.h"

/**
 * @brief DTO for type of message
 * @see https://getstream.io/chat/docs/other-rest/message_format/#message-types
 * @ingroup StreamChatDto
 */
UENUM()
enum class EMessageTypeDto : uint8
{
    Regular,
    Ephemeral,
    Error,
    Reply,
    System,
    Deleted
};
