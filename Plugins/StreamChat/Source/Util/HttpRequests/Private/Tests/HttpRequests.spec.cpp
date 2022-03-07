// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "HttpClient.h"
#include "JsonPlaceholderPost.h"
#include "Misc/AutomationTest.h"

BEGIN_DEFINE_SPEC(FHttpRequestsSpec, "StreamChat.HttpRequests", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
TSharedRef<FHttpClient> Client = MakeShared<FHttpClient>();
const int32 TotalSimultaneousRequests = 20;
TArray<FHttpResponse> SimultaneousResponses;
END_DEFINE_SPEC(FHttpRequestsSpec)

void FHttpRequestsSpec::Define()
{
    Describe(
        "Get",
        [this]
        {
            LatentIt(
                "should return 200 and body",
                EAsyncExecution::ThreadPool,
                [this](const FDoneDelegate TestDone)
                {
                    Client->Get(TEXT("https://jsonplaceholder.typicode.com/posts/1"))
                        .Send(
                            [this, TestDone](const FHttpResponse Response)
                            {
                                AddInfo(Response.Text);
                                TestEqual("Response code", Response.StatusCode, 200);
                                TestTrue("Text", Response.Text.Contains("sunt aut facere repellat provident occaecati excepturi optio reprehenderit"));
                                TestDone.Execute();
                            });
                });

            LatentIt(
                "simultaneous requests should all return",
                EAsyncExecution::ThreadPool,
                [this](const FDoneDelegate TestDone)
                {
                    for (int32 i = 0; i < TotalSimultaneousRequests; ++i)
                    {
                        Client->Get(FString::Printf(TEXT("https://jsonplaceholder.typicode.com/posts/%d"), i + 1))
                            .Send(
                                [this, TestDone](const FHttpResponse Response)
                                {
                                    TestEqual("Response code", Response.StatusCode, 200);

                                    SimultaneousResponses.Add(Response);
                                    if (SimultaneousResponses.Num() == TotalSimultaneousRequests)
                                    {
                                        TestDone.Execute();
                                    }
                                });
                    }
                });

            LatentIt(
                "should deserialize JSON",
                EAsyncExecution::ThreadPool,
                [this](const FDoneDelegate TestDone)
                {
                    Client->Get(TEXT("https://jsonplaceholder.typicode.com/posts/1"))
                        .Send(
                            [this, TestDone](const FHttpResponse Response)
                            {
                                TestEqual("Response code", Response.StatusCode, 200);

                                const auto [UserId, Id, Title, Body] = Response.Json<FJsonPlaceholderPost>();

                                TestEqual("UserId", UserId, 1);
                                TestEqual("Id", Id, 1);
                                TestEqual("Title", Title, TEXT("sunt aut facere repellat provident occaecati excepturi optio reprehenderit"));
                                TestEqual(
                                    "Body",
                                    Body,
                                    TEXT("quia et suscipit\n"
                                         "suscipit recusandae consequuntur expedita et cum\n"
                                         "reprehenderit molestiae ut ut quas totam\n"
                                         "nostrum rerum est autem sunt rem eveniet architecto"));

                                TestDone.Execute();
                            });
                });
        });

    Describe(
        "Post",
        [this]
        {
            LatentIt(
                "should return 200 and new resource",
                EAsyncExecution::ThreadPool,
                [this](const FDoneDelegate TestDone)
                {
                    constexpr int32 FakeUserId = 11;
                    const FString FakeTitle = TEXT("Foo");
                    const FString FakeBody = TEXT("Bar");
                    Client->Post(TEXT("https://jsonplaceholder.typicode.com/posts"))
                        .Json(FJsonPlaceholderPost{FakeUserId, -1, FakeTitle, FakeBody}, ENamingConvention::UpperCamelCase)
                        .Send(
                            [=](const FHttpResponse Response)
                            {
                                AddInfo(Response.Text);
                                TestEqual("Response code", Response.StatusCode, 201);

                                const auto [UserId, Id, Title, Body] = Response.Json<FJsonPlaceholderPost>();

                                TestEqual("UserId", UserId, FakeUserId);
                                TestEqual("Id", Id, 101);
                                TestEqual("Title", Title, FakeTitle);
                                TestEqual("Body", Body, FakeBody);

                                TestDone.Execute();
                            });
                });
        });
}
