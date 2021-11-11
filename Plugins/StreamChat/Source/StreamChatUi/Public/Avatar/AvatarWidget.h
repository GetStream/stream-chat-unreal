// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "Components/RetainerBox.h"
#include "Components/SizeBox.h"
#include "CoreMinimal.h"
#include "ProfilePicWidget.h"
#include "StreamUserWidget.h"
#include "User/User.h"
#include "User/UserRef.h"

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
    void Setup(const TArray<FUserRef>& InUsers, int32 InSize = 80);

protected:
    UPROPERTY(meta = (BindWidget))
    UGridPanel* Grid;

    UPROPERTY(meta = (BindWidget))
    USizeBox* SizeBox;

    UPROPERTY(meta = (BindWidget))
    URetainerBox* RetainerBox;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UProfilePicWidget> ProfilePicWidgetClass = UProfilePicWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    UMaterialInterface* EffectMaterial;

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    FName OnlineStatusMaterialParameterName = TEXT("bOnline");

private:
    virtual void OnSetup() override;
    void UpdateOnlineStatus(bool bOnline);
    void CreateProfilePics();
    UProfilePicWidget* CreateProfilePic(const FUserRef&);

    UFUNCTION()
    void OnUserUpdated();

    UPROPERTY(EditAnywhere, Category = Setup)
    TArray<FUserRef> Users;

    UPROPERTY(EditAnywhere, Category = Setup)
    int32 Size = 80;

    UPROPERTY(Transient)
    UMaterialInstanceDynamic* EffectMaterialDynamic;
};
