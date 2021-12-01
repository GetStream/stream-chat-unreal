// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "StreamChatClientComponent.h"

#include "StreamChatSampleHud.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATSAMPLE_API AStreamChatSampleHud final : public AHUD
{
    GENERATED_BODY()

public:
    AStreamChatSampleHud();

    UFUNCTION(BlueprintImplementableEvent)
    void OnConnect();

private:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
    UStreamChatClientComponent* Client;

    UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
    TArray<UChatChannel*> Channels;
};
