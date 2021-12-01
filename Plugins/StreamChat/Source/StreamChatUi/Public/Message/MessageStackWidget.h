// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Avatar/AvatarWidget.h"
#include "Channel/Message.h"
#include "Components/VerticalBox.h"
#include "CoreMinimal.h"
#include "MessageWidget.h"
#include "StreamUserWidget.h"
#include "TextBubbleWidget.h"
#include "TimestampWidget.h"

#include "MessageStackWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageStackWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const TArray<FMessage>& InMessages, EMessageSide InSide);

    UFUNCTION(BlueprintPure, meta = (DevelopmentOnly), Category = "Stream Chat")
    FString Describe() const;

protected:
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* MessagesPanel;

    UPROPERTY(meta = (BindWidget))
    UTimestampWidget* Timestamp;

    UPROPERTY(meta = (BindWidget))
    UPanelWidget* AvatarSlot;

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    int32 AvatarSize = 36;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UMessageWidget> MessageWidgetClass = UMessageWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UAvatarWidget> AvatarWidgetClass = UAvatarWidget::StaticClass();

private:
    virtual void OnSetup() override;

    UPROPERTY(EditAnywhere, Category = Setup)
    TArray<FMessage> Messages;

    UPROPERTY(EditAnywhere, Category = Setup)
    EMessageSide Side;
};
