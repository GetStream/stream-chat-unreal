// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "PushProvider.h"
#include "StreamChatClientComponent.h"
#include "User/Device.h"

namespace PushDevices
{
UStreamChatClientComponent* Client = nullptr;

// https://getstream.io/chat/docs/unreal/push_devices/?language=unreal#register-a-device
void RegisterADevice()
{
    // register the device with APN (Apple only)
    Client->AddDevice(TEXT("device-token"), EPushProvider::Apn);

    // register the device with Firebase (Android only)
    Client->AddDevice(TEXT("device-token"), EPushProvider::Firebase);
}

// https://getstream.io/chat/docs/unreal/push_devices/?language=unreal#unregister-a-device
void UnregisterADevice()
{
    Client->RemoveDevice(TEXT("device-token"));
}

// https://getstream.io/chat/docs/unreal/push_devices/?language=unreal#list-devices
void ListDevices()
{
    Client->ListDevices(
        [](const TArray<FDevice> Devices)
        {
            // Do something with Devices
        });
}

}    // namespace PushDevices
