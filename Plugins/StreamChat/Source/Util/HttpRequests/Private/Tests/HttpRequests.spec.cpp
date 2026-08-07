// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "HttpClient.h"
#include "JsonPlaceholderPost.h"
#include "Misc/AutomationTest.h"
#include "RequestBuilder.h"

BEGIN_DEFINE_SPEC(FHttpRequestsSpec, "StreamChat.HttpRequests", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)
TSharedRef<FHttpClient> Client = MakeShared<FHttpClient>();
const int32 TotalSimultaneousRequests = 20;
TArray<FHttpResponse> SimultaneousResponses;

// A multipart body is bytes, not text: it can hold NULs and invalid UTF-8, so it is compared as
// bytes rather than converted to an FString, which would stop at the first NUL.
static TArray<uint8> Utf8(const FString& Text)
{
    const FTCHARToUTF8 Converted{*Text};
    TArray<uint8> Out;
    Out.Append(reinterpret_cast<const uint8*>(Converted.Get()), Converted.Length());
    return Out;
}
static bool StartsWith(const TArray<uint8>& Body, const FString& Text)
{
    const TArray<uint8> Needle = Utf8(Text);
    return Body.Num() >= Needle.Num() && FMemory::Memcmp(Body.GetData(), Needle.GetData(), Needle.Num()) == 0;
}
static bool EndsWith(const TArray<uint8>& Body, const FString& Text)
{
    const TArray<uint8> Needle = Utf8(Text);
    return Body.Num() >= Needle.Num() && FMemory::Memcmp(Body.GetData() + Body.Num() - Needle.Num(), Needle.GetData(), Needle.Num()) == 0;
}
static bool Contains(const TArray<uint8>& Body, const FString& Text)
{
    const TArray<uint8> Needle = Utf8(Text);
    for (int32 i = 0; i + Needle.Num() <= Body.Num(); ++i)
    {
        if (FMemory::Memcmp(Body.GetData() + i, Needle.GetData(), Needle.Num()) == 0)
        {
            return true;
        }
    }
    return false;
}
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

                                const FJsonPlaceholderPost Post = Response.Json<FJsonPlaceholderPost>();

                                TestEqual("UserId", Post.UserId, 1);
                                TestEqual("Id", Post.Id, 1);
                                TestEqual("Title", Post.Title, TEXT("sunt aut facere repellat provident occaecati excepturi optio reprehenderit"));
                                TestEqual(
                                    "Body",
                                    Post.Body,
                                    TEXT("quia et suscipit\n"
                                         "suscipit recusandae consequuntur expedita et cum\n"
                                         "reprehenderit molestiae ut ut quas totam\n"
                                         "nostrum rerum est autem sunt rem eveniet architecto"));

                                TestDone.Execute();
                            });
                });
        });

    Describe(
        "Multipart",
        [this]
        {
            It("should keep arbitrary binary content byte for byte",
               [this]
               {
                   // Bytes that do not survive a trip through FString: a NUL, a lone 0xFF, and a
                   // CR/LF pair that must not be mistaken for part boundaries.
                   const TArray<uint8> Content{0x00, 0xFF, 0x0D, 0x0A, 0x7B, 0x80, 0x00};
                   const TArray<uint8> Body =
                       FRequestBuilder::BuildMultipartBody(TEXT("BOUND"), TEXT("file"), TEXT("a.bin"), Content, TEXT("application/octet-stream"));

                   const FString Preamble =
                       TEXT("--BOUND\r\nContent-Disposition: form-data; name=\"file\"; filename=\"a.bin\"\r\nContent-Type: application/octet-stream\r\n\r\n");
                   const int32 Offset = Preamble.Len();
                   TestEqual("Total length", Body.Num(), Offset + Content.Num() + FString(TEXT("\r\n--BOUND--\r\n")).Len());
                   for (int32 i = 0; i < Content.Num(); ++i)
                   {
                       TestEqual(FString::Printf(TEXT("Content byte %d"), i), Body[Offset + i], Content[i]);
                   }
               });

            It("should terminate lines with CRLF and close the boundary",
               [this]
               {
                   const TArray<uint8> Body =
                       FRequestBuilder::BuildMultipartBody(TEXT("BOUND"), TEXT("file"), TEXT("a.txt"), TArray<uint8>{0x41}, TEXT("text/plain"));

                   TestTrue("Opens with boundary", StartsWith(Body, TEXT("--BOUND\r\nContent-Disposition:")));
                   TestTrue("Blank line before content", Contains(Body, TEXT("text/plain\r\n\r\nA")));
                   TestTrue("Closes with terminating boundary", EndsWith(Body, TEXT("\r\n--BOUND--\r\n")));

                   // Every LF in the framing must be part of a CRLF. The content here is a single
                   // 'A', so any bare LF would have to have come from the framing.
                   for (int32 i = 0; i < Body.Num(); ++i)
                   {
                       if (Body[i] == '\n')
                       {
                           TestTrue(FString::Printf(TEXT("LF at %d is preceded by CR"), i), i > 0 && Body[i - 1] == '\r');
                       }
                   }
               });

            It("should not let a filename inject extra headers",
               [this]
               {
                   // A filename is attacker controlled: a quote would end the parameter early and a
                   // CRLF would start a header of the attacker's choosing.
                   const FString Hostile = TEXT("evil\"\r\nX-Injected: yes\r\n\r\nname.txt");
                   const TArray<uint8> Body =
                       FRequestBuilder::BuildMultipartBody(TEXT("BOUND"), TEXT("file"), Hostile, TArray<uint8>{0x41}, TEXT("text/plain"));

                   // The text survives inside the quoted filename, but must never begin a line.
                   TestFalse("Not injected as its own header", Contains(Body, TEXT("\r\nX-Injected")));
                   TestTrue("Filename stays on one line", Contains(Body, TEXT("filename=\"evil'X-Injected: yesname.txt\"")));

                   // Exactly one part header, i.e. the hostile filename did not start a second one.
                   int32 Count = 0;
                   const TArray<uint8> Needle = Utf8(TEXT("Content-Disposition"));
                   for (int32 i = 0; i + Needle.Num() <= Body.Num(); ++i)
                   {
                       if (FMemory::Memcmp(Body.GetData() + i, Needle.GetData(), Needle.Num()) == 0)
                       {
                           ++Count;
                       }
                   }
                   TestEqual("Exactly one Content-Disposition", Count, 1);
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
                            // TestDone is captured by value because the response arrives after the enclosing scope is gone
                            [this, TestDone, FakeUserId, FakeTitle, FakeBody](const FHttpResponse Response)
                            {
                                AddInfo(Response.Text);
                                TestEqual("Response code", Response.StatusCode, 201);

                                const FJsonPlaceholderPost Post = Response.Json<FJsonPlaceholderPost>();

                                TestEqual("UserId", Post.UserId, FakeUserId);
                                TestEqual("Id", Post.Id, 101);
                                TestEqual("Title", Post.Title, FakeTitle);
                                TestEqual("Body", Post.Body, FakeBody);

                                TestDone.Execute();
                            });
                });
        });
}
