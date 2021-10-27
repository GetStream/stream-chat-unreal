#pragma once

#include "CoreMinimal.h"

struct FUserObjectDto;
class FChatSocketEvents;

class STREAMCHATWEBSOCKET_API IChatSocket : public TSharedFromThis<IChatSocket>
{
public:
    static TSharedRef<IChatSocket> Create(
        const FString& ApiKey,
        const FString& Token,
        const FString& Host,
        const FUserObjectDto& User);
    virtual ~IChatSocket() = default;

    virtual void Connect(TFunction<void()> Callback) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    virtual const FString& GetConnectionId() const = 0;

    virtual FChatSocketEvents& Events() = 0;
};
