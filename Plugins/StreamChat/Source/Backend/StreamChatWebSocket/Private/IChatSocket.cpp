#include "IChatSocket.h"

#include "ChatSocket.h"

TSharedRef<IChatSocket> IChatSocket::Create(
    const TSharedRef<FTokenManager>& TokenManager,
    const FString& ApiKey,
    const FString& Host,
    const FUserObjectDto& User)
{
    return MakeShared<FChatSocket>(TokenManager, ApiKey, Host, User);
}
