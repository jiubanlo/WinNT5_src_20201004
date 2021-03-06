/*++

Copyright (c) 1998-2002 Microsoft Corporation

Module Name:

    HttpIo.c

Abstract:

    User-mode interface to HTTP.SYS: Server-side I/O handler.

Author:

    Keith Moore (keithmo)        15-Dec-1998

Revision History:

--*/


#include "precomp.h"


//
// Private macros.
//


//
// Private prototypes.
//


//
// Public functions.
//

/***************************************************************************++

Routine Description:

    Waits for an incoming HTTP request from HTTP.SYS.

Arguments:

    AppPoolHandle - Supplies a handle to a HTTP.SYS application pool
        as returned from either HttpCreateAppPool() or
        HttpOpenAppPool().

    RequestId - Supplies an opaque identifier to receive a specific
        request. If this value is HTTP_NULL_ID, then receive any request.

    Flags - Currently unused and must be zero.

    pRequestBuffer - Supplies a pointer to the request buffer to be filled
        in by HTTP.SYS.

    RequestBufferLength - Supplies the length of pRequestBuffer.

    pBytesReturned - Optionally supplies a pointer to a ULONG which will
        receive the actual length of the data returned in the request buffer
        if this request completes synchronously (in-line).

    pOverlapped - Optionally supplies an OVERLAPPED structure for the
        request.

Return Value:

    ULONG - Completion status.

--***************************************************************************/
ULONG
WINAPI
HttpReceiveHttpRequest(
    IN HANDLE AppPoolHandle,
    IN HTTP_REQUEST_ID RequestId,
    IN ULONG Flags,
    OUT PHTTP_REQUEST pRequestBuffer,
    IN ULONG  RequestBufferLength,
    OUT PULONG  pBytesReturned OPTIONAL,
    IN LPOVERLAPPED pOverlapped OPTIONAL
    )
{
    HTTP_RECEIVE_REQUEST_INFO ReceiveInfo;

#if DBG
    if (pRequestBuffer)
    {
        RtlFillMemory( pRequestBuffer, RequestBufferLength, (UCHAR)'\xc' );
    }
#endif

    ReceiveInfo.RequestId = RequestId;
    ReceiveInfo.Flags = Flags;

    //
    // Make the request.
    //

    return HttpApiDeviceControl(
                AppPoolHandle,                      // FileHandle
                pOverlapped,                        // pOverlapped
                IOCTL_HTTP_RECEIVE_HTTP_REQUEST,    // IoControlCode
                &ReceiveInfo,                       // pInputBuffer
                sizeof(ReceiveInfo),                // InputBufferLength
                pRequestBuffer,                     // pOutputBuffer
                RequestBufferLength,                // OutputBufferLength
                pBytesReturned                      // pBytesTransferred
                );

} // HttpReceiveHttpRequest

/***************************************************************************++

Routine Description:

    Receives entity body for a request already read via ReceiveHttpRequest.

Arguments:

    AppPoolHandle - Supplies a handle to a HTTP.SYS application pool
        as returned from either HttpCreateAppPool() or
        HttpOpenAppPool().

    RequestId - Supplies an opaque identifier to receive a specific
        request. If this value is HTTP_NULL_ID, then receive any request.

    pEntityBodyBuffer - Supplies a pointer to the request buffer to be filled
        in by HTTP.SYS.

    EntityBufferLength - Supplies the length of pEntityBuffer.

    pBytesReturned - Optionally supplies a pointer to a ULONG which will
        receive the actual length of the data returned in the request buffer
        if this request completes synchronously (in-line).

    pOverlapped - Optionally supplies an OVERLAPPED structure for the
        request.

Return Value:

    ULONG - Completion status.

--***************************************************************************/
ULONG
WINAPI
HttpReceiveRequestEntityBody(
    IN HANDLE AppPoolHandle,
    IN HTTP_REQUEST_ID RequestId,
    IN ULONG Flags,
    OUT PVOID pEntityBuffer,
    IN ULONG  EntityBufferLength,
    OUT PULONG  pBytesReturned,
    IN LPOVERLAPPED pOverlapped OPTIONAL
    )
{
    HTTP_RECEIVE_REQUEST_INFO ReceiveInfo;

#if DBG
    if (pEntityBuffer != NULL)
    {
        RtlFillMemory( pEntityBuffer, EntityBufferLength, (UCHAR)'\xc' );
    }
#endif

    ReceiveInfo.RequestId = RequestId;
    ReceiveInfo.Flags = Flags;

    //
    // Make the request.
    //

    return HttpApiDeviceControl(
                AppPoolHandle,                      // FileHandle
                pOverlapped,                        // pOverlapped
                IOCTL_HTTP_RECEIVE_ENTITY_BODY,     // IoControlCode
                &ReceiveInfo,                       // pInputBuffer
                sizeof(ReceiveInfo),                // InputBufferLength
                pEntityBuffer,                      // pOutputBuffer
                EntityBufferLength,                 // OutputBufferLength
                pBytesReturned                      // pBytesTransferred
                );

} // HttpReceiveRequestEntityBody


/***************************************************************************++

Routine Description:

    Sends an HTTP response on the specified connection.

Arguments:

    AppPoolHandle - Supplies a handle to a HTTP.SYS application pool
        as returned from either HttpCreateAppPool() or
        HttpOpenAppPool().

    RequestId - Supplies an opaque identifier specifying the request
        the response is for.

    Flags - Supplies zero or more HTTP_SEND_RESPONSE_FLAG_* control flags.

    pHttpResponse - Supplies the HTTP response.

    pCachePolicy - Supplies caching policy for the response.

    pBytesSent - Optionally supplies a pointer to a ULONG which will
        receive the actual length of the data sent if this request
        completes synchronously (in-line).

    pOverlapped - Optionally supplies an OVERLAPPED structure.

Return Value:

    ULONG - Completion status.

--***************************************************************************/
ULONG
WINAPI
HttpSendHttpResponse(
    IN HANDLE AppPoolHandle,
    IN HTTP_REQUEST_ID RequestId,
    IN ULONG Flags,
    IN PHTTP_RESPONSE pHttpResponse,
    IN PHTTP_CACHE_POLICY pCachePolicy OPTIONAL,
    OUT PULONG  pBytesSent OPTIONAL,
    OUT PHTTP_REQUEST pRequestBuffer OPTIONAL,
    IN ULONG RequestBufferLength OPTIONAL,
    IN LPOVERLAPPED pOverlapped OPTIONAL,
    IN PHTTP_LOG_FIELDS_DATA pLogData OPTIONAL
    )
{
    HTTP_SEND_HTTP_RESPONSE_INFO responseInfo;

    //
    // Build the response structure.
    //

    RtlZeroMemory(&responseInfo, sizeof(responseInfo));

    responseInfo.pHttpResponse      = pHttpResponse;
    responseInfo.EntityChunkCount   = pHttpResponse->EntityChunkCount;
    responseInfo.pEntityChunks      = pHttpResponse->pEntityChunks;

    if (pCachePolicy != NULL)
    {
        responseInfo.CachePolicy    = *pCachePolicy;
    } else {
        responseInfo.CachePolicy.Policy = HttpCachePolicyNocache;
        responseInfo.CachePolicy.SecondsToLive = 0;
    }

    responseInfo.RequestId          = RequestId;
    responseInfo.Flags              = Flags;    
    responseInfo.pLogData           = pLogData;
    
    if (pRequestBuffer)
    {
        pRequestBuffer->RequestId   = HTTP_NULL_ID;
    }

    //
    // Make the request.
    //

    return HttpApiDeviceControl(
                AppPoolHandle,                      // FileHandle
                pOverlapped,                        // pOverlapped
                IOCTL_HTTP_SEND_HTTP_RESPONSE,      // IoControlCode
                &responseInfo,                      // pInputBuffer
                sizeof(responseInfo),               // InputBufferLength
                pRequestBuffer,                     // pOutputBuffer
                RequestBufferLength,                // OutputBufferLength
                pBytesSent                          // pBytesTransferred
                );

} // HttpSendHttpResponse



/***************************************************************************++

Routine Description:

    Sends an HTTP response on the specified connection.

Arguments:

    AppPoolHandle - Supplies a handle to a HTTP.SYS application pool
        as returned from either HttpCreateAppPool() or
        HttpOpenAppPool().

    RequestId - Supplies an opaque identifier specifying the request
        the response is for.

    Flags - Supplies zero or more HTTP_SEND_RESPONSE_FLAG_* control flags.

    pBytesSent - Optionally supplies a pointer to a ULONG which will
        receive the actual length of the data sent if this request
        completes synchronously (in-line).

    pOverlapped - Optionally supplies an OVERLAPPED structure.

Return Value:

    ULONG - Completion status.

--***************************************************************************/
ULONG
WINAPI
HttpSendResponseEntityBody(
    IN HANDLE AppPoolHandle,
    IN HTTP_REQUEST_ID RequestId,
    IN ULONG Flags,
    IN USHORT EntityChunkCount OPTIONAL,
    IN PHTTP_DATA_CHUNK pEntityChunks OPTIONAL,
    OUT PULONG  pBytesSent OPTIONAL,
    OUT PHTTP_REQUEST pRequestBuffer OPTIONAL,
    IN ULONG RequestBufferLength OPTIONAL,
    IN LPOVERLAPPED pOverlapped OPTIONAL,
    IN PHTTP_LOG_FIELDS_DATA pLogData OPTIONAL
    )
{
    HTTP_SEND_HTTP_RESPONSE_INFO responseInfo;

    //
    // Build the response structure.
    //

    RtlZeroMemory(&responseInfo, sizeof(responseInfo));

    responseInfo.EntityChunkCount   = EntityChunkCount;
    responseInfo.pEntityChunks      = pEntityChunks;
    responseInfo.RequestId          = RequestId;
    responseInfo.Flags              = Flags;
    responseInfo.pLogData           = pLogData;

    if (pRequestBuffer)
    {
        pRequestBuffer->RequestId   = HTTP_NULL_ID;
    }

    //
    // Make the request.
    //

    return HttpApiDeviceControl(
                AppPoolHandle,                      // FileHandle
                pOverlapped,                        // pOverlapped
                IOCTL_HTTP_SEND_ENTITY_BODY,        // IoControlCode
                &responseInfo,                      // pInputBuffer
                sizeof(responseInfo),               // InputBufferLength
                pRequestBuffer,                     // pOutputBuffer
                RequestBufferLength,                // OutputBufferLength
                pBytesSent                          // pBytesTransferred
                );

} // HttpSendResponseEntityBody


/***************************************************************************++

Routine Description:

    Wait for the client to initiate a disconnect.

Arguments:

    AppPoolHandle - Supplies a handle to a HTTP.SYS application pool
        as returned from either HttpCreateAppPool() or
        HttpOpenAppPool().

    ConnectionId - Supplies an opaque identifier specifying the connection.

    pOverlapped - Optionally supplies an OVERLAPPED structure.

Return Value:

    ULONG - Completion status.

--***************************************************************************/
ULONG
WINAPI
HttpWaitForDisconnect(
    IN HANDLE AppPoolHandle,
    IN HTTP_CONNECTION_ID ConnectionId,
    IN LPOVERLAPPED pOverlapped OPTIONAL
    )
{
    HTTP_WAIT_FOR_DISCONNECT_INFO waitInfo;

    //
    // Build the structure.
    //

    waitInfo.ConnectionId = ConnectionId;

    //
    // Make the request.
    //

    return HttpApiDeviceControl(
                AppPoolHandle,                      // FileHandle
                pOverlapped,                        // pOverlapped
                IOCTL_HTTP_WAIT_FOR_DISCONNECT,     // IoControlCode
                &waitInfo,                          // pInputBuffer
                sizeof(waitInfo),                   // InputBufferLength
                NULL,                               // pOutputBuffer
                0,                                  // OutputBufferLength
                NULL                                // pBytesTransferred
                );

} // HttpWaitForDisconnect


//
// Private functions.
//

