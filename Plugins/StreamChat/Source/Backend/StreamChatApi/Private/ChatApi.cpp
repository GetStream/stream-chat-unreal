// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ChatApi.h"

#include "HttpClient.h"
#include "Request/Channel/ChannelGetOrCreateRequestDto.h"
#include "Request/Channel/MarkReadRequestDto.h"
#include "Request/Channel/QueryChannelsRequestDto.h"
#include "Request/Event/SendEventRequest.h"
#include "Request/Message/SearchRequestDto.h"
#include "Request/Message/SendMessageRequestDto.h"
#include "Request/Message/UpdateMessageRequestDto.h"
#include "Request/Reaction/SendReactionRequestDto.h"
#include "Request/User/QueryUsersRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "Response/Channel/DeleteChannelResponseDto.h"
#include "Response/Channel/MarkReadResponseDto.h"
#include "Response/ErrorResponseDto.h"
#include "Response/Event/EventResponseDto.h"
#include "Response/Message/MessageResponseDto.h"
#include "Response/Message/SearchResponseDto.h"
#include "Response/Reaction/ReactionResponseDto.h"
#include "TokenManager.h"

namespace
{
FJsonObjectWrapper Wrap(const TSharedRef<FJsonObject>& JsonObject)
{
    FJsonObjectWrapper Wrapper;
    Wrapper.JsonObject = JsonObject;
    return Wrapper;
}
FJsonObjectWrapper Wrap(const TOptional<TSharedRef<FJsonObject>>& JsonObject)
{
    if (JsonObject.IsSet())
    {
        FJsonObjectWrapper Wrapper;
        Wrapper.JsonObject = JsonObject.GetValue();
        return Wrapper;
    }
    return {};
}
}    // namespace

TSharedRef<FChatApi> FChatApi::Create(const FString& InApiKey, const FString& InHost, const TSharedPtr<FTokenManager>& InTokenManager)
{
    TSharedRef<FChatApi> Api = MakeShareable(new FChatApi{InApiKey, InHost, InTokenManager});
    Api->Client->OnRequestDelegate.AddSP(Api, &FChatApi::OnRequest);
    Api->Client->OnErrorDelegate.AddSP(Api, &FChatApi::OnError);
    return Api;
}

void FChatApi::QueryUsers(
    const TCallback<FUsersResponseDto> Callback,
    const FString& ConnectionId,
    const bool bPresence,
    const TOptional<TSharedRef<FJsonObject>>& Filter,
    const TArray<FSortParamRequestDto>& SortOptions,
    const TOptional<uint32> Limit,
    const TOptional<uint32> Offset) const
{
    const FString Url = BuildUrl(TEXT("users"));

    const FQueryUsersRequestDto Body{
        ConnectionId,
        Limit.Get(10),
        Offset.Get(0),
        Wrap(Filter),
        bPresence,
        SortOptions,
    };

    Client->Get(Url).Json(Body).Send(Callback);
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
    FChannelGetOrCreateRequestDto Body{
        ConnectionId,
        Data,
        EnumHasAnyFlags(Flags, EChannelFlags::Watch),
        EnumHasAnyFlags(Flags, EChannelFlags::State),
        EnumHasAnyFlags(Flags, EChannelFlags::Presence),
    };
    if (MemberPagination.IsSet())
    {
        Body.SetMembers(MemberPagination.GetValue());
    }
    if (MessagePagination.IsSet())
    {
        Body.SetMessages(MessagePagination.GetValue());
    }
    if (WatcherPagination.IsSet())
    {
        Body.SetWatchers(WatcherPagination.GetValue());
    }
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::DeleteChannel(const TCallback<FDeleteChannelResponseDto> Callback, const FString& ChannelType, const FString& ChannelId, bool bHardDelete) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    Client->Delete(Url).Query({{TEXT("hard_delete"), bHardDelete}}).Send(Callback);
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
    const TOptional<TSharedRef<FJsonObject>>& Filter,
    const TArray<FSortParamRequestDto>& SortOptions,
    const TOptional<uint32> MemberLimit,
    const TOptional<uint32> MessageLimit,
    const TOptional<uint32> Limit,
    const TOptional<uint32> Offset) const
{
    const FString Url = BuildUrl(TEXT("channels"));

    const FQueryChannelsRequestDto Body{
        Limit.Get(10),
        Offset.Get(0),
        MessageLimit.Get(25),
        MemberLimit.Get(100),
        ConnectionId,
        Wrap(Filter),
        EnumHasAnyFlags(Flags, EChannelFlags::Presence),
        SortOptions,
        EnumHasAnyFlags(Flags, EChannelFlags::State),
        EnumHasAnyFlags(Flags, EChannelFlags::Watch),
    };

    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::SearchMessages(
    const TCallback<FSearchResponseDto> Callback,
    const TSharedRef<FJsonObject>& ChannelFilter,
    const TOptional<FString>& Query,
    const TOptional<TSharedRef<FJsonObject>>& MessageFilter,
    const TArray<FSortParamRequestDto>& Sort,
    TOptional<uint32> MessageLimit,
    TOptional<uint32> Offset,
    TOptional<FString> Next) const
{
    const FString Url = BuildUrl(TEXT("search"));
    FSearchRequestDto Body{
        Wrap(ChannelFilter),
    };
    if (Query.IsSet())
    {
        Body.SetQuery(Query.GetValue());
    }
    if (MessageFilter.IsSet())
    {
        Body.SetMessageFilter(MessageFilter.GetValue());
    }
    if (Sort.Num() > 0)
    {
        Body.SetSort(Sort);
    }
    if (MessageLimit.IsSet())
    {
        Body.SetMessageLimit(MessageLimit.GetValue());
    }
    if (Offset.IsSet())
    {
        Body.SetOffset(Offset.GetValue());
    }
    if (Next.IsSet())
    {
        Body.SetNext(Next.GetValue());
    }

    const FString Payload = Json::Serialize(Body);
    Client->Get(Url).Query({{TEXT("payload"), Payload}}).Send(Callback);
}

void FChatApi::MarkChannelsRead(const TCallback<FMarkReadResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("channels/read"));
    Client->Post(Url).Send(Callback);
}

void FChatApi::MarkChannelRead(
    const TCallback<FMarkReadResponseDto> Callback,
    const FString& ChannelType,
    const FString& ChannelId,
    const TOptional<FString>& MessageId) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s/read"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    FMarkReadRequestDto Body{};
    if (MessageId.IsSet())
    {
        Body.SetMessageId(MessageId.GetValue());
    }
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::SendChannelEventInternal(
    const FString& ChannelType,
    const FString& ChannelId,
    const TSharedRef<FJsonObject>& Event,
    const TCallback<FEventResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s/event"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    const FSendEventRequestDto Body{Wrap(Event)};

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

void FChatApi::OnError(const FHttpResponse& Response, FRequestBuilder& Request) const
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
