// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/PanelWidget.h"
#include "CoreMinimal.h"
#include "Message/MessageSide.h"
#include "ReactionIconWidget.h"
#include "StreamUserWidget.h"

#include "MessageReactionsWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageReactionsWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EMessageSide InSide);

protected:
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* ReactionsPanel;

    UPROPERTY(EditAnywhere, Category = Defaults)
    TSubclassOf<UReactionIconWidget> ReactionWidgetClass;

private:
    virtual void OnSetup() override;

    UPROPERTY(EditAnywhere, Category = Setup)
    FMessage Message;

    UPROPERTY(EditAnywhere, Category = Setup)
    EMessageSide Side;
};
