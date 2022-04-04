// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Common/IconButton.h"
#include "Components/Border.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"

#include "GroupNameInputWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UGroupNameInputWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UGroupNameInputWidget();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGroupNameChanged, const FText&, Text);
    UPROPERTY(BlueprintAssignable)
    FGroupNameChanged OnGroupNameChanged;

    UFUNCTION(BlueprintPure, Category = "Stream|Group Name Input")
    FText GetGroupName() const;

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;

    UPROPERTY(meta = (BindWidget))
    UImage* Divider;

    UPROPERTY(meta = (BindWidget))
    UIconButton* ClearButton;

    UPROPERTY(meta = (BindWidget))
    UEditableText* GroupName;

private:
    virtual void OnSetup() override;
    virtual void OnTheme() override;

    UFUNCTION()
    void OnClearClicked();
    UFUNCTION()
    void OnGroupNameChange(const FText& Text);

    FText LastText;
};
