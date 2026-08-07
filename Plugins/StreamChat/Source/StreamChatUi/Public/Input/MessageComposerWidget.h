// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AttachmentPicker.h"
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

    /// Called before message is sent. Sending message will be cancelled if MessageText is set to an empty string.
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSendMessage, const FString&, MessageText);
    UPROPERTY(BlueprintAssignable)
    FOnSendMessage OnSendMessage;

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

    /**
     * @brief Icon for the attach button.
     *
     * The plugin ships no paperclip of its own, so when this is unset the button draws a plus built
     * out of two bars instead. Assign a texture here to get a proper icon.
     */
    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconTextureAttach;

    UPROPERTY(EditAnywhere, Category = "Attachment")
    FVector2D AttachButtonSize = FVector2D{32.f, 32.f};

    /// Length of each bar of the drawn plus, as a fraction of the button
    UPROPERTY(EditAnywhere, Category = "Attachment", meta = (ClampMin = "0.1", ClampMax = "1.0"))
    float AttachGlyphScale = 0.5f;

    /// Thickness of each bar of the drawn plus, as a fraction of the button
    UPROPERTY(EditAnywhere, Category = "Attachment", meta = (ClampMin = "0.01", ClampMax = "0.5"))
    float AttachGlyphThickness = 0.06f;

private:
    UFUNCTION()
    void OnInputTextChanged(const FText& Text);
    UFUNCTION()
    void OnInputTextCommit(const FText& Text, ETextCommit::Type CommitMethod);
    UFUNCTION()
    void OnCancelEditingButtonClicked();
    UFUNCTION()
    void OnSendButtonClicked();
    UFUNCTION()
    void OnAttachButtonClicked();

    void SendMessage();
    void StopEditMessage();
    void Keystroke();
    void StopTyping();

    /// Build the attach button and its status line, if the app registered a picker to feed them
    void CreateAttachmentWidgets();
    UWidget* BuildAttachIcon();
    UWidget* BuildAttachGlyph();
    void Upload(const FPickedAttachment& Picked);
    void SetStatus(const FString& Status);
    void UpdateAttachmentAppearance();

    void UpdateSendButtonAppearance(bool bEnabled);
    /// A message may carry text, attachments, or both, so the send button follows all of it
    void RefreshSendButtonEnabled();

    enum class ESendButtonIconAppearance
    {
        Send,
        Confirm
    };
    void UpdateEditMessageAppearance(ESendButtonIconAppearance Appearance);

    TOptional<FMessage> EditedMessage;

    /// Uploaded and waiting to go out with the next message
    UPROPERTY(Transient)
    TArray<FAttachment> PendingAttachments;

    // Spawned in C++: WBP_MessageComposer predates attachments and has no slot to bind them to
    UPROPERTY(Transient)
    UButton* AttachButton;
    /// Whatever the attach button draws: the icon texture, or the plus built out of bars
    UPROPERTY(Transient)
    UWidget* AttachButtonIcon;
    /// The bars of the drawn plus, kept so the theme can colour them
    UPROPERTY(Transient)
    TArray<UImage*> AttachGlyphBars;
    UPROPERTY(Transient)
    UTextBlock* AttachStatusTextBlock;

    /// One upload at a time, so the status line always describes the upload in flight
    bool bUploading = false;
};
