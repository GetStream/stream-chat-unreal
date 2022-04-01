// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Contact/SelectedContactsWidget.h"
#include "CoreMinimal.h"
#include "StreamWidget.h"
#include "User/UserListWidget.h"

#include "NewChatWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UNewChatWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UNewChatWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup();

protected:
    UPROPERTY(meta = (BindWidget))
    USelectedContactsWidget* SelectedContacts;
    UPROPERTY(meta = (BindWidget))
    UUserListWidget* UserList;

private:
    virtual void OnSetup() override;
    virtual void OnTheme() override;

    UFUNCTION()
    void OnUserClicked(const FUserRef& User, bool bSelected);
    UFUNCTION()
    void OnSearchTextChanged(const FText& Text);
};
