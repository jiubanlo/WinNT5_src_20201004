/****************************************************************************
           Microsoft RPC Version 1`1
         Copyright Microsoft Corp. 1992
                Hello Example

    FILE:   kerbcli.c
    USAGE:   client    -n network_address
          -p protocol_sequence
          -e endpoint
          -o options

    PURPOSE:   Client side of RPC distributed application
    FUNCTIONS:   main() - binds to server and calls remote procedure
    COMMENTS:
    This distributed application prints a string such as "hello, world"
    on the server. The client manages its connection to the server.
    The client uses the binding handle hello_IfHandle defined in the
    file hello.h.

****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <rpc.h>       // RPC API functions, types
#include "sspitest.h"     // header file generated by MIDL compiler

int Usage(char* pszProgramName)
{
    fprintf(stderr, "Usage:  %s\n", pszProgramName);
    fprintf(stderr, " -p protocol_sequence\n");
    fprintf(stderr, " -n network_address\n");
    fprintf(stderr, " -a delegation address\n");
    fprintf(stderr, " -e endpoint\n");
    fprintf(stderr, " -o network options\n");
    fprintf(stderr, " -l authn level\n");
    fprintf(stderr, " -s authn service\n");
    fprintf(stderr, " -r recursiion level\n");
    fprintf(stderr, " -u username\n");
    fprintf(stderr, " -k password\n");
    fprintf(stderr, " -d domain\n");
    fprintf(stderr, " -x shutdown server\n");
    fprintf(stderr, " -# number of times to call\n");
    fprintf(stderr, " -t target principal\n");
    exit(1);
}

#ifndef UNLEN
#define UNLEN 256
#endif

int __cdecl
main (argc, argv)
    int argc;
    PSTR argv[];
{
    RPC_STATUS status;             // returned by RPC API function
    PSTR pszProtocolSequence = "ncacn_ip_tcp";
    PSTR pszNetworkAddress = NULL;
    PSTR pszEndpoint = "30760";
    PSTR pszOptions = NULL;
    PSTR pszStringBinding = NULL;
    PSTR pszDelegationAddress = NULL;
    PSTR pszPrincipal = NULL;
    CHAR PrincipalBuffer[UNLEN] = {0};
    ULONG PrincipalLength;
    ULONG AuthnLevel = RPC_C_AUTHN_LEVEL_DEFAULT;
    ULONG AuthnService = RPC_C_AUTHN_WINNT;
    ULONG RecursionLevel = 0;
    ULONG LoopCount = 1;
    BOOLEAN ShutdownService = FALSE;
    ULONG i;
    handle_t BindingHandle = NULL;
    SEC_WINNT_AUTH_IDENTITY_A sID = {0};

    sID.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;

    // allow the user to override settings with command line switches
    for (i = 1; i < (ULONG) argc; i++)
    {
        if ((*argv[i] == '-') || (*argv[i] == '/'))
        {
            switch (tolower(*(argv[i] + 1))) {
            case 'p':  // protocol sequence
                pszProtocolSequence = argv[++i];
                break;
            case 'n':  // network address
                pszNetworkAddress = argv[++i];
                break;
            case 'a':  // network address
                pszDelegationAddress = argv[++i];
                break;
            case 'e':
                pszEndpoint = argv[++i];
                break;
            case 'o':
                pszOptions = argv[++i];
                break;
            case 't':
                pszPrincipal = argv[++i];
                break;
            case 'u':
                sID.User = argv[++i];
                sID.UserLength = strlen(sID.User);
                break;
            case 'd':
                sID.Domain = argv[++i];
                sID.DomainLength = strlen(sID.Domain);
                break;
            case 'k':
                sID.Password = argv[++i];
                sID.PasswordLength = strlen(sID.Password);
                break;
            case 'l':
                AuthnLevel = strtol(argv[++i], NULL, 0);
                break;
            case 's':
                AuthnService = strtol(argv[++i], NULL, 0);
                break;
            case 'r':
                RecursionLevel = strtol(argv[++i], NULL, 0);
                  break;
            case '#':
                LoopCount = strtol(argv[++i], NULL, 0);
                break;
            case 'x':
                ShutdownService = TRUE;
                break;

            case 'h':
            case '?':
            default:
                Usage(argv[0]);
            }
        }
        else
        {
            Usage(argv[0]);
        }
    }

    //
    // If the principal is NULL, get it from the environment
    //

    if (pszPrincipal == NULL)
    {
        PSTR pszUserRealm;
        PSTR pszUserName;

        PrincipalBuffer[0] = '\0';

        pszUserRealm = getenv( "USERDOMAIN" );
        pszUserName  = getenv( "USERNAME" );
        if (pszUserRealm != NULL)
        {
            strcpy(PrincipalBuffer, pszUserRealm);
        }
        if ((pszUserRealm != NULL) &&
            (pszUserName != NULL))
        {
            strcat(PrincipalBuffer, "\\");
        }
        if (pszUserName != NULL)
        {
            strcat(PrincipalBuffer, pszUserName);
        }
        pszPrincipal = PrincipalBuffer;

    }


    // Use a convenience function to concatenate the elements of
    // the string binding into the proper sequence.

    status = RpcStringBindingCompose(NULL,
        pszProtocolSequence,
        pszNetworkAddress,
        pszEndpoint,
        pszOptions,
        &pszStringBinding);

    if (status)
    {
        printf("RpcStringBindingCompose returned %d\n", status);
        exit(2);
    }
    printf("pszStringBinding = %s\n", pszStringBinding);

    //
    // Set the binding handle that will be used to bind to the server.
    //

    status = RpcBindingFromStringBinding(pszStringBinding,
        &BindingHandle);
    if (status) {
        printf("RpcBindingFromStringBinding returned %d\n", status);
        exit(2);
    }


    status = RpcStringFree(&pszStringBinding);   // remote calls done; unbind
    if (status)
    {
        printf("RpcStringFree returned %d\n", status);
        exit(2);
    }

    //
    // Tell RPC to do the security thing.
    //

    printf("Binding auth info set to level %d, service %d, principal %s\n",
        AuthnLevel, AuthnService, pszPrincipal);
    status = RpcBindingSetAuthInfo(BindingHandle,
        pszPrincipal,
        AuthnLevel,
        AuthnService,
        sID.UserLength || sID.DomainLength || sID.PasswordLength ? &sID : NULL,
        RPC_C_AUTHZ_NAME);

    if ( status )
    {
        printf("RpcBindingSetAuthInfo returned %ld\n", status);
        exit(2);
    }

    //
    // Do the actual RPC calls to the server.
    //

    RpcTryExcept
    {
        for (i = 0; i < LoopCount; i++)
        {
            status = RemoteCall(
                BindingHandle,
                0,                      // no options for now
                pszDelegationAddress,
                pszProtocolSequence,
                pszEndpoint,
                pszPrincipal,
                pszNetworkAddress,
                AuthnLevel,
                AuthnService,
                RecursionLevel
                );
            if (status != 0)
            {
                printf("RemoteCall failed: 0x%x\n",status);
                break;
            }
        }

        if (ShutdownService)
        {
            Shutdown( BindingHandle );
        }
    }
    RpcExcept(EXCEPTION_EXECUTE_HANDLER)
    {
        printf("Runtime library reported an exception %d\n", RpcExceptionCode());

    } RpcEndExcept


    // The calls to the remote procedures are complete.
    // Free the binding handle

    status = RpcBindingFree(&BindingHandle);  // remote calls done; unbind
    if (status)
    {
        printf("RpcBindingFree returned %d\n", status);
        exit(2);
    }

    return 0;
}

// ====================================================================
//                MIDL allocate and free
// ====================================================================


void __RPC_FAR * __RPC_API MIDL_user_allocate(size_t len)
{
    return malloc(len);
}

void __RPC_API MIDL_user_free(void __RPC_FAR * ptr)
{
    free(ptr);
}

/* end file helloc.c */
