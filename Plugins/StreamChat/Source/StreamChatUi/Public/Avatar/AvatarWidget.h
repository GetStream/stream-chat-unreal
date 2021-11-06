// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
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
    void Setup(const TArray<FUser>& InUsers, int32 InSize = 40);

protected:
    UPROPERTY(meta = (BindWidget))
    UGridPanel* Grid;

    UPROPERTY(meta = (BindWidget))
    USizeBox* SizeBox;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UProfilePicWidget> ProfilePicWidgetClass = UProfilePicWidget::StaticClass();

private:
    virtual void OnSetup() override;
    void CreateProfilePics();
    UProfilePicWidget* CreateProfilePic(const FUser&);

    UPROPERTY(EditAnywhere, Category = Setup)
    TArray<FUser> Users;

    UPROPERTY(EditAnywhere, Category = Setup)
    int32 Size = 40;
};
