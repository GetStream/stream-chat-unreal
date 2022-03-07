// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "UiBlueprintLibrary.h"

#include "Algo/ForEach.h"
#include "Components/ScrollBox.h"
#include "Components/Widget.h"
#include "Framework/Application/IMenu.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "Layout/WidgetPath.h"
#include "Math/Color.h"
#include "User/User.h"
#include "WidgetUtil.h"

void UUiBlueprintLibrary::AddContextMenu(UWidget* Widget, const FPointerEvent& InMouseEvent)
{
    // Right clicked, so summon a context menu if the cursor is within the widget
    const FWidgetPath WidgetPath = InMouseEvent.GetEventPath() != nullptr ? *InMouseEvent.GetEventPath() : FWidgetPath();

    if (const TSharedPtr<SWidget> MenuContentWidget = Widget->TakeWidget(); MenuContentWidget.IsValid())
    {
        static constexpr bool bFocusImmediately = true;
        TSharedPtr<IMenu> ContextMenu = FSlateApplication::Get().PushMenu(
            InMouseEvent.GetWindow(),
            WidgetPath,
            MenuContentWidget.ToSharedRef(),
            InMouseEvent.GetScreenSpacePosition(),
            FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
            bFocusImmediately);
    }
}

void UUiBlueprintLibrary::DismissContextMenu(UWidget* Widget)
{
    FSlateApplication::Get().DismissMenuByWidget(Widget->TakeWidget());
}

FString UUiBlueprintLibrary::GetChannelTitle(const UChatChannel* Channel)
{
    if (!Channel)
    {
        return TEXT("");
    }

    if (TOptional<FString> Name = Channel->Properties.GetName())
    {
        return Name.GetValue();
    }

    const auto OthersPredicate = [&](const FMember& M)
    {
        return !M.User.IsCurrent();
    };

    if (Channel->Properties.Members.Num() > 2)
    {
        FString Result;
        bool First = true;
        Algo::ForEachIf(
            Channel->Properties.Members,
            OthersPredicate,
            [&](const FMember& M)
            {
                if (First)
                {
                    First = false;
                }
                else
                {
                    Result += TEXT(", ");
                }
                Result += M.User->Name;
            });
        return Result;
    }

    if (const FMember* Member = Channel->Properties.Members.FindByPredicate(OthersPredicate))
    {
        return Member->User->Name;
    }

    return TEXT("");
}

void UUiBlueprintLibrary::GetFirstVisibleChildOfScrollBox(UScrollBox* ScrollBox, int32& OutIndex, float& OutLeadingEdge)
{
    OutIndex = INDEX_NONE;
    const FGeometry Geo = ScrollBox->GetCachedGeometry();
    FArrangedChildren ArrangedChildren(EVisibility::Visible);
    const TSharedRef<SScrollBox> ScrollBoxWidget = StaticCastSharedRef<SScrollBox>(ScrollBox->TakeWidget());
    ScrollBoxWidget->GetChildren()
        ->GetChildAt(0)    // HorizontalBox
        ->GetChildren()
        ->GetChildAt(0)    // Overlay
        ->GetChildren()
        ->GetChildAt(0)    // ScrollPanel
        ->ArrangeChildren(Geo, ArrangedChildren);
    for (int32 Index = 0; Index < ArrangedChildren.Num(); ++Index)
    {
        if (const float Local = Geo.AbsoluteToLocal(ArrangedChildren[Index].Geometry.GetAbsolutePosition()).Y; Local > 0.f)
        {
            OutIndex = Index;
            OutLeadingEdge = Local;
            return;
        }
    }
}

const FUser& UUiBlueprintLibrary::DerefUserRef(const FUserRef& User)
{
    return User.GetUser();
}

bool UUiBlueprintLibrary::IsCurrent(const FUserRef& User)
{
    return User.IsCurrent();
}

TArray<FMessage> UUiBlueprintLibrary::FilterRecent(const TArray<FMessage>& Messages, const FTimespan& Since)
{
    return Messages.FilterByPredicate([&](const FMessage& M) { return FDateTime::UtcNow() - M.CreatedAt < Since; });
}

FLinearColor UUiBlueprintLibrary::GenerateColorFromString(const FString& Input)
{
    return WidgetUtil::ChooseColorForString(Input);
}

FString UUiBlueprintLibrary::Conv_ColorToHexString(const FLinearColor& InColor)
{
    return InColor.ToFColor(true).ToHex().LeftChop(2);
}
