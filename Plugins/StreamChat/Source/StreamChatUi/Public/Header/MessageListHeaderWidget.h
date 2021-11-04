// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeaderWidget.h"
#include "MessageListHeaderWidget.generated.h"

class UChatChannel;

/**
 *
 * https://www.figma.com/file/ekifwChR9tR7zRJg1QEzSM/Chat-Design-Kit---Current-version?node-id=15271%3A237867
 */
UCLASS()
class STREAMCHATUI_API UMessageListHeaderWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UHeaderWidget* Header;
};
