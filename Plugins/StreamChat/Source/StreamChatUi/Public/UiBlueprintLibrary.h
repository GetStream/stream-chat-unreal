// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "Input/Events.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "User/User.h"

#include "UiBlueprintLibrary.generated.h"

class UScrollBox;
class UWidget;

UCLASS()
class STREAMCHATUI_API UUiBlueprintLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|UI")
    static void AddContextMenu(UWidget* Widget, const FPointerEvent& InMouseEvent);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat|UI")
    static void DismissContextMenu(UWidget* Widget);

    UFUNCTION(BlueprintPure, Category = "Stream Chat|UI")
    static FString GetChannelTitle(const UChatChannel* Channel);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat|UI")
    static void GetFirstVisibleChildOfScrollBox(UScrollBox* ScrollBox, int32& OutIndex, float& OutLeadingEdge);

    UFUNCTION(BlueprintPure, Category = "Stream Chat|User", meta = (DisplayName = "Deference User Ref", CompactNodeTitle = "->", BlueprintAutocast))
    static const FUser& DerefUserRef(const FUserRef& User);

    UFUNCTION(BlueprintPure, Category = "Stream Chat|User")
    static bool IsCurrent(const FUserRef& User);

    UFUNCTION(BlueprintPure, Category = "Stream Chat|Messages")
    static TArray<FMessage> FilterRecent(const TArray<FMessage>& Messages, const FTimespan& Since);

    /// Deterministically generate one of 16 preset colors from a string
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Messages")
    static FLinearColor GenerateColorFromString(const FString& Input);

    /** Converts a linear color value to a hex string, in the form 'RRGGBB' (no '#' prefix, no alpha) */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "To Hex String (linear color)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|String")
    static FString Conv_ColorToHexString(const FLinearColor& InColor);
};
