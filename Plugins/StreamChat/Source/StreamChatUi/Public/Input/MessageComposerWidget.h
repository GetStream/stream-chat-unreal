// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/ChatChannel.h"
#include "Components/Button.h"
#include "Components/Image.h"
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
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void EditMessage(const FMessage& Message);

    UPROPERTY(EditAnywhere, Category = "Button")
    UTexture2D* ButtonTextureNormal;

    UPROPERTY(EditAnywhere, Category = "Button")
    UTexture2D* ButtonTextureDisabled;

    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconTextureSend;

    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconTextureConfirm;

    UPROPERTY(EditAnywhere, Category = "Icon")
    FMargin IconPaddingSend;

    UPROPERTY(EditAnywhere, Category = "Icon")
    FMargin IconPaddingConfirm;

protected:
    UPROPERTY(meta = (BindWidget))
    UMessageInputWidget* MessageInput;

    UPROPERTY(meta = (BindWidget))
    UButton* SendMessageButton;

    UPROPERTY(meta = (BindWidget))
    UImage* SendMessageIcon;

    UPROPERTY(meta = (BindWidget))
    UButton* CancelEditingButton;

    UPROPERTY(meta = (BindWidget))
    UPanelWidget* CancelEditingHeaderPanel;

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
