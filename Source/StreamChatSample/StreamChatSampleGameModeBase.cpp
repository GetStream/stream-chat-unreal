// Copyright Epic Games, Inc. All Rights Reserved.

#include "StreamChatSampleGameModeBase.h"

#include "StreamChatSampleHud.h"

AStreamChatSampleGameModeBase::AStreamChatSampleGameModeBase()
{
	HUDClass = AStreamChatSampleHud::StaticClass();
}
