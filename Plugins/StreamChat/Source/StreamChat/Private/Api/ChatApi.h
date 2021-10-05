#pragma once
#include "Dto/Response/ChannelState.h"

class FChatApi
{
public:
	explicit FChatApi(const FString& ApiKey);
	void GetOrCreateChannel(TFunction<void(FChannelState)> Callback,
		const FString& ChannelType,
		bool bState = true,
		bool bWatch = false,
		bool bPresence = false,
		const FString& ChannelId = {});

private:
	FString BuildUrl(const FString& Path) const;

	FString ApiKey;
	FString Scheme;
	FString Host;
};
