// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
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
    /// Called with the current chat channel when it's available. You need to enable WantsChannel for this to be fired.
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Channel"))
    void OnChannel_BP(UChatChannel* Theme);

    /// Should the OnTheme function be called with the current theme, when it is available?
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    bool bWantsTheme = false;
    /// Should the OnChannel function be called with the current channel, when it is available?
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    bool bWantsChannel = false;

    /// The channel if this widget is below a ChannelContextWidget in the hierarchy and if WantsChannel is true.
    UPROPERTY(BlueprintReadOnly, Transient, Category = Stream)
    UChatChannel* Channel;

private:
    virtual bool Initialize() override;

    // Don't allow (pre)construction. This should all be done in OnSetup()
    virtual void NativeConstruct() override final;

    virtual void NativePreConstruct() override final;

    /// You should override this to perform all child widget initialization.
    /// Widget bindings, defaults and setup properties will be valid here.
    /// Only called once when widget is spawned, NOT when added to parent/viewport
    virtual void OnSetup()
    {
    }

    virtual bool WantsTheme()
    {
        return bWantsTheme;
    }
    virtual bool WantsChannel()
    {
        return bWantsChannel;
    }

    /// Called with the current theme when it's available
    virtual void OnTheme(const UThemeDataAsset*)
    {
    }
    /// Called with the current chat channel when it's available
    virtual void OnChannel(UChatChannel*)
    {
    }
};
