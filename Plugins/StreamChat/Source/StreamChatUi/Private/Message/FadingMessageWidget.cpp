#include "Message/FadingMessageWidget.h"

#include "TimerManager.h"
#include "User/User.h"

void UFadingMessageWidget::Setup(const FMessage& InMessage, const FTimespan& InLifetime)
{
    Message = InMessage;
    if (InLifetime.GetTicks() != 0)
    {
        Lifetime = InLifetime;
    }

    Super::Setup();
}

void UFadingMessageWidget::OnSetup()
{
    if (TextBlock)
    {
        if (Message.User.IsValid() && !Message.User->Name.IsEmpty())
        {
            TextBlock->SetText(FText::FromString(FString::Printf(TEXT("%s: %s"), *Message.User->Name, *Message.Text)));
        }
        else
        {
            TextBlock->SetText(FText::FromString(Message.Text));
        }
    }

    if (FadeAnimation)
    {
        const FTimespan MessageAge = FDateTime::UtcNow() - Message.CreatedAt;
        const FTimespan ShowFor = Lifetime - MessageAge;
        const double ShowForSeconds = ShowFor.GetTotalSeconds();
        if (!ensureMsgf(ShowForSeconds > 0., TEXT("Trying to show a fading message which is older than its lifetime")))
        {
            RemoveFromParent();
            return;
        }

        if (ShowForSeconds < FadeAnimation->GetEndTime())
        {
            // Only play the remaining fade
            PlayAnimation(FadeAnimation, FadeAnimation->GetEndTime() - ShowForSeconds);
            return;
        }

        const double Delay = FMath::Max(0., ShowForSeconds - FadeAnimation->GetEndTime());
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [WeakThis = TWeakObjectPtr<UFadingMessageWidget>(this)]
            {
                if (WeakThis.IsValid())
                {
                    WeakThis->FadeOut();
                }
            },
            Delay,
            false);
    }
}

void UFadingMessageWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
    if (Animation == FadeAnimation)
    {
        RemoveFromParent();
    }
}

void UFadingMessageWidget::FadeOut()
{
    PlayAnimation(FadeAnimation);
}
