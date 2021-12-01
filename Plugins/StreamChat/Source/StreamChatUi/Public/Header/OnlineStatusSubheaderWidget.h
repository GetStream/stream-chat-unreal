// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/ChatChannel.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"

#include "OnlineStatusSubheaderWidget.generated.h"

UCLASS()
class STREAMCHATUI_API UOnlineStatusSubheaderWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;

private:
    void UpdateLabel() const;
    FText GetLabel() const;
    FText GetSingleUserLabel(const FUserRef& User) const;
    FText GetMultiUserLabel() const;
    static const FDateTime& GetLastActive(const FUserRef& User);

    UPROPERTY(Transient)
    UChatChannel* Channel;

    FTimerHandle Handle;
};
