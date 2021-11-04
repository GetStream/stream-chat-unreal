// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
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
    void Setup(const FUser& InUser);

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* Image;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* InitialsTextBlock;

private:
    virtual void OnSetup() override;

    static FLinearColor ChooseColorForString(const FString&);

    UPROPERTY(EditAnywhere, Category = Setup)
    FUser User;

    UPROPERTY(EditAnywhere, Category = Setup)
    UMaterialInterface* Material;

    UPROPERTY(EditAnywhere, Category = Setup)
    FName MaterialTextureParameterName = TEXT("Texture");
};
