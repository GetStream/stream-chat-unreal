// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/ChatChannel.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "OnlineStatusSubheaderWidget.generated.h"

UCLASS()
class STREAMCHATUI_API UOnlineStatusSubheaderWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UOnlineStatusSubheaderWidget();

protected:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;

private:
    void UpdateLabel() const;
    FText GetLabel() const;
    FText GetSingleUserLabel(const FUserRef& User) const;
    FText GetMultiUserLabel() const;

    FTimerHandle Handle;
};
