// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "ThemeDataAsset.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UThemeDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    static UThemeDataAsset* Get(const UWidget* Widget);
    /// Should the name of a user appear colored
    UPROPERTY(EditAnywhere, Category = "User")
    bool bColoredName;

    // The bubble color of messages sent by the current user
    UPROPERTY(EditAnywhere, Category = "Message Bubble")
    FLinearColor MeBubbleColor;
    // The bubble color of messages sent by other users
    UPROPERTY(EditAnywhere, Category = "Message Bubble")
    FLinearColor YouBubbleColor;
    // The bubble color of messages which have been soft deleted
    UPROPERTY(EditAnywhere, Category = "Message Bubble")
    FLinearColor DeletedBubbleColor;

    // The color of the text of messages
    UPROPERTY(EditAnywhere, Category = "Message Bubble")
    FLinearColor NormalMessageTextColor;
    // The color of the text of messages which have been soft deleted
    UPROPERTY(EditAnywhere, Category = "Message Bubble")
    FLinearColor DeletedMessageTextColor;
};
