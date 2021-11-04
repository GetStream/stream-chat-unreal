#include "Header/MessageListHeaderWidget.h"

#include "Context/ChannelContextWidget.h"
#include "UiBlueprintLibrary.h"

void UMessageListHeaderWidget::NativeConstruct()
{
    if (const UChatChannel* Channel = UChannelContextWidget::GetChannel(this))
    {
        const FText Title = UUiBlueprintLibrary::GetChannelTitle(Channel);
        Header->SetTitle(Title);
    }

    Super::NativeConstruct();
}
