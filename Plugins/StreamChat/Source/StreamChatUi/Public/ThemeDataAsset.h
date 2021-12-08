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

    /// The default color of the text on an named avatar. Note that this will be ignored if "Colored Name" is true
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Bubble")
    FName NamedAvatarTextColor = TEXT("text-high-emphasis");

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

    /// The color of the border between context menu items
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Context Menu")
    FName ContextMenuBorderColor = TEXT("borders");
    /// The color of a context menu item button in the normal state
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Context Menu")
    FName ContextMenuDefaultButtonColor = TEXT("bars-bg");
    /// The color of a context menu item button in the pressed state
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Context Menu")
    FName ContextMenuPressedButtonColor = TEXT("input-bg");
    /// The color of the text on context menu items
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Context Menu")
    FName ContextMenuDefaultTextColor = TEXT("text-high-emphasis");
    /// The color of the text on "negative" context menu items
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Context Menu")
    FName ContextMenuNegativeTextColor = TEXT("accent-error");
    /// The color of the icon on context menu items
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Context Menu")
    FName ContextMenuDefaultIconColor = TEXT("text-low-emphasis");
    /// The color of the icon on "negative" context menu items
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Context Menu")
    FName ContextMenuNegativeIconColor = TEXT("accent-error");

    // The background color of the reaction picker
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Reaction Picker")
    FName ReactionPickerBackgroundColor = TEXT("bars-bg");
    // The background color of a button in the reaction picker when hovered/clicked
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Reaction Picker")
    FName ReactionPickerSelectedColor = TEXT("input-bg");
};
