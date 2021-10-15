#include "IChatSocket.h"

#include "ChatSocket.h"

TSharedRef<IChatSocket>
IChatSocket::Create(const FString& ApiKey, const FString& Token, const FString& Host, const FUserDto& User)
{
    return MakeShared<FChatSocket>(ApiKey, Token, Host, User);
}
