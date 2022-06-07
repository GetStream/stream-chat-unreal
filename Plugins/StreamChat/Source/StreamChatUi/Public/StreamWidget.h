// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Context/ChannelContextWidget.h"
#include "Context/ClientContextWidget.h"
#include "CoreMinimal.h"

#include "StreamWidget.generated.h"

class UChatChannel;
class UThemeDataAsset;

UCLASS(Abstract, meta = (DisableNativeTick))
class STREAMCHATUI_API UStreamWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /// This must be called in your custom Setup(...) function.
    void Setup();

protected:
    virtual void NativePreConstruct() override;

    /// The chat client context if this widget is below a ClientContextWidget in the hierarchy
    UFUNCTION(BlueprintGetter)
    UClientContextWidget* GetClientContext() const;
    /// The chat client if this widget is below a ClientContextWidget in the hierarchy
    UFUNCTION(BlueprintGetter)
    UStreamChatClientComponent* GetClient() const;
    /// The channel context if this widget is below a such a context in the hierarchy
    UFUNCTION(BlueprintGetter)
    UChannelContextWidget* GetChannelContext() const;
    /// The channel if this widget is below a ChannelContextWidget in the hierarchy
    UFUNCTION(BlueprintGetter)
    UChatChannel* GetChannel() const;
    /// The theme if this widget is below a ThemeContextWidget in the hierarchy
    UFUNCTION(BlueprintGetter)
    UThemeDataAsset* GetTheme() const;

    /// Call the internal OnSetup function automatically. Needed if you don't have a custom Setup function.
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    bool bAutoSetup = false;

private:
    virtual bool Initialize() override;

    /// You should override this to perform all child widget initialization (e.g. text, image, etc + bound widgets).
    /// Widget bindings, defaults and setup properties will be valid here.
    /// Only called once when widget is spawned after Initialize, NOT when added to parent/viewport
    virtual void OnSetup()
    {
    }

    /// The chat client context if this widget is below a ClientContextWidget in the hierarchy and if WantsClient is true.
    UPROPERTY(BlueprintGetter = GetClientContext, Transient, Category = Stream)
    mutable UClientContextWidget* ClientContext;
    /// The chat client if this widget is below a ClientContextWidget in the hierarchy and if WantsClient is true.
    UPROPERTY(BlueprintGetter = GetClient, Transient, Category = Stream)
    mutable UStreamChatClientComponent* Client;
    /// The channel context if this widget is below a such a context in the hierarchy and if WantsChannel is true.
    UPROPERTY(BlueprintGetter = GetChannelContext, Transient, Category = Stream)
    mutable UChannelContextWidget* ChannelContext;
    /// The channel if this widget is below a ChannelContextWidget in the hierarchy and if WantsChannel is true.
    UPROPERTY(BlueprintGetter = GetChannel, Transient, Category = Stream)
    mutable UChatChannel* Channel;
    /// The theme if this widget is below a ThemeContextWidget in the hierarchy and if WantsTheme is true.
    UPROPERTY(BlueprintGetter = GetTheme, Transient, Category = Stream)
    mutable UThemeDataAsset* Theme;
    bool bConstructed = false;
};
