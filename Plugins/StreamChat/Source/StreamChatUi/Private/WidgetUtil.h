// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"

namespace WidgetUtil
{
UWidget* GetTypedParentWidget(const UWidget*, const TSubclassOf<UWidget>);

template <class T>
T* GetTypedParentWidget(const UWidget* Widget)
{
    static_assert(TIsDerivedFrom<T, UWidget>::IsDerived, "T must be derived from UWidget");
    return static_cast<T*>(GetTypedParentWidget(Widget, T::StaticClass()));
}

FString TruncateWithEllipsis(const FString&, uint32 MaxWidth, const FSlateFontInfo& FontInfo);

uint32 HashStringWithMax(const FString&, uint32 Max = 16);
}    // namespace WidgetUtil
