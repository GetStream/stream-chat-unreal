// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Context/ChannelContextWidget.h"
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
    /// Called with the current theme when it's available. You need to enable WantsTheme for this to be fired.
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Theme"))
    void OnTheme_BP(const UThemeDataAsset* Theme);
    /// Called with the chat client when it's available. You need to enable WantsClient for this to be fired.
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Client"))
    void OnClient_BP();
    /// Called with the current chat channel when it's available. You need to enable WantsChannel for this to be fired.
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Channel"))
    void OnChannel_BP();

    /// Should the OnTheme function be called with the current theme, when it is available?
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    bool bWantsTheme = false;
    /// Should the OnClient function be called with the current chat client, when it is available?
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    bool bWantsClient = false;
    /// Should the OnChannel function be called with the current channel, when it is available?
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    bool bWantsChannel = false;
    /// Call the internal OnSetup function automatically. Needed if you don't have a custom Setup function.
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    bool bAutoSetup = false;

    /// The chat client if this widget is below a ClientContextWidget in the hierarchy and if WantsClient is true.
    UPROPERTY(BlueprintReadOnly, Transient, Category = Stream)
    UStreamChatClientComponent* Client;
    /// The channel context if this widget is below a such a context in the hierarchy and if WantsChannel is true.
    UPROPERTY(BlueprintReadOnly, Transient, Category = Stream)
    UChannelContextWidget* ChannelContext;
    /// The channel if this widget is below a ChannelContextWidget in the hierarchy and if WantsChannel is true.
    UPROPERTY(BlueprintReadOnly, Transient, Category = Stream)
    UChatChannel* Channel;

private:
    virtual bool Initialize() override;

    // Don't allow (pre)construction. This should all be done in OnSetup()
    virtual void NativeConstruct() override final;

    virtual void NativePreConstruct() override final;

    /// You should override this to perform all child widget initialization (e.g. text, image, etc + bound widgets).
    /// Widget bindings, defaults and setup properties will be valid here.
    /// Only called once when widget is spawned, NOT when added to parent/viewport
    virtual void OnSetup()
    {
    }

    /// Called with the current theme when it's available
    virtual void OnTheme(const UThemeDataAsset*)
    {
    }
    /// Called when the chat client is available
    virtual void OnClient()
    {
    }
    /// Called when the current chat channel is available
    virtual void OnChannel()
    {
    }

    UThemeDataAsset* DefaultTheme;
};
