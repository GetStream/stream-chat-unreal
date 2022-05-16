// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Common/PaginateListWidget.h"
#include "Components/NativeWidgetHost.h"
#include "CoreMinimal.h"
#include "MessagePosition.h"
#include "MessageSide.h"
#include "MessageWidget.h"
#include "StreamWidget.h"

#include "MessageListWidget.generated.h"

class UChannelContextWidget;

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageListWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UMessageListWidget();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPaginatingMessagesDelegate, EPaginationDirection, Direction, EHttpRequestState, RequestState);
    /// Fired during various stages of message pagination
    UPROPERTY(BlueprintAssignable)
    FPaginatingMessagesDelegate OnPaginatingMessages;

protected:
    virtual void OnChannel() override;
    virtual void NativeDestruct() override;

    void Paginate(const EPaginationDirection PaginationDirection, const TFunction<void()> Callback);

    UPROPERTY(meta = (BindWidget))
    UNativeWidgetHost* ListView;

    /// Number of additional messages to be queried when paginating
    UPROPERTY(EditAnywhere, Category = Pagination)
    int32 Limit = 20;

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    TSubclassOf<UMessageWidget> MessageWidgetClass;

    DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(UMessageWidget*, FGetMessageWidget, const FMessage&, Message, EMessageSide, Side, EMessagePosition, Position);
    UPROPERTY(EditAnywhere, Category = Events)
    FGetMessageWidget OnGetMessageWidgetEvent;

private:
    UWidget* CreateMessageWidget(const FMessageRef&);

    UFUNCTION()
    void OnMessagesUpdated();
    UFUNCTION()
    void ScrollToBottom(const FMessage& Message);

    EMessagePosition GetPosition(const FMessage& Message) const;

    TSharedPtr<SPaginateListWidget<FMessageRef>> PaginateListWidget;
};
