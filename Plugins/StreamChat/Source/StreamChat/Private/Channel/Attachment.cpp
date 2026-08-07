// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Attachment.h"

#include "Response/Message/AttachmentDto.h"

namespace
{
// Wire values as documented for the attachment `type` field. An attachment produced by URL scraping
// arrives with no type at all, which is what distinguishes Link from Unknown.
EAttachmentType TypeFromString(const FString& RawType, const FAttachmentDto& Dto)
{
    if (RawType.Equals(TEXT("image"), ESearchCase::IgnoreCase))
    {
        return EAttachmentType::Image;
    }
    if (RawType.Equals(TEXT("file"), ESearchCase::IgnoreCase))
    {
        return EAttachmentType::File;
    }
    if (RawType.Equals(TEXT("video"), ESearchCase::IgnoreCase))
    {
        return EAttachmentType::Video;
    }
    if (RawType.Equals(TEXT("audio"), ESearchCase::IgnoreCase))
    {
        return EAttachmentType::Audio;
    }
    if (RawType.Equals(TEXT("giphy"), ESearchCase::IgnoreCase))
    {
        return EAttachmentType::Giphy;
    }
    if (RawType.IsEmpty() && !Dto.OgScrapeUrl.IsEmpty())
    {
        return EAttachmentType::Link;
    }
    return EAttachmentType::Unknown;
}

FString TypeToString(const EAttachmentType Type, const FString& RawType)
{
    switch (Type)
    {
        case EAttachmentType::Image:
            return TEXT("image");
        case EAttachmentType::File:
            return TEXT("file");
        case EAttachmentType::Video:
            return TEXT("video");
        case EAttachmentType::Audio:
            return TEXT("audio");
        case EAttachmentType::Giphy:
            return TEXT("giphy");
        case EAttachmentType::Link:
            // Scraped links carry no type on the wire
            return {};
        case EAttachmentType::Unknown:
        default:
            // Send back whatever arrived, so a type this SDK version predates survives a round trip
            return RawType;
    }
}
}    // namespace

FAttachment::FAttachment(const FAttachmentDto& Dto)
    : Title{Dto.Title}
    , AssetUrl{Dto.AssetUrl}
    , ImageUrl{Dto.ImageUrl}
    , ThumbUrl{Dto.ThumbUrl}
    , Text{Dto.Text}
    , MimeType{Dto.MimeType}
    , FileSize{Dto.FileSize}
    , RawType{Dto.Type}
    , ExtraData{Dto.AdditionalFields}
{
    Type = TypeFromString(Dto.Type, Dto);
}

FAttachmentDto FAttachment::ToDto() const
{
    FAttachmentDto Dto;
    Dto.Type = TypeToString(Type, RawType);
    Dto.Title = Title;
    Dto.AssetUrl = AssetUrl;
    Dto.ImageUrl = ImageUrl;
    Dto.ThumbUrl = ThumbUrl;
    Dto.Text = Text;
    Dto.MimeType = MimeType;
    Dto.FileSize = FileSize;
    Dto.AdditionalFields = ExtraData;
    return Dto;
}

FString FAttachment::GetUrl() const
{
    // Images come back in ImageUrl, everything else in AssetUrl. Fall back to the thumbnail so a
    // caller still has something to fetch when only a preview was generated.
    if (!ImageUrl.IsEmpty())
    {
        return ImageUrl;
    }
    if (!AssetUrl.IsEmpty())
    {
        return AssetUrl;
    }
    return ThumbUrl;
}
