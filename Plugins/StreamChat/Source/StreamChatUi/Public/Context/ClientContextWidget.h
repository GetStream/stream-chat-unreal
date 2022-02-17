// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ContextWidget.h"
#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"

#include "ClientContextWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UClientContextWidget final : public UContextWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(UStreamChatClientComponent* InClient);
    static UClientContextWidget* Get(const UWidget* Widget);
    static UStreamChatClientComponent* GetClient(const UWidget* Widget);
    UStreamChatClientComponent* GetClient() const;

    // TODO A back stack container widget might one day make sense
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBackDelegate);
    UPROPERTY(BlueprintAssignable)
    FBackDelegate OnBack;

private:
    UPROPERTY(Transient)
    UStreamChatClientComponent* Client;
};
