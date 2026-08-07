// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FileUploadResponseDto.generated.h"

/**
 * @brief #/components/schemas/FileUploadResponse
 * Returned by the file and image upload endpoints.
 *
 * The uploaded URL comes back in `file` for both endpoints. `thumb_url` is only populated by the
 * image endpoint, and only once the backend has generated a preview.
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FFileUploadResponseDto
{
    GENERATED_BODY()

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;

    /// URL of the uploaded file, to be used as an attachment's asset or image URL
    UPROPERTY()
    FString File;

    /// URL of a generated preview. Image uploads only, and may be empty
    UPROPERTY()
    FString ThumbUrl;
};
