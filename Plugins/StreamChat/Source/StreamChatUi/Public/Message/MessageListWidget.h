// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Common/PaginateScrollWidget.h"
#include "CoreMinimal.h"
#include "MessagePosition.h"
#include "MessageSide.h"
#include "MessageWidget.h"

#include "MessageListWidget.generated.h"

class UChannelContextWidget;

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageListWidget final : public UPaginateScrollWidget
{
    GENERATED_BODY()

public:
    UMessageListWidget();

protected:
    virtual void OnChannel() override;
    virtual void NativeDestruct() override;
    virtual void Paginate(const EPaginationDirection Direction, const TFunction<void()> Callback) override;

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void CreateMessageWidgets();

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    TSubclassOf<UMessageWidget> MessageWidgetClass;

    DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(UMessageWidget*, FGetMessageWidget, const FMessage&, Message, EMessageSide, Side, EMessagePosition, Position);
    UPROPERTY(EditAnywhere, Category = Events)
    FGetMessageWidget OnGetMessageWidgetEvent;

private:
    UMessageWidget* CreateMessageWidget(const FMessage&, EMessageSide, EMessagePosition);

    UFUNCTION()
    void OnMessagesUpdated();
    UFUNCTION()
    void ScrollToBottom(const FMessage& Message);
};
