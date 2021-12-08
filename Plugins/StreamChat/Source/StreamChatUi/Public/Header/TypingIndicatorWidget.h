// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "User/UserRef.h"

#include "TypingIndicatorWidget.generated.h"

/**
 * @brief Displays typing status for multiple users.
 *
 * Does not care about the users you add, so you most likely shouldn't add the currently logged in user.
 * Does not have a "no one typing" state, so you should hide/show/create/destroy this widget when typists are > 0
 */
UCLASS()
class STREAMCHATUI_API UTypingIndicatorWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void AddUser(const FUserRef& User);
    /// Returns true if no more users exist after removing user
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    bool RemoveUser(const FUserRef& User);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    TArray<FUserRef> Users;

private:
    void UpdateLabel() const;
    FText GetLabel() const;
};
