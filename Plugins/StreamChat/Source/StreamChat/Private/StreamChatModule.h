// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

/**
 * @defgroup StreamChat Stream Chat
 * @brief This module is the main interface to the Stream Chat low level client functionality
 */

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FStreamChatModule final : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
