// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Button.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "ChannelListHeaderWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UChannelListHeaderWidget final : public UStreamWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* Icon;

    UPROPERTY(meta = (BindWidget))
    UButton* Button;

private:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void OnButtonClick();
};
