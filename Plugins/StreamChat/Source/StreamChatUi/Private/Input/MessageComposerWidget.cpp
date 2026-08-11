// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Input/MessageComposerWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Brushes/SlateImageBrush.h"
#include "Components/ButtonSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Context/ChannelContextWidget.h"
#include "ThemeDataAsset.h"
#include "TimerManager.h"

namespace
{
/**
 * The nearest ancestor of Widget that can hold more than one child.
 *
 * WBP_MessageComposer wraps the send button in a size box, and a size box holds exactly one child:
 * inserting into it would evict the button rather than sit beside it.
 */
UPanelWidget* FindPanelWithRoomForMore(const UWidget* Widget)
{
    UPanelWidget* Panel = Widget ? Cast<UPanelWidget>(Widget->GetParent()) : nullptr;
    while (Panel && !Panel->CanHaveMultipleChildren())
    {
        Panel = Cast<UPanelWidget>(Panel->GetParent());
    }
    return Panel;
}
}    // namespace

UWidget* UMessageComposerWidget::BuildAttachIcon()
{
    UImage* Icon = WidgetTree->ConstructWidget<UImage>();
    Icon->SetBrushFromTexture(IconTextureAttach, true);
    return Icon;
}

UWidget* UMessageComposerWidget::BuildAttachGlyph()
{
    // Two crossed bars rather than a "+" in a text block. A glyph sits on its font's baseline, with
    // room reserved below it for descenders, so centring the text block leaves the plus visibly high;
    // these are centred by construction. The plugin ships no paperclip texture to use instead.
    UOverlay* Glyph = WidgetTree->ConstructWidget<UOverlay>();

    const float Length = AttachButtonSize.X * AttachGlyphScale;
    const float Thickness = FMath::Max(1.f, FMath::RoundToFloat(AttachButtonSize.X * AttachGlyphThickness));

    for (const FVector2D& BarSize : {FVector2D{Length, Thickness}, FVector2D{Thickness, Length}})
    {
        // A bar is an image with no texture, which slate draws as a solid rectangle in the tint
        // colour, the same way the dividers elsewhere in this plugin are drawn. Its size comes from
        // the brush, since UImage::SetBrushSize is deprecated and SetDesiredSizeOverride is not
        // reapplied when the slate widget is rebuilt.
        UImage* Bar = WidgetTree->ConstructWidget<UImage>();
        FSlateBrush BarBrush = Bar->GetBrush();
        BarBrush.ImageSize = BarSize;
        Bar->SetBrush(BarBrush);
        AttachGlyphBars.Add(Bar);

        if (UOverlaySlot* BarSlot = Cast<UOverlaySlot>(Glyph->AddChild(Bar)))
        {
            BarSlot->SetHorizontalAlignment(HAlign_Center);
            BarSlot->SetVerticalAlignment(VAlign_Center);
        }
    }

    return Glyph;
}

void UMessageComposerWidget::NativeOnInitialized()
{
    if (MessageInput)
    {
        MessageInput->OnTextChanged.AddDynamic(this, &UMessageComposerWidget::OnInputTextChanged);
        MessageInput->OnTextCommitted.AddDynamic(this, &UMessageComposerWidget::OnInputTextCommit);
    }
    if (CancelEditingButton)
    {
        CancelEditingButton->OnClicked.AddDynamic(this, &UMessageComposerWidget::OnCancelEditingButtonClicked);
    }
    if (SendMessageButton)
    {
        SendMessageButton->OnClicked.AddDynamic(this, &UMessageComposerWidget::OnSendButtonClicked);
    }

    CreateAttachmentWidgets();

    Super::NativeOnInitialized();
}

void UMessageComposerWidget::CreateAttachmentWidgets()
{
    // No picker means the app has no way of choosing a file, so offering the button would be a lie
    if (!FAttachmentPicker::HasProvider() || !WidgetTree || !SendMessageButton)
    {
        return;
    }

    UPanelWidget* InputRow = FindPanelWithRoomForMore(SendMessageButton);
    if (!InputRow)
    {
        return;
    }

    AttachButton = WidgetTree->ConstructWidget<UButton>();
    AttachButton->OnClicked.AddDynamic(this, &UMessageComposerWidget::OnAttachButtonClicked);

    // Flat, and with the style's own padding removed: the engine's default button background would
    // look nothing like the rest of the composer, and its padding would push the icon off centre.
    FButtonStyle Style = AttachButton->GetStyle();
    for (FSlateBrush* Brush : {&Style.Normal, &Style.Hovered, &Style.Pressed, &Style.Disabled})
    {
        Brush->DrawAs = ESlateBrushDrawType::NoDrawType;
    }
    Style.NormalPadding = FMargin{0.f};
    Style.PressedPadding = FMargin{0.f};
    AttachButton->SetStyle(Style);

    AttachButtonIcon = IconTextureAttach ? BuildAttachIcon() : BuildAttachGlyph();
    if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(AttachButton->SetContent(AttachButtonIcon)))
    {
        ButtonSlot->SetPadding(FMargin{0.f});
        ButtonSlot->SetHorizontalAlignment(HAlign_Center);
        ButtonSlot->SetVerticalAlignment(VAlign_Center);
    }

    USizeBox* Sizer = WidgetTree->ConstructWidget<USizeBox>();
    Sizer->SetWidthOverride(AttachButtonSize.X);
    Sizer->SetHeightOverride(AttachButtonSize.Y);
    Sizer->AddChild(AttachButton);

    if (UHorizontalBoxSlot* BoxSlot = Cast<UHorizontalBoxSlot>(InputRow->InsertChildAt(0, Sizer)))
    {
        BoxSlot->SetVerticalAlignment(VAlign_Center);
    }

    // The status line goes below the "Edit Message" header, if this composer has one
    if (CancelEditingHeaderPanel)
    {
        if (UPanelWidget* Column = FindPanelWithRoomForMore(CancelEditingHeaderPanel))
        {
            AttachStatusTextBlock = WidgetTree->ConstructWidget<UTextBlock>();
            AttachStatusTextBlock->SetVisibility(ESlateVisibility::Collapsed);
            FSlateFontInfo Font = AttachStatusTextBlock->GetFont();
            Font.Size = 12;
            AttachStatusTextBlock->SetFont(Font);

            // Directly below the header when they share a parent, otherwise at the top of whatever
            // column we did find, which is still above the input
            const int32 HeaderIndex = Column->GetChildIndex(CancelEditingHeaderPanel);
            Column->InsertChildAt(HeaderIndex == INDEX_NONE ? 0 : HeaderIndex + 1, AttachStatusTextBlock);
        }
    }
}

void UMessageComposerWidget::NativeConstruct()
{
    // Can only find the ChannelContextWidget once this widget is added to the UI hierarchy (Construct)
    if (UChannelContextWidget* Context = UChannelContextWidget::TryGet(this))
    {
        Context->OnStartEditMessage.AddDynamic(this, &UMessageComposerWidget::EditMessage);
        Context->OnThreadChanged.AddDynamic(this, &UMessageComposerWidget::OnThreadChanged);
    }

    if (MessageInput)
    {
        MessageInput->SetKeyboardFocus();
    }

    if (EditMessageTextBlock && DefaultHeaderText.IsEmpty())
    {
        // Captured before anything rewrites it, so the banner can go back to saying what the WBP says
        DefaultHeaderText = EditMessageTextBlock->GetText();
    }

    if (const UThemeDataAsset* Theme = UThemeDataAsset::Get(this))
    {
        if (BackgroundBorder)
        {
            BackgroundBorder->SetBrushColor(Theme->GetPaletteColor(Theme->MessageComposerBackgroundColor));
        }
        if (EditMessageTextBlock)
        {
            EditMessageTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->MessageComposerHeaderTextColor));
        }
        if (TopBorderImage)
        {
            TopBorderImage->SetColorAndOpacity(Theme->GetPaletteColor(Theme->MessageComposerBorderColor));
        }
        if (AttachButtonIcon)
        {
            const FLinearColor& Color = Theme->GetPaletteColor(Theme->MessageComposerAttachIconColor);
            if (UImage* AsIcon = Cast<UImage>(AttachButtonIcon))
            {
                AsIcon->SetColorAndOpacity(Color);
            }
            for (UImage* Bar : AttachGlyphBars)
            {
                Bar->SetColorAndOpacity(Color);
            }
        }
        if (AttachStatusTextBlock)
        {
            AttachStatusTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->MessageComposerAttachStatusTextColor));
        }
    }

    UpdateSendButtonAppearance(false);
    UpdateEditMessageAppearance(ESendButtonIconAppearance::Send);

    Super::NativeConstruct();
}

void UMessageComposerWidget::NativeDestruct()
{
    if (UChannelContextWidget* Context = UChannelContextWidget::TryGet(this))
    {
        Context->OnStartEditMessage.RemoveDynamic(this, &UMessageComposerWidget::EditMessage);
        Context->OnThreadChanged.RemoveDynamic(this, &UMessageComposerWidget::OnThreadChanged);
    }
    Super::NativeDestruct();
}

void UMessageComposerWidget::EditMessage(const FMessage& Message)
{
    EditedMessage = Message;
    if (MessageInput)
    {
        MessageInput->SetText(FText::FromString(Message.Text));
    }
    UpdateEditMessageAppearance(ESendButtonIconAppearance::Confirm);
}

void UMessageComposerWidget::OnThreadChanged(const bool bThreadOpen, const FMessage& ParentMessage)
{
    if (bThreadOpen)
    {
        ThreadParentMessage = ParentMessage;
    }
    else
    {
        ThreadParentMessage.Reset();
    }

    // Anything half-typed was meant for wherever the user was before
    if (MessageInput)
    {
        MessageInput->SetText(FText::GetEmpty());
    }
    EditedMessage.Reset();
    UpdateEditMessageAppearance(ESendButtonIconAppearance::Send);
    RefreshSendButtonEnabled();
}

void UMessageComposerWidget::OnInputTextChanged(const FText&)
{
    Keystroke();
    RefreshSendButtonEnabled();
}

void UMessageComposerWidget::OnInputTextCommit(const FText&, const ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        SendMessage();

        if (MessageInput)
        {
            GetWorld()->GetTimerManager().SetTimerForNextTick([&] { MessageInput->SetKeyboardFocus(); });
        }
    }
    StopTyping();
}

void UMessageComposerWidget::OnCancelEditingButtonClicked()
{
    if (EditedMessage)
    {
        StopEditMessage();
        return;
    }

    // Nothing being edited, so the banner is the thread one and this is the way out of the thread
    if (UChannelContextWidget* Context = UChannelContextWidget::TryGet(this))
    {
        Context->CloseThread();
    }
}

void UMessageComposerWidget::OnSendButtonClicked()
{
    SendMessage();
    StopTyping();
}

void UMessageComposerWidget::OnAttachButtonClicked()
{
    if (bUploading)
    {
        return;
    }

    FAttachmentPicker::Pick(
        [WeakThis = TWeakObjectPtr<UMessageComposerWidget>(this)](const TOptional<FPickedAttachment>& Picked)
        {
            if (WeakThis.IsValid() && Picked.IsSet())
            {
                WeakThis->Upload(*Picked);
            }
        });
}

void UMessageComposerWidget::Upload(const FPickedAttachment& Picked)
{
    UChatChannel* Channel = UChannelContextWidget::TryGetChannel(this);
    if (!Channel || Picked.Content.IsEmpty())
    {
        return;
    }

    bUploading = true;
    UpdateAttachmentAppearance();
    SetStatus(FString::Printf(TEXT("Uploading %s..."), *Picked.FileName));

    const auto OnUploaded = [WeakThis = TWeakObjectPtr<UMessageComposerWidget>(this), FileName = Picked.FileName](const FAttachment& Attachment)
    {
        if (!WeakThis.IsValid())
        {
            return;
        }

        WeakThis->bUploading = false;

        // The upload endpoint answers with a URL and nothing else, so an empty one is the failure
        if (Attachment.GetUrl().IsEmpty())
        {
            WeakThis->UpdateAttachmentAppearance();
            WeakThis->SetStatus(FString::Printf(TEXT("Could not upload %s"), *FileName));
            return;
        }

        WeakThis->PendingAttachments.Add(Attachment);
        WeakThis->UpdateAttachmentAppearance();
    };

    if (Picked.bIsImage)
    {
        Channel->UploadImage(Picked.FileName, Picked.Content, OnUploaded);
    }
    else
    {
        Channel->UploadFile(Picked.FileName, Picked.Content, OnUploaded);
    }
}

void UMessageComposerWidget::SetStatus(const FString& Status)
{
    if (!AttachStatusTextBlock)
    {
        return;
    }

    AttachStatusTextBlock->SetText(FText::FromString(Status));
    AttachStatusTextBlock->SetVisibility(Status.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UMessageComposerWidget::UpdateAttachmentAppearance()
{
    if (AttachButton)
    {
        AttachButton->SetIsEnabled(!bUploading);
    }
    RefreshSendButtonEnabled();

    if (bUploading)
    {
        // Whoever started the upload names the file being uploaded
        return;
    }

    if (PendingAttachments.IsEmpty())
    {
        SetStatus({});
    }
    else if (PendingAttachments.Num() == 1)
    {
        SetStatus(FString::Printf(TEXT("%s attached"), *PendingAttachments[0].Title));
    }
    else
    {
        SetStatus(FString::Printf(TEXT("%d files attached"), PendingAttachments.Num()));
    }
}

void UMessageComposerWidget::RefreshSendButtonEnabled()
{
    const bool bHasText = MessageInput && !MessageInput->GetText().IsEmpty();
    UpdateSendButtonAppearance(bHasText || !PendingAttachments.IsEmpty());
}

void UMessageComposerWidget::SendMessage()
{
    if (!MessageInput)
    {
        return;
    }

    const FString Text = MessageInput->GetText().ToString();
    // A message with an attachment and no caption is perfectly valid
    if (Text.IsEmpty() && PendingAttachments.IsEmpty())
    {
        return;
    }

    if (!Text.IsEmpty() && Text[0] == TEXT('/'))
    {
        UE_LOG(LogTemp, Warning, TEXT("The Unreal SDK doesn't currently support slash commands"));
        return;
    }

    OnSendMessage.Broadcast(Text);

    UChatChannel* Channel = UChannelContextWidget::TryGetChannel(this);
    if (!Channel)
    {
        return;
    }

    if (EditedMessage)
    {
        EditedMessage->Text = Text;
        if (EditedMessage->IsBounced())
        {
            // Moderation never stored the bounced message, so there is nothing on the server to edit.
            // The rephrased text goes out as a fresh attempt instead.
            Channel->ResendMessage(*EditedMessage);
        }
        else
        {
            Channel->UpdateMessage(*EditedMessage);
        }
        StopEditMessage();
    }
    else
    {
        FMessage Message{Text};
        Message.Attachments = MoveTemp(PendingAttachments);
        PendingAttachments.Reset();

        if (ThreadParentMessage)
        {
            // Not shown in the channel: a reply belongs to its thread unless the user says otherwise,
            // and this composer has no control to say otherwise with
            Channel->SendReply(Message, *ThreadParentMessage);
        }
        else
        {
            Channel->SendMessage(Message);
        }
        MessageInput->SetText(FText::GetEmpty());
        UpdateAttachmentAppearance();
    }
}

void UMessageComposerWidget::UpdateSendButtonAppearance(const bool bEnabled)
{
    if (SendMessageButton)
    {
        SendMessageButton->SetEnabled(bEnabled);
    }
}

void UMessageComposerWidget::StopEditMessage()
{
    EditedMessage.Reset();

    UpdateEditMessageAppearance(ESendButtonIconAppearance::Send);

    if (MessageInput)
    {
        MessageInput->SetText(FText::GetEmpty());
    }
}

void UMessageComposerWidget::Keystroke()
{
    UChatChannel* Channel = UChannelContextWidget::TryGetChannel(this);
    if (Channel)
    {
        Channel->KeyStroke();
    }
}

void UMessageComposerWidget::StopTyping()
{
    if (UChatChannel* Channel = UChannelContextWidget::TryGetChannel(this))
    {
        Channel->StopTyping();
    }
}

void UMessageComposerWidget::UpdateEditMessageAppearance(const ESendButtonIconAppearance Appearance)
{
    if (SendMessageButton)
    {
        UTexture2D* Texture = Appearance == ESendButtonIconAppearance::Send ? IconTextureSend : IconTextureConfirm;
        SendMessageButton->SetIconFromTexture(Texture);
        const FMargin IconPadding = Appearance == ESendButtonIconAppearance::Send ? IconPaddingSend : IconPaddingConfirm;
        SendMessageButton->SetIconPadding(IconPadding);
    }

    UpdateHeaderBanner();
}

void UMessageComposerWidget::UpdateHeaderBanner()
{
    // Editing wins over the thread banner: you can edit a message from inside a thread, and knowing
    // which message is being rewritten matters more than being reminded where you are.
    const bool bEditing = EditedMessage.IsSet();
    const bool bInThread = ThreadParentMessage.IsSet();

    if (EditMessageTextBlock)
    {
        // The editing label belongs to the WBP, so it is restored rather than reinvented here
        EditMessageTextBlock->SetText(!bEditing && bInThread ? ThreadHeaderText : DefaultHeaderText);
    }

    if (CancelEditingHeaderPanel)
    {
        const ESlateVisibility PanelVisibility = bEditing || bInThread ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
        CancelEditingHeaderPanel->SetVisibility(PanelVisibility);
    }
}
