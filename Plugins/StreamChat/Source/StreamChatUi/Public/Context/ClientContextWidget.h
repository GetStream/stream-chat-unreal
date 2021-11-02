// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StreamChatClientComponent.h"
#include "ContextWidget.h"
#include "CoreMinimal.h"

#include "ClientContextWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UClientContextWidget final : public UContextWidget
{
    GENERATED_BODY()

public:
    static UStreamChatClientComponent* GetClient(UWidget* Widget);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    UStreamChatClientComponent* Client;
};
