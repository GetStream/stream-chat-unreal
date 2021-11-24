// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FChatSocketEvents;
class FTokenManager;
struct FOwnUserDto;
struct FUserObjectDto;

/**
 * @brief Interface to a Stream Chat WebSocket connection
 * @ingroup StreamChatWebSocket
 */
class STREAMCHATWEBSOCKET_API IChatSocket : public TSharedFromThis<IChatSocket>
{
public:
    static TSharedRef<IChatSocket> Create(const TSharedRef<FTokenManager>&, const FString& ApiKey, const FString& Host, const FUserObjectDto& User);
    virtual ~IChatSocket();

    virtual void Connect(TFunction<void(const FOwnUserDto&)> Callback) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    virtual const FString& GetConnectionId() const = 0;

    virtual FChatSocketEvents& Events() = 0;
};
