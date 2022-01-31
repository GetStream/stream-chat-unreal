// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ChatApi.h"

#include "ConstantTokenProvider.h"
#include "IChatSocket.h"
#include "Jwt.h"
#include "Misc/AutomationTest.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "TokenManager.h"

BEGIN_DEFINE_SPEC(FChatApiSpec, "StreamChat.ChatApi", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
const FString ApiKey = TEXT("kmajgxb2rk4p");
const FString Host = TEXT("chat.stream-io-api.com");
const FUserObjectDto User{TEXT("TestUser")};
const TSharedRef<FTokenManager> TokenManager = MakeShared<FTokenManager>();
const TSharedRef<FChatApi> Api = FChatApi::Create(ApiKey, Host, TokenManager);
TSharedPtr<IChatSocket> Socket;
END_DEFINE_SPEC(FChatApiSpec)

void FChatApiSpec::Define()
{
    Describe(
        "Create + remove channel",
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

            LatentIt(
                "should create a channel",
                [=](const FDoneDelegate& TestDone)
                {
                    Api->QueryChannel(
                        [=](const FChannelStateResponseDto& ChannelStateResponseDto)
                        {
                            TestEqual("Response.Channel.Id", ChannelStateResponseDto.Channel.Id, TEXT("test-channel"));
                            TestDone.Execute();
                        },
                        TEXT("messaging"),
                        Socket->GetConnectionId(),
                        EChannelFlags::None,
                        {},
                        {TEXT("test-channel")});
                });
        });
}
