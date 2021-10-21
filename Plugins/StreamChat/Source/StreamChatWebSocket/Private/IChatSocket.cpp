#include "IChatSocket.h"

#include "ChatSocket.h"

TSharedRef<IChatSocket>
IChatSocket::Create(const FString& ApiKey, const FString& Token, const FString& Host, const FUserObjectDto& User)
{
    return MakeShared<FChatSocket>(ApiKey, Token, Host, User);
}
