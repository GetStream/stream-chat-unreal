// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ChatApi.h"

#include "ConstantTokenProvider.h"
#include "Dom/JsonObject.h"
#include "IChatSocket.h"
#include "Jwt.h"
#include "Misc/AutomationTest.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "Response/Channel/DeleteChannelResponseDto.h"
#include "TokenManager.h"

BEGIN_DEFINE_SPEC(FChatApiSpec, "StreamChat.ChatApi", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
const FString ApiKey = TEXT("kmajgxb2rk4p");
const FString Host = TEXT("chat.stream-io-api.com");
const FUserObjectDto User{TEXT("TestUser")};
const FString ChannelId = TEXT("test-channel");
const FString ChannelType = TEXT("messaging");
const TSharedRef<FTokenManager> TokenManager = MakeShared<FTokenManager>();
const TSharedRef<FChatApi> Api = FChatApi::Create(ApiKey, Host, TokenManager);
TSharedPtr<IChatSocket> Socket;
END_DEFINE_SPEC(FChatApiSpec)

void FChatApiSpec::Define()
{
    Describe(
        "Channel",
        [=]
        {
            const FString Token = Jwt::Development(User.Id);
            TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(Token), User.Id);

            LatentBeforeEach(
                [=](const FDoneDelegate& TestDone)
                {
                    Socket = IChatSocket::Create(TokenManager, ApiKey, Host, User);
                    Socket->Connect([=](const FOwnUserDto&) { TestDone.Execute(); });
                });

            AfterEach([=] { Socket->Disconnect(); });

            Describe(
                "Create",
                [=]
                {
                    LatentIt(
                        "should create a channel",
                        [=](const FDoneDelegate& TestDone)
                        {
                            const auto Callback = [=](const FChannelStateResponseDto& Dto)
                            {
                                TestEqual("Response.Channel.Cid", Dto.Channel.Cid, TEXT("messaging:test-channel"));
                                TestEqual("Response.Channel.Id", Dto.Channel.Id, ChannelId);
                                TestEqual("No additional fields", Dto.Channel.AdditionalFields.GetFields().Num(), 0);
                                TestDone.Execute();
                            };
                            Api->QueryChannel(Callback, ChannelType, Socket->GetConnectionId(), EChannelFlags::None, {}, ChannelId);
                        });

                    LatentAfterEach(
                        [=](const FDoneDelegate& TestDone)
                        {
                            const TSharedRef<FJsonObject> Filter = MakeShared<FJsonObject>();
                            Filter->SetStringField(TEXT("id"), ChannelId);
                            Api->QueryChannels(
                                [=](const FChannelsResponseDto& Dto)
                                {
                                    TestEqual("One channel in response", Dto.Channels.Num(), 1);
                                    TestDone.Execute();
                                },
                                Socket->GetConnectionId(),
                                EChannelFlags::None,
                                Filter);
                        });
                });

            Describe(
                "Delete",
                [=]
                {
                    LatentIt(
                        "should delete channel",
                        [=](const FDoneDelegate& TestDone)
                        {
                            const auto Callback = [=](const FDeleteChannelResponseDto& Dto)
                            {
                                TestEqual("Response.Channel.Cid", Dto.Channel.Cid, TEXT("messaging:test-channel"));
                                TestEqual("Response.Channel.Id", Dto.Channel.Id, TEXT("test-channel"));
                                TestEqual("No additional fields", Dto.Channel.AdditionalFields.GetFields().Num(), 0);
                                AddInfo(FString::Printf(TEXT("Duration: %s"), *Dto.Duration));
                                TestDone.Execute();
                            };
                            Api->DeleteChannel(Callback, ChannelType, ChannelId, true);
                        });

                    LatentAfterEach(
                        [=](const FDoneDelegate& TestDone)
                        {
                            const TSharedRef<FJsonObject> Filter = MakeShared<FJsonObject>();
                            Filter->SetStringField(TEXT("id"), ChannelId);
                            Api->QueryChannels(
                                [=](const FChannelsResponseDto& Dto)
                                {
                                    TestEqual("No channels in response", Dto.Channels.Num(), 0);
                                    TestDone.Execute();
                                },
                                Socket->GetConnectionId(),
                                EChannelFlags::None,
                                Filter);
                        });
                });
        });
}
