// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChatApi.h"

#include "Algo/Transform.h"
#include "HttpClient.h"
#include "Request/Channel/ChannelGetOrCreateRequestDto.h"
#include "Request/Channel/HideChannelRequestDto.h"
#include "Request/Channel/MarkReadRequestDto.h"
#include "Request/Channel/QueryChannelsRequestDto.h"
#include "Request/Channel/QueryMembersRequestDto.h"
#include "Request/Channel/UpdateChannelPartialRequestDto.h"
#include "Request/Channel/UpdateChannelRequestDto.h"
#include "Request/Device/DeviceFieldsDto.h"
#include "Request/Event/SendEventRequest.h"
#include "Request/Message/SearchRequestDto.h"
#include "Request/Message/SendMessageRequestDto.h"
#include "Request/Message/UpdateMessageRequestDto.h"
#include "Request/Moderation/BanRequestDto.h"
#include "Request/Moderation/FlagRequestDto.h"
#include "Request/Moderation/MuteChannelRequestDto.h"
#include "Request/Moderation/MuteUserRequestDto.h"
#include "Request/Moderation/QueryBannedUsersRequestDto.h"
#include "Request/Reaction/SendReactionRequestDto.h"
#include "Request/User/GuestRequestDto.h"
#include "Request/User/QueryUsersRequestDto.h"
#include "Request/User/UpdateUserPartialRequestDto.h"
#include "Request/User/UpdateUsersPartialRequestDto.h"
#include "Request/User/UpdateUsersRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "Response/Channel/DeleteChannelResponseDto.h"
#include "Response/Channel/MarkReadResponseDto.h"
#include "Response/Channel/MembersResponseDto.h"
#include "Response/Channel/UpdateChannelPartialResponseDto.h"
#include "Response/Channel/UpdateChannelResponseDto.h"
#include "Response/Device/ListDevicesResponseDto.h"
#include "Response/ErrorResponseDto.h"
#include "Response/Event/EventResponseDto.h"
#include "Response/Message/MessageResponseDto.h"
#include "Response/Message/SearchResponseDto.h"
#include "Response/Moderation/FlagResponseDto.h"
#include "Response/Moderation/MuteChannelResponseDto.h"
#include "Response/Moderation/MuteUserResponseDto.h"
#include "Response/Moderation/QueryBannedUsersResponseDto.h"
#include "Response/Reaction/GetReactionsResponseDto.h"
#include "Response/Reaction/ReactionResponseDto.h"
#include "Response/ResponseDto.h"
#include "Response/User/GuestResponseDto.h"
#include "Response/User/UpdateUsersResponseDto.h"
#include "Response/User/UsersResponseDto.h"
#include "Token.h"
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

FChatApi::FChatApi(const FString& InApiKey, const FString& InHost, const TSharedPtr<FTokenManager>& InTokenManager)
    : TokenManager(InTokenManager), ApiKey(InApiKey), Client(MakeShared<FHttpClient>()), Scheme(TEXT("https")), Host(InHost)
{
}

TSharedRef<FChatApi> FChatApi::Create(const FString& InApiKey, const FString& InHost, const TSharedPtr<FTokenManager>& InTokenManager)
{
    TSharedRef<FChatApi> Api = MakeShareable(new FChatApi{InApiKey, InHost, InTokenManager});
    Api->Client->OnRequestDelegate.AddSP(Api, &FChatApi::OnRequest);
    Api->Client->OnErrorDelegate.AddSP(Api, &FChatApi::OnError);
    return Api;
}

void FChatApi::BanUser(
    const FString& TargetUserId,
    const FString& Type,
    const FString& Id,
    const TOptional<FTimespan>& Timeout,
    const TOptional<FString>& Reason,
    const bool bShadow,
    const bool bIpBan,
    const TCallback<FResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("moderation/ban"));
    FBanRequestDto Body{TargetUserId, Type, Id, bShadow, bIpBan};
    if (Reason.IsSet())
    {
        Body.Reason = Reason.GetValue();
    }
    if (Timeout.IsSet())
    {
        Body.Timeout = Timeout.GetValue().GetTotalMinutes();
    }

    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::UnbanUser(const FString& TargetUserId, const FString& Type, const FString& Id, const TCallback<FResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("moderation/ban"));
    FQueryParameters Query{{TEXT("target_user_id"), TargetUserId}};
    if (!Type.IsEmpty())
    {
        Query.Add(TEXT("type"), Type);
    }
    if (!Id.IsEmpty())
    {
        Query.Add(TEXT("id"), Id);
    }

    Client->Delete(Url).Query(Query).Send(Callback);
}

void FChatApi::QueryBannedUsers(
    const TSharedRef<FJsonObject>& Filter,
    const TArray<FSortParamRequestDto>& SortOptions,
    const TOptional<FDateTime> CreatedAtAfterOrEqual,
    const TOptional<FDateTime> CreatedAtAfter,
    const TOptional<FDateTime> CreatedAtBeforeOrEqual,
    const TOptional<FDateTime> CreatedAtBefore,
    const TOptional<uint32> Limit,
    const TOptional<uint32> Offset,
    const TCallback<FQueryBannedUsersResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("query_banned_users"));
    const FQueryBannedUsersRequestDto Body{
        Limit.Get(TNumericLimits<uint32>::Max()),
        Offset.Get(TNumericLimits<uint32>::Max()),
        Wrap(Filter),
        CreatedAtAfter.Get(FDateTime{0}),
        CreatedAtAfterOrEqual.Get(FDateTime{0}),
        CreatedAtBefore.Get(FDateTime{0}),
        CreatedAtBeforeOrEqual.Get(FDateTime{0}),
        SortOptions,
    };

    const FString Payload = Json::Serialize(Body);
    Client->Get(Url).Query({{TEXT("payload"), Payload}}).Send(Callback);
}

void FChatApi::Flag(const FString& TargetMessageId, const FString& TargetUserId, const TCallback<FFlagResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("moderation/flag"));
    const FFlagRequestDto Body{TargetMessageId, TargetUserId};
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::MuteUsers(const TArray<FString>& TargetUserIds, TOptional<FTimespan> Timeout, const TCallback<FMuteUserResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("moderation/mute"));
    FMuteUserRequestDto Body{TargetUserIds};
    if (Timeout.IsSet())
    {
        Body.Timeout = Timeout.GetValue().GetTotalMinutes();
    }
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::UnmuteUsers(const TArray<FString>& TargetUserIds, const TCallback<FResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("moderation/unmute"));
    const FMuteUserRequestDto Body{TargetUserIds};
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::MuteChannels(const TArray<FString>& TargetCids, TOptional<FTimespan> Timeout, const TCallback<FMuteChannelResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("moderation/mute/channel"));
    FMuteChannelRequestDto Body{TargetCids};
    if (Timeout.IsSet())
    {
        Body.Expiration = Timeout.GetValue().GetTotalMilliseconds();
    }
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::UnmuteChannels(const TArray<FString>& TargetCids, const TCallback<FResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("moderation/unmute/channel"));
    const FMuteChannelRequestDto Body{TargetCids};
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::QueryUsers(
    const FString& ConnectionId,
    const bool bPresence,
    const TOptional<TSharedRef<FJsonObject>>& Filter,
    const TArray<FSortParamRequestDto>& SortOptions,
    const TOptional<uint32> Limit,
    const TOptional<uint32> Offset,
    const TCallback<FUsersResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("users"));

    const FQueryUsersRequestDto Body{
        Limit.Get(TNumericLimits<uint32>::Max()),
        Offset.Get(TNumericLimits<uint32>::Max()),
        Wrap(Filter),
        // TODO expose these fields
        {},
        {},
        {},
        {},
        ConnectionId,
        bPresence,
        SortOptions,
    };

    const FString Payload = Json::Serialize(Body);
    Client->Get(Url).Query({{TEXT("payload"), Payload}}).Send(Callback);
}

void FChatApi::CreateGuest(const FUserObjectRequestDto& User, const TCallback<FGuestResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("guest"));
    const FGuestRequestDto Body{User};
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::PartialUpdateUsers(const TArray<FPartialUpdateUser>& Users, const TCallback<FUpdateUsersResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("users"));
    FUpdateUsersPartialRequestDto Body;
    Algo::Transform(Users, Body.Users, [](const FPartialUpdateUser& P) { return FUpdateUserPartialRequestDto{P.UserId, Wrap(P.Set), P.Unset}; });
    Client->Patch(Url).Json(Body).Send(Callback);
}

void FChatApi::UpsertUsers(const TMap<FString, FUserObjectRequestDto>& Users, const TCallback<FUpdateUsersResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("users"));
    const FUpdateUsersRequestDto Body{Users};
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::AddDevice(const FString& DeviceId, const EPushProvider PushProvider, const TCallback<FResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("devices"));
    const FDeviceFieldsDto Body{PushProvider, DeviceId};
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::RemoveDevice(const FString& DeviceId, const TCallback<FResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("devices"));
    Client->Delete(Url).Query({{TEXT("id"), DeviceId}}).Send(Callback);
}

void FChatApi::ListDevices(const TCallback<FListDevicesResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("devices"));
    Client->Get(Url).Send(Callback);
}

void FChatApi::QueryChannel(
    const FString& ChannelType,
    const FString& ConnectionId,
    const EChannelFlags Flags,
    const FChannelRequestDto& Data,
    const TOptional<FString>& ChannelId,
    const TOptional<FMessagePaginationParamsRequestDto> MessagePagination,
    const TOptional<FPaginationParamsRequestDto> MemberPagination,
    const TOptional<FPaginationParamsRequestDto> WatcherPagination,
    const TCallback<FChannelStateResponseDto> Callback) const
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

void FChatApi::DeleteChannel(const FString& ChannelType, const FString& ChannelId, const TCallback<FDeleteChannelResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    Client->Delete(Url).Send(Callback);
}

void FChatApi::PartialUpdateChannel(
    const FString& ChannelType,
    const FString& ChannelId,
    const TSharedRef<FJsonObject>& Set,
    const TArray<FString>& Unset,
    const TCallback<FUpdateChannelPartialResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    const FUpdateChannelPartialRequestDto Body{Wrap(Set), Unset};
    Client->Patch(Url).Json(Body).Send(Callback);
}

void FChatApi::UpdateChannel(
    const FString& ChannelType,
    const FString& ChannelId,
    const FUpdateChannelRequestDto& Data,
    const TCallback<FUpdateChannelResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    Client->Post(Url).Json(Data).Send(Callback);
}

void FChatApi::HideChannel(const FString& ChannelType, const FString& ChannelId, const bool bClearHistory, const TCallback<FResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s/hide"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    const FHideChannelRequestDto Data{bClearHistory};
    Client->Post(Url).Json(Data).Send(Callback);
}

void FChatApi::ShowChannel(const FString& ChannelType, const FString& ChannelId, const TCallback<FResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s/show"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    Client->Post(Url).EmptyJson().Send(Callback);
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
    const FName& Type,
    const bool bEnforceUnique,
    const TOptional<uint32> Score,
    const bool bSkipPush,
    const TCallback<FReactionResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("messages/%s/reaction"), *MessageId);
    const FString Url = BuildUrl(Path);
    const FSendReactionRequestDto Body{
        bEnforceUnique,
        {
            MessageId,
            Score.Get(TNumericLimits<uint32>::Max()),
            Type,
        },
        bSkipPush,
    };
    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::DeleteReaction(const FString& MessageId, const FName& Type, const TCallback<FReactionResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("messages/%s/reaction/%s"), *MessageId, *Type.ToString());
    const FString Url = BuildUrl(Path);
    Client->Delete(Url).Send(Callback);
}

void FChatApi::GetReactions(const FString& MessageId, TOptional<uint32> Limit, TOptional<uint32> Offset, const TCallback<FGetReactionsResponseDto> Callback)
    const
{
    const FString Path = FString::Printf(TEXT("messages/%s/reactions"), *MessageId);
    const FString Url = BuildUrl(Path);
    FQueryParameters Query;
    if (Limit.IsSet())
    {
        Query.Add(TEXT("limit"), FQueryParameter{static_cast<int32>(Limit.GetValue())});
    }
    if (Offset.IsSet())
    {
        Query.Add(TEXT("offset"), FQueryParameter{static_cast<int32>(Offset.GetValue())});
    }
    Client->Get(Url).Query(Query).Send(Callback);
}

void FChatApi::QueryChannels(
    const FString& ConnectionId,
    const EChannelFlags Flags,
    const TOptional<TSharedRef<FJsonObject>>& Filter,
    const TArray<FSortParamRequestDto>& SortOptions,
    const TOptional<uint32> MemberLimit,
    const TOptional<uint32> MessageLimit,
    const TOptional<uint32> Limit,
    const TOptional<uint32> Offset,
    const TCallback<FChannelsResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("channels"));

    const FQueryChannelsRequestDto Body{
        Limit.Get(TNumericLimits<uint32>::Max()),
        Offset.Get(TNumericLimits<uint32>::Max()),
        Wrap(Filter),
        MessageLimit.Get(TNumericLimits<uint32>::Max()),
        MemberLimit.Get(TNumericLimits<uint32>::Max()),
        ConnectionId,
        EnumHasAnyFlags(Flags, EChannelFlags::Presence),
        SortOptions,
        EnumHasAnyFlags(Flags, EChannelFlags::State),
        EnumHasAnyFlags(Flags, EChannelFlags::Watch),
    };

    Client->Post(Url).Json(Body).Send(Callback);
}

void FChatApi::StopWatchingChannel(const FString& ChannelType, const FString& ChannelId, const FString& ConnectionId, const TCallback<FResponseDto> Callback)
    const
{
    const FString Path = FString::Printf(TEXT("channels/%s/%s/stop-watching"), *ChannelType, *ChannelId);
    const FString Url = BuildUrl(Path);
    Client->Post(Url).Query({{TEXT("connection_id"), ConnectionId}}).EmptyJson().Send(Callback);
}

void FChatApi::SearchMessages(
    const TSharedRef<FJsonObject>& ChannelFilter,
    const TOptional<FString>& Query,
    const TOptional<TSharedRef<FJsonObject>>& MessageFilter,
    const TArray<FSortParamRequestDto>& Sort,
    const TOptional<uint32> MessageLimit,
    const TOptional<uint32> Offset,
    const TOptional<FString> Next,
    const TCallback<FSearchResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("search"));
    const FSearchRequestDto Body{
        MessageLimit.Get(TNumericLimits<uint32>::Max()),
        Offset.Get(TNumericLimits<uint32>::Max()),
        Wrap(ChannelFilter),
        Query.Get(TEXT("")),
        Wrap(MessageFilter),
        Sort,
        Next.Get(TEXT("")),
    };

    const FString Payload = Json::Serialize(Body);
    Client->Get(Url).Query({{TEXT("payload"), Payload}}).Send(Callback);
}

void FChatApi::MarkChannelsRead(const TCallback<FMarkReadResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("channels/read"));
    Client->Post(Url).Send(Callback);
}

void FChatApi::MarkChannelRead(
    const FString& ChannelType,
    const FString& ChannelId,
    const TOptional<FString>& MessageId,
    const TCallback<FMarkReadResponseDto> Callback) const
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

void FChatApi::QueryMembers(
    const FString& ChannelType,
    const TOptional<TSharedRef<FJsonObject>>& Filter,
    const TOptional<FString>& ChannelId,
    const TOptional<TArray<FString>>& Members,
    const TArray<FSortParamRequestDto>& SortOptions,
    const FMessagePaginationParamsRequestDto& Pagination,
    const TCallback<FMembersResponseDto> Callback) const
{
    const FString Url = BuildUrl(TEXT("members"));
    TArray<FChannelMemberDto> MemberDtos;
    if (Members.IsSet())
    {
        Algo::Transform(
            Members.GetValue(),
            MemberDtos,
            [](const FString& Id)
            {
                FChannelMemberDto Dto;
                Dto.UserId = Id;
                return Dto;
            });
    }
    const FQueryMembersRequestDto Body{
        ChannelType,
        ChannelId.Get(TEXT("")),
        MemberDtos,
        Wrap(Filter),
        SortOptions,
        Pagination.Limit,
        Pagination.Offset,
        Pagination.IdGt,
        Pagination.IdGte,
        Pagination.IdLt,
        Pagination.IdLte,
        Pagination.CreatedAtAfter,
        Pagination.CreatedAtAfterOrEqual,
        Pagination.CreatedAtBefore,
        Pagination.CreatedAtBeforeOrEqual,
    };

    const FString Payload = Json::Serialize(Body);
    Client->Get(Url).Query({{TEXT("payload"), Payload}}).Send(Callback);
}

void FChatApi::GetMessage(const FString& MessageId, const TCallback<FMessageResponseDto> Callback) const
{
    const FString Path = FString::Printf(TEXT("messages/%s"), *MessageId);
    const FString Url = BuildUrl(Path);
    Client->Get(Url).Send(Callback);
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

void FChatApi::AddAuth(FRequestBuilder& Request, const FToken& Token) const
{
    const auto Headers = [&]() -> FQueryParameters
    {
        switch (Token.TokenType)
        {
            case ETokenType::Jwt:
                return {
                    {TEXT("stream-auth-type"), TEXT("jwt")},
                    {TEXT("Authorization"), Token.JwtString},
                };
            case ETokenType::Anonymous:
                return {
                    {TEXT("stream-auth-type"), TEXT("anonymous")},
                };
        }
        return {};
    }();
    Request.Header(Headers).Query({{TEXT("api_key"), ApiKey}});
}

void FChatApi::OnRequest(FRequestBuilder& Request) const
{
    const FToken Token = TokenManager->LoadToken();
    AddAuth(Request, Token);
}

void FChatApi::OnError(const FHttpResponse& Response, FRequestBuilder& Request) const
{
    const FErrorResponseDto Error = Response.Json<FErrorResponseDto>();
    if (Error.IsTokenExpired())
    {
        const FToken NewToken = TokenManager->LoadToken(true);
        AddAuth(Request, NewToken);
        Request.Resend();
    }
    UE_LOG(LogTemp, Error, TEXT("API error response [Code=%d, Message=%s]"), Error.Code, *Error.Message);
}
