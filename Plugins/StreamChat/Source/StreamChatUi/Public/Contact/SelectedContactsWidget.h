// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Common/IconButton.h"
#include "Components/EditableText.h"
#include "Components/WrapBox.h"
#include "CoreMinimal.h"
#include "SelectedContactWidget.h"
#include "StreamWidget.h"

#include "SelectedContactsWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API USelectedContactsWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream|Selected Contacts")
    void SetUsers(const TArray<FUserRef>& Users);

    UFUNCTION(BlueprintCallable, Category = "Stream|Selected Contacts")
    void AddUser(const FUserRef& User);
    UFUNCTION(BlueprintCallable, Category = "Stream|Selected Contacts")
    void RemoveUser(const FUserRef& User);
    UFUNCTION(BlueprintPure, Category = "Stream|Selected Contacts")
    const TArray<FUserRef>& GetUsers() const;
    UFUNCTION(BlueprintCallable, Category = "Stream|Selected Contacts")
    void SetGroupMode(bool bIsGroupMode);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchTextChanged, const FText&, Text);
    UPROPERTY(BlueprintAssignable)
    FSearchTextChanged OnSearchTextChanged;

protected:
    UPROPERTY(meta = (BindWidget))
    UWrapBox* WrapBox;

    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;

    UPROPERTY(meta = (BindWidget))
    UImage* Divider;

    UPROPERTY(meta = (BindWidget))
    UIconButton* AddUserButton;

    UPROPERTY(meta = (BindWidget))
    UEditableText* SearchText;

    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<USelectedContactWidget> SelectedContactWidgetClass = USelectedContactWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    TArray<FUserRef> Contacts;

private:
    virtual void OnSetup() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void OnAddUserClicked();
    UFUNCTION()
    void OnSearchTextCommit(const FText& Text, ETextCommit::Type Commit);

    void PopulateWrapBox();
    void SetTypingMode(bool bNewTypingMode);

    bool bTypingMode = false;
    FText LastSearchText;
};
