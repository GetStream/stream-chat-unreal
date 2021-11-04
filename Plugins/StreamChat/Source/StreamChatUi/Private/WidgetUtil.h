#pragma once

#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"

namespace WidgetUtil
{
UWidget* GetTypedParentWidget(UWidget*, const TSubclassOf<UWidget>);

template <class T>
T* GetTypedParentWidget(UWidget* Widget)
{
    static_assert(TIsDerivedFrom<T, UWidget>::IsDerived, "T must be derived from UWidget");
    return static_cast<T*>(GetTypedParentWidget(Widget, T::StaticClass()));
}

void DownloadImage(const FString& Url, TFunction<void(UTexture2DDynamic*)>);
uint32 HashStringWithMax(const FString&, uint32 Max = 16);
}    // namespace WidgetUtil
