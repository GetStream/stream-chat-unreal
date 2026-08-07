// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "AttachmentLayout.h"

namespace AttachmentLayout
{
FVector2D FitToBudget(const FIntPoint Source, const float BudgetWidth, const float MaxAspect, const float DpiScale)
{
    if (Source.X <= 0 || Source.Y <= 0)
    {
        // Nothing to take an aspect ratio from, so reserve a modest landscape box for now
        return FVector2D{BudgetWidth, FMath::RoundToFloat(BudgetWidth * 0.6f)};
    }

    const float Scale = FMath::Max(1.f, DpiScale);
    const float NaturalWidth = static_cast<float>(Source.X) / Scale;

    float Width = FMath::Min(BudgetWidth, NaturalWidth);
    float Height = Width * static_cast<float>(Source.Y) / static_cast<float>(Source.X);

    // Too tall: shrink the whole thing until the height fits, rather than squashing the picture
    const float MaxHeight = Width * FMath::Max(1.f, MaxAspect);
    if (Height > MaxHeight)
    {
        Width *= MaxHeight / Height;
        Height = MaxHeight;
    }

    return FVector2D{FMath::RoundToFloat(Width), FMath::RoundToFloat(Height)};
}

FString WithResizeQuery(const FString& Url, const FIntPoint MaxPixels)
{
    if (!Url.Contains(TEXT("stream-io-cdn.com/"), ESearchCase::IgnoreCase) || MaxPixels.X <= 0 || MaxPixels.Y <= 0)
    {
        return Url;
    }

    const TCHAR* Separator = Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?");
    return FString::Printf(TEXT("%s%sw=%d&h=%d&resize=clip"), *Url, Separator, MaxPixels.X, MaxPixels.Y);
}

}    // namespace AttachmentLayout
