// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/FontReactionIconWidget.h"

void UFontReactionIconWidget::OnSetup()
{
    if (TextBlock)
    {
        if (const FString* String = Emoji.Find(ReactionType))
        {
            TextBlock->SetText(FText::FromString(*String));
        }
    }
}
