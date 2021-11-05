#include "Header/MessageListHeaderWidget.h"

#include "Algo/Transform.h"
#include "Context/ChannelContextWidget.h"
#include "UiBlueprintLibrary.h"

void UMessageListHeaderWidget::NativeConstruct()
{
    Channel = UChannelContextWidget::GetChannel(this);
    Channel->OnTypingIndicator.AddDynamic(this, &UMessageListHeaderWidget::OnTypingIndicator);

    const FText Title = UUiBlueprintLibrary::GetChannelTitle(Channel);
    if (Header)
    {
        Header->SetTitle(Title);
        ShowOnlineStatusSubheader();
    }

    if (Avatar)
    {
        // TODO Group chat
        const auto NotCurrentUser = [&](const FMember& M)
        {
            return M.User.Id != Channel->CurrentUser.Id;
        };
        constexpr auto ToUser = [](const FMember& M)
        {
            return M.User;
        };
        TArray<FUser> Others;
        Algo::TransformIf(Channel->State.Members, Others, NotCurrentUser, ToUser);
        {
            Avatar->Setup(Others);
        }
    }

    Super::NativeConstruct();
}

void UMessageListHeaderWidget::NativeDestruct()
{
    Channel->OnTypingIndicator.RemoveDynamic(this, &UMessageListHeaderWidget::OnTypingIndicator);
    Super::NativeDestruct();
}

void UMessageListHeaderWidget::OnTypingIndicator(const ETypingIndicatorState TypingState, const FUser& User)
{
    if (User.Id == Channel->CurrentUser.Id)
    {
        return;
    }

    if (TypingState == ETypingIndicatorState::StartTyping)
    {
        if (!TypingIndicator)
        {
            TypingIndicator = CreateWidget<UTypingIndicatorWidget>(this, TypingIndicatorWidgetClass);
            Header->SetSubtitleContent(TypingIndicator);
        }
        TypingIndicator->AddUser(User);
    }
    else if (TypingState == ETypingIndicatorState::StopTyping)
    {
        if (TypingIndicator)
        {
            const bool bIsEmpty = TypingIndicator->RemoveUser(User);
            if (bIsEmpty)
            {
                ShowOnlineStatusSubheader();
            }
        }
    }
}

void UMessageListHeaderWidget::ShowOnlineStatusSubheader()
{
    UOnlineStatusSubheaderWidget* Widget = CreateWidget<UOnlineStatusSubheaderWidget>(this, StatusWidgetClass);
    Header->SetSubtitleContent(Widget);
    TypingIndicator = nullptr;
}
