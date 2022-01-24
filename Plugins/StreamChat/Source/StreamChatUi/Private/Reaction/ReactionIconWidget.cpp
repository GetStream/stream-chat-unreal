// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/ReactionIconWidget.h"

void UReactionIconWidget::Setup(const FName& InReactionType, EMessageSide InSide)
{
    ReactionType = InReactionType;
    Side = InSide;

    Super::Setup();
}
