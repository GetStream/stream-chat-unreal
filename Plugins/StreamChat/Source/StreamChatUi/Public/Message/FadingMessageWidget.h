// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "BubbleStackPosition.h"
#include "Channel/Message.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "MessageSide.h"
#include "StreamUserWidget.h"

#include "FadingMessageWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UFadingMessageWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, const FTimespan& InLifetime);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FadeAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FMessage Message;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FTimespan Lifetime = FTimespan::FromSeconds(20.f);

private:
    virtual void OnSetup() override;

    virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

    void FadeOut();

    FTimerHandle TimerHandle;
};
