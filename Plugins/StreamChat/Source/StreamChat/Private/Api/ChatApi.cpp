#include "ChatApi.h"

#include "Dto/Request/ChannelGetOrCreateRequestDto.h"
#include "Dto/Request/SendMessageRequestDto.h"
#include "Dto/Response/ChannelStateResponseDto.h"
#include "Dto/Response/ErrorResponseDto.h"
#include "Dto/Response/MessageResponseDto.h"
#include "StreamChatSettings.h"
#include "Token/TokenManager.h"

FChatApi::FChatApi(const FString& InApiKey, const TSharedRef<FTokenManager>& InTokenManager)
    : TokenManager(InTokenManager)
    , ApiKey(InApiKey)
    , Client(MakeShared<FHttpClient>())
    , Scheme(TEXT("https"))
    , Host(GetDefault<UStreamChatSettings>()->Host)
{
    Client->OnRequestDelegate.AddRaw(this, &FChatApi::AddAuth);
    Client->OnErrorDelegate.AddStatic(&FChatApi::OnError);
}

void FChatApi::GetOrCreateChannel(
    const FString& ChannelType,
    const FString& ConnectionId,
    const FString& ChannelId,
    const EChannelCreationFlags Flags,
    const TFunction<void(const FChannelStateResponseDto&)> Callback) const
{
    const FString ChannelPath =
        ChannelId.IsEmpty() ? ChannelType : FString::Printf(TEXT("%s/%s"), *ChannelType, *ChannelId);
    const FString Path = FString::Printf(TEXT("channels/%s/query"), *ChannelPath);
    const FString Url = BuildUrl(Path);
    const FChannelGetOrCreateRequestDto Body{
        ConnectionId,
        EnumHasAnyFlags(Flags, EChannelCreationFlags::Watch),
        EnumHasAnyFlags(Flags, EChannelCreationFlags::State),
        EnumHasAnyFlags(Flags, EChannelCreationFlags::Presence)};
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::SendNewMessage(
    const FString& ChannelType,
    const FString& ChannelId,
    const FMessageRequestDto& MessageRequest,
    const bool bSkipPush,
    const TFunction<void(const FMessageResponseDto&)> Callback) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s/message"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    const FSendMessageRequestDto Body{MessageRequest, bSkipPush};
    Client->Post(Url).Json(Body).Send(Callback);
}

FString FChatApi::BuildUrl(const FString& Path) const
{
    checkf(Path[0] != TEXT('/'), TEXT("Don't prefix the path with a slash [Path=%s]"), *Path);
    return FString::Printf(TEXT("%s://%s/%s"), *Scheme, *Host, *Path);
}

void FChatApi::AddAuth(FRequestBuilder& Request) const
{
    const FString Token = TokenManager->LoadToken();
    Request
        .Header({
            {TEXT("stream-auth-type"), TEXT("jwt")},
            {TEXT("Authorization"), Token},
        })
        .Query({{TEXT("api_key"), ApiKey}});
}

void FChatApi::OnError(const FHttpResponse& Response)
{
    const auto [StatusCode, Code, Duration, Message, MoreInfo] = Response.Json<FErrorResponseDto>();
    UE_LOG(LogTemp, Error, TEXT("API error response [Code=%d, Message=%s]"), Code, *Message);
}
