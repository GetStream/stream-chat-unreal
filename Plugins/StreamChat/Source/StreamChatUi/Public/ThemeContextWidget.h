// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ContextWidget.h"
#include "CoreMinimal.h"

#include "ThemeContextWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UThemeContextWidget final : public UContextWidget
{
    GENERATED_BODY()

public:
    static FLinearColor GetTheme(UWidget* Widget);
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat")
    FLinearColor Theme;
};