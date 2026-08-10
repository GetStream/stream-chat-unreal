// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Common/SPaginateListWidget.h"
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
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;

    UWidget* CreateMessageWidget(const FMessageRef&);
    void Paginate(const EPaginationDirection PaginationDirection, const TFunction<void()> Callback);

    UFUNCTION()
    void OnMessagesUpdated();
    UFUNCTION()
    void OnRepliesUpdated(const FString& ParentMessageId);
    UFUNCTION()
    void OnThreadChanged(bool bThreadOpen, const FMessage& ParentMessage);
    UFUNCTION()
    void ScrollToBottom(const FMessage& Message);

    EMessagePosition GetPosition(const FMessage& Message) const;

    /// Collect what should be on screen: the channel's messages, or an open thread's parent and replies
    void RefreshItems();
    bool IsThreadOpen() const;

    TSharedPtr<SPaginateListWidget<FMessageRef>> PaginateListWidget;

    /**
     * What the list is showing.
     *
     * The list view holds this by pointer for its lifetime, so it has to be one array that outlives
     * every refresh. Pointing straight at the message store is not an option for a thread: its
     * replies live in a map, whose values move when it grows.
     */
    FMessages Items;

    /// ID of the parent message of the open thread, empty when showing the channel
    FString ThreadParentId;
};
