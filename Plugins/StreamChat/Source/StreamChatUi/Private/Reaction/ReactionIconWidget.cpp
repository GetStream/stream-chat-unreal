// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/ReactionIconWidget.h"

void UReactionIconWidget::Setup(const FName& InReactionType, EMessageSide InSide)
{
    ReactionType = InReactionType;
    Side = InSide;

    Super::Setup();
}

void UReactionIconWidget::OnSetup()
{
    if (Image)
    {
        if (UTexture2D** Texture = Textures.Find(ReactionType))
        {
            Image->SetBrushFromTexture(*Texture, true);
        }
        if (bShouldTint)
        {
            if (const FLinearColor* Color = Colors.Find(Side))
            {
                Image->SetColorAndOpacity(*Color);
            }
        }
    }
}
