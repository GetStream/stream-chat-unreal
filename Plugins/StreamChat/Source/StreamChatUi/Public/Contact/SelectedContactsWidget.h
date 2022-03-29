// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

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
    USelectedContactsWidget();
    UFUNCTION(BlueprintCallable, Category = "Selected Contacts")
    void SetUsers(const TArray<FUserRef>& Users);

    UFUNCTION(BlueprintCallable, Category = "Selected Contacts")
    void AddUser(const FUserRef& User);
    UFUNCTION(BlueprintCallable, Category = "Selected Contacts")
    void RemoveUser(const FUserRef& User);

protected:
    UPROPERTY(meta = (BindWidget))
    UWrapBox* WrapBox;

    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<USelectedContactWidget> SelectedContactWidgetClass = USelectedContactWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    TArray<FUserRef> Contacts;

private:
    virtual void OnSetup() override;
    virtual void OnTheme() override;
    void PopulateWrapBox();
};
