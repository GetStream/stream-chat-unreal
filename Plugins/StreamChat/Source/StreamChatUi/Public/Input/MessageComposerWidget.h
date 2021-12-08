// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/ChatChannel.h"
#include "Common/IconButton.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "MessageInputWidget.h"
#include "Types/SlateEnums.h"

#include "MessageComposerWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageComposerWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void EditMessage(const FMessage& Message);

protected:
    UPROPERTY(meta = (BindWidget))
    UMessageInputWidget* MessageInput;

    UPROPERTY(meta = (BindWidget))
    UIconButton* SendMessageButton;

    UPROPERTY(meta = (BindWidget))
    UButton* CancelEditingButton;

    UPROPERTY(meta = (BindWidget))
    UPanelWidget* CancelEditingHeaderPanel;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EditMessageTextBlock;

    UPROPERTY(meta = (BindWidget))
    UBorder* BackgroundBorder;

    UPROPERTY(meta = (BindWidget))
    UImage* TopBorderImage;

    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconTextureSend;

    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconTextureConfirm;

    UPROPERTY(EditAnywhere, Category = "Icon")
    FMargin IconPaddingSend;

    UPROPERTY(EditAnywhere, Category = "Icon")
    FMargin IconPaddingConfirm;

private:
    UFUNCTION()
    void OnInputTextChanged(const FText& Text);
    UFUNCTION()
    void OnInputTextCommit(const FText& Text, ETextCommit::Type CommitMethod);
    UFUNCTION()
    void OnCancelEditingButtonClicked();
    UFUNCTION()
    void OnSendButtonClicked();

    void SendMessage();
    void StopEditMessage();

    void UpdateSendButtonAppearance(bool bEnabled);

    enum class ESendButtonIconAppearance
    {
        Send,
        Confirm
    };
    void UpdateEditMessageAppearance(ESendButtonIconAppearance Appearance);

    TOptional<FMessage> EditedMessage;
};
