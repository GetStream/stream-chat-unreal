// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"

#include "AttachmentDto.generated.h"

/**
 * @brief #/components/schemas/Attachment
 * A file, image or link attached to a message.
 *
 * Attachments are either supplied by the sender, produced by a slash command, or synthesised by the
 * backend when it scrapes a URL found in the message text. Only a handful of fields are meaningful
 * for any given attachment: an image uses ImageUrl, a file uses AssetUrl, and a scraped link fills
 * in the OgScrapeUrl and author fields. Anything not modelled here survives in AdditionalFields.
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FAttachmentDto
{
    GENERATED_BODY()

    /// Attachment type, e.g. `image`, `file`, `video`, `audio` or `giphy`.
    /// Left empty on upload, the backend infers it from the uploaded content.
    UPROPERTY()
    FString Type;

    /// Display title, conventionally the original filename
    UPROPERTY()
    FString Title;

    /// URL the title links to
    UPROPERTY()
    FString TitleLink;

    /// Description text
    UPROPERTY()
    FString Text;

    /// URL of the uploaded file. Set for non-image attachments
    UPROPERTY()
    FString AssetUrl;

    /// URL of the uploaded image. Set for image attachments
    UPROPERTY()
    FString ImageUrl;

    /// URL of a smaller preview of the image, when the backend generated one
    UPROPERTY()
    FString ThumbUrl;

    /// MIME type of the attached file, e.g. `image/png`
    UPROPERTY()
    FString MimeType;

    /// Size of the attached file in bytes.
    /// int32 is sufficient: Stream caps uploads at 100 MB
    UPROPERTY()
    int32 FileSize = 0;

    /// URL that was scraped to produce this attachment, for link previews
    UPROPERTY()
    FString OgScrapeUrl;

    /// Author of the scraped page
    UPROPERTY()
    FString AuthorName;

    /// Link to the author of the scraped page
    UPROPERTY()
    FString AuthorLink;

    /// Text to show when the attachment itself cannot be rendered
    UPROPERTY()
    FString Fallback;

    /// Fields the SDK does not model, kept so they survive a round trip
    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
