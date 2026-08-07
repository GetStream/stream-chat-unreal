// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Attachment.h"
#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"

namespace FileUploads
{
UChatChannel* Channel;
TArray<uint8> ImageBytes;
TArray<uint8> FileBytes;

// https://getstream.io/chat/docs/unreal/file-uploads/?language=unreal#uploading-files-to-a-channel
void UploadingFilesToAChannel()
{
    // Uploading and sending are separate steps. Upload first, then put the resulting
    // attachment in FMessage::Attachments and send that message.
    //
    // The SDK takes bytes rather than a path, so the choice of file picker, and the
    // platform differences that come with it, stay in your app.
    Channel->UploadImage(
        TEXT("image.jpg"),
        ImageBytes,
        [](const FAttachment& Attachment)
        {
            FMessage Message{TEXT("Check this out")};
            Message.Attachments.Add(Attachment);
            Channel->SendMessage(Message);
        });

    // Anything that is not an image goes through UploadFile. Prefer UploadImage for
    // images: the backend recognises them, may generate a thumbnail, and clients
    // render them inline.
    Channel->UploadFile(
        TEXT("document.pdf"),
        FileBytes,
        [](const FAttachment& Attachment)
        {
            FMessage Message;
            Message.Attachments.Add(Attachment);
            Channel->SendMessage(Message);
        });
}

// https://getstream.io/chat/docs/unreal/file-uploads/?language=unreal#uploading-standalone-files
void UploadingStandaloneFiles()
{
    // Uploading outside a channel is not yet available in the Unreal SDK.
    // As a workaround, upload through UChatChannel::UploadImage and reuse the returned
    // URL when you update the user or channel.
    Channel->UploadImage(
        TEXT("avatar.jpg"),
        ImageBytes,
        [](const FAttachment& Attachment)
        {
            const FString ImageUrl = Attachment.GetUrl();
            // Store ImageUrl in the user's or channel's image field
        });
}

// https://getstream.io/chat/docs/unreal/file-uploads/?language=unreal#deleting-files
void DeletingFiles()
{
    FAttachment Attachment;

    // Routed to the image or the file endpoint depending on the attachment, so you do
    // not have to remember which one it was uploaded through.
    Channel->DeleteAttachment(
        Attachment,
        [](const bool& bSuccess)
        {
            // Deleted
        });
}

// https://getstream.io/chat/docs/unreal/file-uploads/?language=unreal#using-your-own-cdn
void UsingYourOwnCdn()
{
    // The Unreal SDK has no custom uploader hook. Nothing about sending a message
    // requires the URL to have come from Stream, so upload to your own CDN and then
    // build the attachment yourself.
    FAttachment Attachment;
    Attachment.Type = EAttachmentType::Image;
    Attachment.ImageUrl = TEXT("file-url-to-your-cdn");
    Attachment.Title = TEXT("image.jpg");

    FMessage Message{TEXT("Message with file attachment")};
    Message.Attachments.Add(Attachment);
    Channel->SendMessage(Message);
}

}    // namespace FileUploads
