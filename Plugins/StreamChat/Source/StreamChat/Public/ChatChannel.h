// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ChatChannel.generated.h"

class FChatApi;
struct FChannelState;

/**
 *
 */
UCLASS()
class STREAMCHAT_API UChatChannel final : public UObject
{
    GENERATED_BODY()

public:
    static UChatChannel*
    Create(const TSharedRef<FChatApi>&, const FString& ConnectionId, const FString& Type, const FString& Id);

    void Watch(TFunction<void(const FChannelState&)> Callback = {}) const;

private:
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
    FString Type;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
    FString Id;

    FString ConnectionId;

    TSharedPtr<FChatApi> Api;
};
