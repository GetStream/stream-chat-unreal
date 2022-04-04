// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Common/Html/HtmlElementStyle.h"
#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "ThemeDataAsset.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UThemeDataAsset final : public UDataAsset
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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Avatar")
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
    /// The color of the text of messages which have been soft deleted
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Bubble")
    FName DeletedMessageTextColor = TEXT("text-low-emphasis");
    /// The text style information for message bubbles
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Styles|Message Bubble")
    FHtmlElementStyles BubbleHtmlStyles;

    /// The color of the background of the message composer
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Composer")
    FName MessageComposerBackgroundColor = TEXT("bars-bg");
    /// The color of the borders in the message composer
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Message Composer")
    FName MessageComposerBorderColor = TEXT("borders");
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

    // The background color of a highlight button when hovered
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Back Button")
    FName BackButtonSelectedColor = TEXT("input-bg");

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

    /// The background color of bottom reactions including the current user
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Bottom Reactions")
    FName MeBottomReactionColor = TEXT("borders");
    /// The background color of bottom reactions not including the current user
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Bottom Reactions")
    FName YouBottomReactionColor = TEXT("bars-bg");
    /// The border color of any bottom reaction
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Bottom Reactions")
    FName BottomReactionBorderColor = TEXT("borders");
    /// The text color of any bottom reaction
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Bottom Reactions")
    FName BottomReactionTextColor = TEXT("text-high-emphasis");

    /// The background color of the reactions tooltip
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Reactions Tooltip")
    FName ReactionsTooltipBackgroundColor = TEXT("bars-bg");
    /// The background color of the highlighted area surrounding the reaction icon in the reactions tooltip
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Reactions Tooltip")
    FName ReactionsTooltipIconHighlightColor = TEXT("link-bg");

    /// The background color of the default header widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Header")
    FName HeaderBackgroundColor = TEXT("bars-bg");

    /// The background color of the default header widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Header")
    FName HeaderTitleTextColor = TEXT("text-high-emphasis");

    /// The background color of the message list container
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Team Chat")
    FName MessageListContainerBackgroundColor = TEXT("app-bg");
    /// The background color of the channel list container
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Team Chat")
    FName ChannelListContainerBackgroundColor = TEXT("input-bg");
    /// The color of the dividers in the team chat layout
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Team Chat")
    FName TeamChatDividerColor = TEXT("borders");

    /// The color of the divider between the header and messages in the channel widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Channel")
    FName ChannelHeaderDividerColor = TEXT("borders");

    /// The normal background color of a header button
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Header")
    FName HeaderButtonNormalBackgroundColor = TEXT("bars-bg");
    /// The hovered background color of a header button
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Header")
    FName HeaderButtonHoveredBackgroundColor = TEXT("input-bg");
    /// The selected background color of a header button
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Header")
    FName HeaderButtonSelectedBackgroundColor = TEXT("borders");
    /// The color of the title text of the channel status widget normally
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Channel Status")
    FName ChannelStatusTitleTextColor = TEXT("text-high-emphasis");
    /// The color of the title text of the channel status widget when muted
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Channel Status")
    FName ChannelStatusMutedTitleTextColor = TEXT("text-low-emphasis");
    /// The color of the recent message text
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Channel Status")
    FName ChannelStatusRecentMessageTextColor = TEXT("text-low-emphasis");
    /// The color of the divider between each channel status widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Channel Status")
    FName ChannelStatusDividerColor = TEXT("borders");

    /// The color of the top left icon in the channel list header
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Channel List Header")
    FName ChannelListHeaderIconColor = TEXT("text-high-emphasis");

    /// The color of the background of a selected contact "pill"
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Selected Contact")
    FName SelectedContactBackgroundColor = TEXT("borders");

    /// The color of the divider below the selected contacts widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Selected Contacts")
    FName SelectedContactsDividerColor = TEXT("borders");
    /// The color of the label text in the selected contacts widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Selected Contacts")
    FName SelectedContactsLabelTextColor = TEXT("text-low-emphasis");
    /// The background color of the selected contacts widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Selected Contacts")
    FName SelectedContactsBackgroundColor = TEXT("bars-bg");
    /// The search text color of the selected contacts widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Selected Contacts")
    FName SelectedContactsInputTextColor = TEXT("text-high-emphasis");

    /// The color of the text in a section header widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Section Heading")
    FName SectionHeadingTextColor = TEXT("text-low-emphasis");
    /// The color of the background in a section header widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Section Heading")
    FName SectionHeadingBackgroundColor = TEXT("input-bg");

    /// The text color of a status subtitle
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colors|Section Heading")
    FName StatusSubtitleTextColor = TEXT("text-low-emphasis");
};
