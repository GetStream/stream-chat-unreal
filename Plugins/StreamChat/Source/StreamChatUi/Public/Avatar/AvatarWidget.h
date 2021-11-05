// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "ProfilePicWidget.h"
#include "StreamUserWidget.h"
#include "User.h"

#include "AvatarWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UAvatarWidget : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const TArray<FUser>& InUsers);

protected:
    UPROPERTY(meta = (BindWidget))
    UGridPanel* Grid;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UProfilePicWidget> ProfilePicWidgetClass = UProfilePicWidget::StaticClass();

private:
    virtual void OnSetup() override;
    void CreateProfilePics();
    UProfilePicWidget* CreateProfilePic(const FUser&);

    UPROPERTY(EditAnywhere, Category = Setup)
    TArray<FUser> Users;
};
