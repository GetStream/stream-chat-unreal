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
    UFUNCTION(BlueprintPure, Category = "Theme")
    const FLinearColor& GetPaletteColor(const FName& Name) const;

    static UThemeDataAsset* Get(const UWidget* Widget);
    /// Should the name of a user appear colored
    UPROPERTY(EditAnywhere, Category = "User")
    bool bColoredName;

    /// All colors should be named here, for referencing elsewhere
    UPROPERTY(EditAnywhere, Category = Colors)
    TMap<FName, FLinearColor> Palette;

    /// The bubble color of messages sent by the current user
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Bubble")
    FName MeBubbleColor = TEXT("borders");
    /// The bubble color of messages sent by other users
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Bubble")
    FName YouBubbleColor = TEXT("bars-bg");
    /// The bubble color of messages which have been soft deleted
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Bubble")
    FName DeletedBubbleColor = TEXT("input-bg");

    /// The color of the text of messages
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Bubble")
    FName NormalMessageTextColor = TEXT("text-high-emphasis");
    /// The color of the text of messages which have been soft deleted
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Bubble")
    FName DeletedMessageTextColor = TEXT("text-low-emphasis");

    /// The color of the background of the message composer
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Composer")
    FName MessageComposerBackgroundColor = TEXT("bars-bg");
    /// The color of the header text (e.g. "Edit Message" or "Reply to Message")
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Composer")
    FName MessageComposerHeaderTextColor = TEXT("text-high-emphasis");

    /// The color of the input text
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Input")
    FName MessageInputTextColor = TEXT("text-high-emphasis");
    /// The color of the input box background
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Input")
    FName MessageInputBackgroundColor = TEXT("app-bg");
    /// The color of the input box border
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Input")
    FName MessageInputBorderColor = TEXT("borders");

    /// The enabled color for the background of icon buttons
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Icon Button")
    FName IconButtonEnabledBackgroundColor = TEXT("accent-primary");
    /// The disabled color for the background of icon buttons
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Icon Button")
    FName IconButtonDisabledBackgroundColor = TEXT("disabled");

    /// The color of the timestamp text under a message stack
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Timestamp")
    FName TimestampTextColor = TEXT("text-low-emphasis");
};
