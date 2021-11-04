#include "Header/MessageListHeaderWidget.h"

#include "Context/ChannelContextWidget.h"
#include "UiBlueprintLibrary.h"

void UMessageListHeaderWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UMessageListHeaderWidget::NativeConstruct()
{
    if (const UChatChannel* Channel = UChannelContextWidget::GetChannel(this))
    {
        const FText Title = UUiBlueprintLibrary::GetChannelTitle(Channel);
        if (Header)
        {
            Header->SetTitle(Title);
        }

        if (Avatar)
        {
            // TODO Group chat
            const auto OthersPredicate = [&](const FMember& M)
            {
                return M.User.Id != Channel->CurrentUser.Id;
            };
            if (const FMember* Member = Channel->State.Members.FindByPredicate(OthersPredicate))
            {
                Avatar->Setup(Member->User);
            }
        }
    }

    Super::NativeConstruct();
}
