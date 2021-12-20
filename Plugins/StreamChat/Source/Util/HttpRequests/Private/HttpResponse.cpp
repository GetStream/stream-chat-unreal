// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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
