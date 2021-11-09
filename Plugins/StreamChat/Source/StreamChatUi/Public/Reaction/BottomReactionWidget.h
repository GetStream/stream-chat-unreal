// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "Message/MessageSide.h"
#include "Reaction/ReactionGroup.h"
#include "ReactionIconWidget.h"
#include "StreamUserWidget.h"

#include "BottomReactionWidget.generated.h"

/**
 * https://www.figma.com/file/oeHiSMDoutuVbJ9WiVyhcG/Chat-Design-Kit-1.5-Android---Work-in-Progress?node-id=17089%3A113943
 */
UCLASS()
class STREAMCHATUI_API UBottomReactionWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FReactionGroup& InReactionGroup);

protected:
    UPROPERTY(meta = (BindWidget))
    UReactionIconWidget* ReactionIcon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ReactionCountTextBlock;

private:
    virtual void OnSetup() override;

    EMessageSide GetSide() const;

    UPROPERTY(EditAnywhere, Category = Setup)
    FReactionGroup ReactionGroup;
};
