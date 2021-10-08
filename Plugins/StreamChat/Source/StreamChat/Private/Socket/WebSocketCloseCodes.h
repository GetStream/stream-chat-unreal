#pragma once

namespace WebSocketCloseCode
{
/// Successful operation / regular socket shutdown
constexpr uint16 Normal = 1000;
/// Client is leaving (browser tab closing)
constexpr uint16 GoingAway = 1001;
/// Endpoint received a malformed frame
constexpr uint16 ProtocolError = 1002;
/// Endpoint received an unsupported frame (e.g. binary-only endpoint received text frame)
constexpr uint16 Unsupported = 1003;
/// Expected close status, received none
constexpr uint16 NoStatus = 1005;
/// No close code frame has been received
constexpr uint16 Abnormal = 1006;
/// Endpoint received inconsistent message (e.g. malformed UTF-8)
constexpr uint16 UnsupportedPayload = 1007;
/// Generic code used for situations other than 1003 and 1009
constexpr uint16 PolicyViolation = 1008;
/// Endpoint won't process large frame
constexpr uint16 TooLarge = 1009;
/// Client wanted an extension which server did not negotiate
constexpr uint16 MandatoryExtension = 1010;
/// Internal server error while operating
constexpr uint16 ServerError = 1011;
/// Server/service is restarting
constexpr uint16 ServiceRestart = 1012;
/// Temporary server condition forced blocking client's request
constexpr uint16 TryAgainLater = 1013;
/// Server acting as gateway received an invalid response
constexpr uint16 BadGateway = 1014;
/// Transport Layer Security handshake failure
constexpr uint16 TlsHandshakeFail = 1015;

const TMap<uint16, const TCHAR*> HumanReadable = {
    {Normal, TEXT("Normal")},
    {GoingAway, TEXT("Going away")},
    {ProtocolError, TEXT("Protocol error")},
    {Unsupported, TEXT("Unsupported")},
    {NoStatus, TEXT("No status")},
    {Abnormal, TEXT("Abnormal")},
    {UnsupportedPayload, TEXT("Unsupported payload")},
    {PolicyViolation, TEXT("Policy violation")},
    {TooLarge, TEXT("Too large")},
    {MandatoryExtension, TEXT("Mandatory extension")},
    {ServerError, TEXT("Server error")},
    {ServiceRestart, TEXT("Service restart")},
    {TryAgainLater, TEXT("Try again later")},
    {BadGateway, TEXT("Bad gateway")},
    {TlsHandshakeFail, TEXT("TLS handshake fail")},
};

inline const TCHAR* ToString(const uint16 WebSocketCloseCode)
{
    const TCHAR* const* Found = HumanReadable.Find(WebSocketCloseCode);
    return Found ? *Found : TEXT("Unknown");
}
}    // namespace WebSocketCloseCode
