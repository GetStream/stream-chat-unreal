// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"

class UMenuAnchor;

namespace WidgetUtil
{
UWidget* GetTypedParentWidget(const UWidget*, const TSubclassOf<UWidget>);
UWidget* GetTypedChildWidget(const UWidget*, const TSubclassOf<UWidget>);

template <class T>
T* GetTypedParentWidget(const UWidget* Widget)
{
    static_assert(TIsDerivedFrom<T, UWidget>::IsDerived, "T must be derived from UWidget");
    return static_cast<T*>(GetTypedParentWidget(Widget, T::StaticClass()));
}
template <class T>
T* GetTypedChildWidget(const UWidget* Widget)
{
    static_assert(TIsDerivedFrom<T, UWidget>::IsDerived, "T must be derived from UWidget");
    return static_cast<T*>(GetTypedChildWidget(Widget, T::StaticClass()));
}

/**
 * @brief Shorten an input string to fit in a given pixel width
 * @param MaxWidth Available space for text to occupy, in pixels
 * @param FontInfo Font style, size, for measuring text
 * @return The input string, shortened and appended with ...
 */
FString TruncateWithEllipsis(const FString&, uint32 MaxWidth, const FSlateFontInfo& FontInfo);

uint32 HashStringWithMax(const FString&, uint32 Max = 16);

FLinearColor ChooseColorForString(const FString&);

void HideDefaultMenuBackground(UMenuAnchor* Anchor);

}    // namespace WidgetUtil
