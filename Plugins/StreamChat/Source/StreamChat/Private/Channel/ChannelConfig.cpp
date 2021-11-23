// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelConfig.h"

#include "Response/Channel/ChannelConfigWithInfoDto.h"

FChannelConfig::FChannelConfig(const FChannelConfigWithInfoDto& Dto)
    : bConnectEvents(Dto.bConnectEvents)
    , MaxMessageLength(Dto.MaxMessageLength)
    , bMutes(Dto.bMutes)
    , bReactions(Dto.bReactions)
    , bReadEvents(Dto.bReadEvents)
    , bReplies(Dto.bReplies)
    , bSearch(Dto.bSearch)
    , bTypingEvents(Dto.bTypingEvents)
    , bUploads(Dto.bUploads)
    , bUrlEnrichment(Dto.bURLEnrichment)
{
}
