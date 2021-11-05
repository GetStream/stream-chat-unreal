// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "User.h"

#include "TypingIndicatorWidget.generated.h"

/**
 * Displays typing status for multiple users.
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
    void AddUser(const FUser& User);
    /// Returns true if no more users exist after removing user
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    bool RemoveUser(const FUser& User);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;

private:
    void UpdateLabel() const;
    FText GetLabel() const;

    UPROPERTY(EditAnywhere, Category = Setup)
    TArray<FUser> Users;
};
