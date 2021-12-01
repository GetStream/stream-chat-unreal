// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ChatApi.h"

#include "HttpClient.h"
#include "Request/Channel/ChannelGetOrCreateRequestDto.h"
#include "Request/Channel/QueryChannelsRequestDto.h"
#include "Request/Event/SendEventRequest.h"
#include "Request/Message/SendMessageRequestDto.h"
#include "Request/Message/UpdateMessageRequestDto.h"
#include "Request/Reaction/SendReactionRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "Response/ErrorResponseDto.h"
#include "Response/Message/MessageResponseDto.h"
#include "Response/Reaction/ReactionResponseDto.h"
#include "TokenManager.h"

TSharedRef<FChatApi> FChatApi::Create(const FString& InApiKey, const FString& InHost, const TSharedPtr<FTokenManager>& InTokenManager)
{
    TSharedRef<FChatApi> Api = MakeShareable(new FChatApi{InApiKey, InHost, InTokenManager});
    Api->Client->OnRequestDelegate.AddSP(Api, &FChatApi::OnRequest);
    Api->Client->OnErrorDelegate.AddSP(Api, &FChatApi::OnError);
    return Api;
}

FChatApi::FChatApi(const FString& InApiKey, const FString& InHost, const TSharedPtr<FTokenManager>& InTokenManager)
    : TokenManager(InTokenManager), ApiKey(InApiKey), Client(MakeShared<FHttpClient>()), Scheme(TEXT("https")), Host(InHost)
{
}

void FChatApi::QueryChannel(
    const TCallback<FChannelStateResponseDto> Callback,
    const FString& ChannelType,
    const FString& ConnectionId,
    const EChannelFlags Flags,
    const FChannelRequestDto& Data,
    const TOptional<FString>& ChannelId,
    const TOptional<FMessagePaginationParamsRequestDto> MessagePagination,
    const TOptional<FPaginationParamsRequestDto> MemberPagination,
    const TOptional<FPaginationParamsRequestDto> WatcherPagination) const
{
    const FString ChannelPath = !ChannelId.IsSet() ? ChannelType : FString::Printf(TEXT("%s/%s"), *ChannelType, *ChannelId.GetValue());
    const FString Path = FString::Printf(TEXT("channels/%s/query"), *ChannelPath);
    const FString Url = BuildUrl(Path);
    const FChannelGetOrCreateRequestDto Body{
        ConnectionId,
        Data,
        EnumHasAnyFlags(Flags, EChannelFlags::Watch),
        EnumHasAnyFlags(Flags, EChannelFlags::State),
        EnumHasAnyFlags(Flags, EChannelFlags::Presence),
        MemberPagination,
        MessagePagination,
        WatcherPagination,
    };
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::SendNewMessage(
    const FString& ChannelType,
    const FString& ChannelId,
    const FMessageRequestDto& MessageRequest,
    const bool bSkipPush,
    const TCallback<FMessageResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s/message"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    const FSendMessageRequestDto Body{MessageRequest, bSkipPush};
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::UpdateMessage(const FMessageRequestDto& MessageRequest, const TCallback<FMessageResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("messages/%s"), *MessageRequest.Id);
    const FString Url = BuildUrl(Path);
    const FUpdateMessageRequestDto Body{MessageRequest};
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::DeleteMessage(const FString& Id, bool bHard, const TCallback<FMessageResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("messages/%s"), *Id);
    const FString Url = BuildUrl(Path);
    FRequestBuilder Req = Client->Delete(Url);
    if (bHard)
    {
        Req.Query({{TEXT("hard"), true}});
    }
    Req.Send(Callback);
}

void FChatApi::SendReaction(
    const FString& MessageId,
    const FReactionRequestDto& ReactionRequest,
    const bool bEnforceUnique,
    const bool bSkipPush,
    const TCallback<FReactionResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("messages/%s/reaction"), *MessageId);
    const FString Url = BuildUrl(Path);
    const FSendReactionRequestDto Body{bEnforceUnique, ReactionRequest, bSkipPush};
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::DeleteReaction(const FString& MessageId, const FName& Type, const TCallback<FReactionResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("messages/%s/reaction/%s"), *MessageId, *Type.ToString());
    const FString Url = BuildUrl(Path);
    Client->Delete(Url).Send(Callback);
}

void FChatApi::QueryChannels(
    const TCallback<FChannelsResponseDto> Callback,
    const FString& ConnectionId,
    const EChannelFlags Flags,
    const TOptional<FJsonObjectWrapper>& Filter,
    const TArray<FSortParamRequestDto>& SortOptions,
    const TOptional<uint32> MemberLimit,
    const TOptional<uint32> MessageLimit,
    const FPaginationOptions PaginationOptions) const
{
    const FString Url = BuildUrl(TEXT("channels"));
    // TODO FJsonObjectWrapper for filter?
    // TODO Pagination?
    const FQueryChannelsRequestDto Body{
        ConnectionId,
        Filter.Get({}),
        PaginationOptions.Limit,
        MemberLimit,
        MessageLimit,
        PaginationOptions.Offset,
        EnumHasAnyFlags(Flags, EChannelFlags::Presence),
        SortOptions,
        EnumHasAnyFlags(Flags, EChannelFlags::State),
        EnumHasAnyFlags(Flags, EChannelFlags::Watch),
    };
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::SendChannelEventInternal(
    const FString& ChannelType,
    const FString& ChannelId,
    const FJsonObjectWrapper& Event,
    const TCallback<FEventResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s/event"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    const FSendEventRequestDto Body{Event};

    Client->Post(Url).Json(Body).Send(Callback);
}

FString FChatApi::BuildUrl(const FString& Path) const
{
    checkf(Path[0] != TEXT('/'), TEXT("Don't prefix the path with a slash [Path=%s]"), *Path);
    return FString::Printf(TEXT("%s://%s/%s"), *Scheme, *Host, *Path);
}

void FChatApi::AddAuth(FRequestBuilder& Request, const FString& Token) const
{
    Request
        .Header({
            {TEXT("stream-auth-type"), TEXT("jwt")},
            {TEXT("Authorization"), Token},
        })
        .Query({{TEXT("api_key"), ApiKey}});
}

void FChatApi::OnRequest(FRequestBuilder& Request) const
{
    const FString Token = TokenManager->LoadToken();
    AddAuth(Request, Token);
}

void FChatApi::OnError(const FHttpResponse& Response, FRequestBuilder& Request)
{
    const FErrorResponseDto Error = Response.Json<FErrorResponseDto>();
    if (Error.IsTokenExpired())
    {
        const FString NewToken = TokenManager->LoadToken(true);
        AddAuth(Request, NewToken);
        Request.Resend();
    }
    UE_LOG(LogTemp, Error, TEXT("API error response [Code=%d, Message=%s]"), Error.Code, *Error.Message);
}
