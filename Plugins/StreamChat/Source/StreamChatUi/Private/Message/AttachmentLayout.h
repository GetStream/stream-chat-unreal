// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/// Working out how much room an inline image gets, kept apart from the widget so it can be tested
namespace AttachmentLayout
{
/**
 * @brief The size, in slate units, to draw an image of the given pixel dimensions at.
 *
 * Fitted to BudgetWidth with the height following from the aspect ratio, which is never distorted.
 * An image smaller than the budget is left at its own size rather than blown up, and one taller than
 * MaxAspect times its width is scaled down as a whole so a single photo cannot fill the message list.
 *
 * @param Source Pixel dimensions of the image, or anything non-positive if they are not known yet
 * @param BudgetWidth Widest the image may be drawn, in slate units
 * @param MaxAspect How many times its own width the image may be tall
 * @param DpiScale Slate units per pixel, so a source's own size can be compared against the budget
 */
FVector2D FitToBudget(FIntPoint Source, float BudgetWidth, float MaxAspect, float DpiScale);

/**
 * @brief Ask Stream's CDN for an image no bigger than it is about to be drawn.
 *
 * A photo straight off a phone camera runs to 25 megapixels, which is 93 MB once decoded to BGRA,
 * for something that ends up a couple of hundred points wide. `resize=clip` fits the image inside
 * the box without distorting it, and never enlarges.
 *
 * Only Stream's CDN understands these, so any other URL is returned untouched rather than being sent
 * query parameters it might reject.
 */
FString WithResizeQuery(const FString& Url, FIntPoint MaxPixels);

}    // namespace AttachmentLayout
