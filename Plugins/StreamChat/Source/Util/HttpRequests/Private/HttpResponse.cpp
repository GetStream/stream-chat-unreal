// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "HttpResponse.h"

#include "Interfaces/IHttpResponse.h"

FHttpResponse::FHttpResponse(const FHttpResponsePtr Response)
{
    if (Response)
    {
        StatusCode = Response->GetResponseCode();
        Text = Response->GetContentAsString();
    }
}

bool FHttpResponse::IsSuccessful() const
{
    return StatusCode >= 200 && StatusCode < 300;
}
