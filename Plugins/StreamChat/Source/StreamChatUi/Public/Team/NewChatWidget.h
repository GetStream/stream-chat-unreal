// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Contact/SelectedContactsWidget.h"
#include "CoreMinimal.h"
#include "GroupNameInputWidget.h"
#include "Input/MessageComposerWidget.h"
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
    UHeaderWidget* Header;
    UPROPERTY(meta = (BindWidget))
    UGroupNameInputWidget* GroupName;
    UPROPERTY(meta = (BindWidget))
    USelectedContactsWidget* SelectedContacts;
    UPROPERTY(meta = (BindWidget))
    UUserListWidget* UserList;
    UPROPERTY(meta = (BindWidget))
    UMessageComposerWidget* Composer;

private:
    virtual void OnSetup() override;

    UFUNCTION()
    void OnUserClicked(const FUserRef& User, bool bSelected);
    UFUNCTION()
    void OnGroupNameChanged(const FText& Text);
    UFUNCTION()
    void OnSearchTextChanged(const FText& Text);
    UFUNCTION()
    void OnSendMessage(const FString& Text);
};
