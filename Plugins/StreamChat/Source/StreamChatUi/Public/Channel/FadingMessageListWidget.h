// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChatChannel.h"
#include "Components/EditableText.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBox.h"
#include "CoreMinimal.h"
#include "Message/FadingMessageWidget.h"
#include "StreamUserWidget.h"

#include "FadingMessageListWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UFadingMessageListWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(UChatChannel* InChannel, const FTimespan& InMessageLifetime);
    void Setup(UChatChannel* InChannel);

protected:
    UPROPERTY(meta = (BindWidget))
    UScrollBox* MessagesScrollBox;

    UPROPERTY(meta = (BindWidget))
    UEditableText* ChatInputBox;

    UPROPERTY(meta = (BindWidget))
    UWidget* ChatInputVisual;

    UPROPERTY(EditAnywhere, Category = Defaults)
    FName OpenChatInputActionName = TEXT("Chat");

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UFadingMessageWidget> FadingMessageWidgetClass = UFadingMessageWidget::StaticClass();

private:
    virtual void OnSetup() override;

    UFUNCTION()
    void MessagesUpdated(const TArray<FMessage>& Messages);

    UFUNCTION()
    void TextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

    UFUNCTION()
    void OpenChat();
    void CloseChat();

    UPROPERTY(Transient)
    UChatChannel* Channel;

    UPROPERTY(EditAnywhere, Category = Setup)
    FTimespan MessageLifetime = FTimespan::FromSeconds(20.f);
};
