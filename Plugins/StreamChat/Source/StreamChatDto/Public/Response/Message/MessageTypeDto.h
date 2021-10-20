#pragma once

#include "CoreMinimal.h"

UENUM()
/**
 * https://getstream.io/chat/docs/other-rest/message_format/#message-types
 */
enum class EMessageTypeDto : uint8
{
    Regular,
    Ephemeral,
    Error,
    Reply,
    System,
    Deleted
};
