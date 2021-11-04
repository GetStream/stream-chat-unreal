// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReactionsBubbleWidget.h"
#include "StreamUserWidget.h"
#include "Channel/Message.h"
#include "Message/MessageSide.h"

#include "MessageReactionsWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageReactionsWidget : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EMessageSide InSide);

protected:
    UPROPERTY(meta = (BindWidget))
    UPanelSlot* BubbleWidgetPanel;

    UPROPERTY(EditAnywhere, Category = Defaults)
    TSubclassOf<UReactionsBubbleWidget> MeSingleReactionBubbleWidgetClass;
    UPROPERTY(EditAnywhere, Category = Defaults)
    TSubclassOf<UReactionsBubbleWidget> MeMultipleReactionsBubbleWidgetClass;
    UPROPERTY(EditAnywhere, Category = Defaults)
    TSubclassOf<UReactionsBubbleWidget> YouSingleReactionBubbleWidgetClass;
    UPROPERTY(EditAnywhere, Category = Defaults)
    TSubclassOf<UReactionsBubbleWidget> YouMultipleReactionsBubbleWidgetClass;

private:
    virtual void OnSetup() override;

     TSubclassOf<UReactionsBubbleWidget> GetReactionsBubbleWidgetClass() const;

    UPROPERTY(EditAnywhere, Category = Setup)
    FMessage Message;

    UPROPERTY(EditAnywhere, Category = Setup)
    EMessageSide Side;
};
