// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelStatusWidget.h"

#include "Algo/Transform.h"
#include "UiBlueprintLibrary.h"
#include "WidgetUtil.h"

void UChannelStatusWidget::Setup(UChatChannel* InChannel)
{
    Channel = InChannel;

    Super::Setup();
}

void UChannelStatusWidget::UpdateSelection(UChatChannel* SelectedChannel) const
{
    if (!Button)
    {
        return;
    }
    if (SelectedChannel->State.Cid == Channel->State.Cid)
    {
        Button->SetStyle(SelectedStyle);
    }
    else
    {
        Button->SetStyle(NormalStyle);
    }
}

void UChannelStatusWidget::OnSetup()
{
    if (!Channel)
    {
        return;
    }

    Channel->MessagesUpdated.AddDynamic(this, &UChannelStatusWidget::OnMessagesUpdated);

    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UChannelStatusWidget::OnButtonClicked);
    }

    if (TitleTextBlock)
    {
        const FText Title = UUiBlueprintLibrary::GetChannelTitle(Channel);
        TitleTextBlock->SetText(Title);
    }

    if (Avatar)
    {
        TArray<FUserRef> Users;
        Algo::Transform(Channel->State.Members, Users, [](const FMember& M) { return M.User; });
        Avatar->Setup(Users);
    }

    UpdateDynamic();
}

void UChannelStatusWidget::UpdateDynamic() const
{
    if (!Channel)
    {
        return;
    }

    if (RecentMessageTextBlock)
    {
        const FText Text = FText::FromString(Channel->State.Messages.Last().Text);
        RecentMessageTextBlock->SetText(Text);
    }

    if (Timestamp)
    {
        Timestamp->Setup(Channel->State.Messages.Last(), false, true);
    }
}

void UChannelStatusWidget::OnMessagesUpdated(const TArray<FMessage>&)
{
    UpdateDynamic();
}

void UChannelStatusWidget::OnButtonClicked()
{
    OnChannelStatusButtonClickedNative.Broadcast(Channel);
    OnChannelStatusButtonClicked.Broadcast(Channel);
}
