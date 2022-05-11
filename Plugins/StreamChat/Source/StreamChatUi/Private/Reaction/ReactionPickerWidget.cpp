// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/ReactionPickerWidget.h"

#include "Channel/ChatChannel.h"
#include "ThemeDataAsset.h"
#include "User/UserManager.h"

namespace
{
const FName ReactionTypes[] = {TEXT("love"), TEXT("like"), TEXT("sad"), TEXT("haha"), TEXT("wow")};
}

UReactionPickerWidget::UReactionPickerWidget()
{
    bWantsChannel = true;
    bWantsTheme = true;
}

void UReactionPickerWidget::Setup(const FMessage& InMessage)
{
    Message = InMessage;

    Super::Setup();
}

void UReactionPickerWidget::OnSetup()
{
    if (ButtonsPanel)
    {
        ButtonsPanel->ClearChildren();
        for (const FName& ReactionType : ReactionTypes)
        {
            UReactionPickerButtonWidget* Widget = CreateWidget<UReactionPickerButtonWidget>(this, ReactionPickerButtonWidgetClass);
            Widget->Setup(ReactionType, Message.User.IsCurrent() ? EMessageSide::Me : EMessageSide::You);
            Widget->OnReactionPickerButtonClickedNative.AddUObject(this, &UReactionPickerWidget::OnReactionButtonClicked);
            ButtonsPanel->AddChild(Widget);
        }
    }
}

void UReactionPickerWidget::OnTheme()
{
    if (Border)
    {
        Border->SetBrushColor(Theme->GetPaletteColor(Theme->ReactionPickerBackgroundColor));
    }
}

void UReactionPickerWidget::OnReactionButtonClicked(const FName& ReactionType)
{
    if (ensure(Channel))
    {
        const TOptional<FReaction> OwnReaction = Message.Reactions.GetOwnReaction(ReactionType, UUserManager::Get());
        if (OwnReaction.IsSet())
        {
            Channel->DeleteReaction(Message, OwnReaction.GetValue());
        }
        else
        {
            Channel->SendReaction(Message, ReactionType, false);
        }
    }

    FSlateApplication::Get().DismissMenuByWidget(TakeWidget());
}
