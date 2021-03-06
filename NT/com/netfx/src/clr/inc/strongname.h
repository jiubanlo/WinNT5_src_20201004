// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
#ifndef __STRONG_NAME_H
#define __STRONG_NAME_H

// ===========================================================================
// File: StrongName.h
// 
// Wrappers for signing and hashing functions needed to implement strong names
// ===========================================================================


#include <windows.h>
#include <wincrypt.h>
#include <ole2.h>
#include <corerror.h>


#ifdef __cplusplus
extern "C"{
#endif 


// Public key blob binary format.
typedef struct {
    unsigned int SigAlgID;       // (ALG_ID) signature algorithm used to create the signature
    unsigned int HashAlgID;      // (ALG_ID) hash algorithm used to create the signature
    ULONG        cbPublicKey;    // length of the key in bytes
    BYTE         PublicKey[1];   // variable length byte array containing the key value in format output by CryptoAPI
} PublicKeyBlob;


// Location in the registry (under HKLM) that strong name configuration info is
// stored.
#define SN_CONFIG_KEY               "Software\\Microsoft\\StrongName"
#define SN_CONFIG_CSP               "CSP"                   // REG_SZ
#define SN_CONFIG_MACHINE_KEYSET    "MachineKeyset"         // REG_DWORD
#define SN_CONFIG_HASH_ALG          "HashAlgorithm"         // REG_DWORD
#define SN_CONFIG_SIGN_ALG          "SignAlgorithm"         // REG_DWORD
#define SN_CONFIG_VERIFICATION      "Verification"          // Registry subkey
#define SN_CONFIG_USERLIST          "UserList"              // REG_MULTI_SZ
#define SN_CONFIG_CACHE_VERIFY      "CacheVerify"           // REG_DWORD

#define SN_CONFIG_KEY_W             L"Software\\Microsoft\\StrongName"
#define SN_CONFIG_CSP_W             L"CSP"                  // REG_SZ
#define SN_CONFIG_MACHINE_KEYSET_W  L"MachineKeyset"        // REG_DWORD
#define SN_CONFIG_HASH_ALG_W        L"HashAlgorithm"        // REG_DWORD
#define SN_CONFIG_SIGN_ALG_W        L"SignAlgorithm"        // REG_DWORD
#define SN_CONFIG_VERIFICATION_W    L"Verification"         // Registry subkey
#define SN_CONFIG_USERLIST_W        L"UserList"             // REG_MULTI_SZ
#define SN_CONFIG_CACHE_VERIFY_W    L"CacheVerify"          // REG_DWORD


#ifdef SNAPI_INTERNAL
#define SNAPI __declspec(dllexport) BOOLEAN __stdcall
#define SNAPI_(_type) __declspec(dllexport) _type __stdcall
#else
#define SNAPI __declspec(dllimport) BOOLEAN __stdcall
#define SNAPI_(_type) __declspec(dllimport) _type __stdcall
#endif


// Return last error.
SNAPI_(DWORD) StrongNameErrorInfo(VOID);


// Free buffer allocated by routines below.
SNAPI_(VOID) StrongNameFreeBuffer(BYTE *pbMemory);  // [in] address of memory to free


// Generate a new key pair for strong name use.
SNAPI StrongNameKeyGen(LPCWSTR  wszKeyContainer,    // [in] desired key container name
                       DWORD    dwFlags,            // [in] flags (see below)
                       BYTE   **ppbKeyBlob,         // [out] public/private key blob
                       ULONG   *pcbKeyBlob);

// Flags for StrongNameKeyGen.
#define SN_LEAVE_KEY    0x00000001                  // Leave key pair registered with CSP


// Import key pair into a key container.
SNAPI StrongNameKeyInstall(LPCWSTR  wszKeyContainer,// [in] desired key container name, must be a non-empty string
                           BYTE    *pbKeyBlob,      // [in] public/private key pair blob
                           ULONG    cbKeyBlob);


// Delete a key pair.
SNAPI StrongNameKeyDelete(LPCWSTR wszKeyContainer); // [in] desired key container name


// Retrieve the public portion of a key pair.
SNAPI StrongNameGetPublicKey (LPCWSTR   wszKeyContainer,    // [in] desired key container name
                              BYTE     *pbKeyBlob,          // [in] public/private key blob (optional)
                              ULONG     cbKeyBlob,
                              BYTE    **ppbPublicKeyBlob,   // [out] public key blob
                              ULONG    *pcbPublicKeyBlob);


// Hash and sign a manifest.
SNAPI StrongNameSignatureGeneration(LPCWSTR     wszFilePath,        // [in] valid path to the PE file for the assembly
                                    LPCWSTR     wszKeyContainer,    // [in] desired key container name
                                    BYTE       *pbKeyBlob,          // [in] public/private key blob (optional)
                                    ULONG       cbKeyBlob,
                                    BYTE      **ppbSignatureBlob,   // [out] signature blob
                                    ULONG      *pcbSignatureBlob);


// Create a strong name token from an assembly file.
SNAPI StrongNameTokenFromAssembly(LPCWSTR   wszFilePath,            // [in] valid path to the PE file for the assembly
                                  BYTE    **ppbStrongNameToken,     // [out] strong name token 
                                  ULONG    *pcbStrongNameToken);

// Create a strong name token from an assembly file and additionally return the full public key.
SNAPI StrongNameTokenFromAssemblyEx(LPCWSTR   wszFilePath,            // [in] valid path to the PE file for the assembly
                                    BYTE    **ppbStrongNameToken,     // [out] strong name token 
                                    ULONG    *pcbStrongNameToken,
                                    BYTE    **ppbPublicKeyBlob,       // [out] public key blob
                                    ULONG    *pcbPublicKeyBlob);

// Create a strong name token from a public key blob.
SNAPI StrongNameTokenFromPublicKey(BYTE    *pbPublicKeyBlob,        // [in] public key blob
                                   ULONG    cbPublicKeyBlob,
                                   BYTE   **ppbStrongNameToken,     // [out] strong name token 
                                   ULONG   *pcbStrongNameToken);


// Verify a strong name/manifest against a public key blob.
SNAPI StrongNameSignatureVerification(LPCWSTR wszFilePath,      // [in] valid path to the PE file for the assembly
                                      DWORD   dwInFlags,        // [in] flags modifying behaviour (see below)
                                      DWORD  *pdwOutFlags);     // [out] additional output info (see below)


// Verify a strong name/manifest against a public key blob.
SNAPI StrongNameSignatureVerificationEx(LPCWSTR     wszFilePath,        // [in] valid path to the PE file for the assembly
                                        BOOLEAN     fForceVerification, // [in] verify even if settings in the registry disable it
                                        BOOLEAN    *pfWasVerified);     // [out] set to false if verify succeeded due to registry settings


// Verify a strong name/manifest against a public key blob when the assembly is
// already memory mapped.
SNAPI StrongNameSignatureVerificationFromImage(BYTE     *pbBase,             // [in] base address of mapped manifest file
                                               DWORD     dwLength,           // [in] length of mapped image in bytes
                                               DWORD     dwInFlags,          // [in] flags modifying behaviour (see below)
                                               DWORD    *pdwOutFlags);       // [out] additional output info (see below)

// Flags for use with the verify routines.
#define SN_INFLAG_FORCE_VER      0x00000001     // verify even if settings in the registry disable it
#define SN_INFLAG_INSTALL        0x00000002     // verification is the first (on entry to the cache)
#define SN_INFLAG_ADMIN_ACCESS   0x00000004     // cache protects assembly from all but admin access
#define SN_INFLAG_USER_ACCESS    0x00000008     // cache protects user's assembly from other users
#define SN_INFLAG_ALL_ACCESS     0x00000010     // cache provides no access restriction guarantees
#define SN_INFLAG_RUNTIME        0x80000000     // internal debugging use only 

#define SN_OUTFLAG_WAS_VERIFIED  0x00000001     // set to false if verify succeeded due to registry settings


// Verify that two assemblies differ only by signature blob.
SNAPI StrongNameCompareAssemblies(LPCWSTR   wszAssembly1,           // [in] file name of first assembly
                                  LPCWSTR   wszAssembly2,           // [in] file name of second assembly
                                  DWORD    *pdwResult);             // [out] result of comparison (see codes below)

#define SN_CMP_DIFFERENT    0   // Assemblies contain different data
#define SN_CMP_IDENTICAL    1   // Assemblies are exactly the same, even signatures
#define SN_CMP_SIGONLY      2   // Assemblies differ only by signature (and checksum etc.)


// Compute the size of buffer needed to hold a hash for a given hash algorithm.
SNAPI StrongNameHashSize(ULONG  ulHashAlg,  // [in] hash algorithm
                         DWORD *pcbSize);   // [out] size of the hash in bytes


// Compute the size that needs to be allocated for a signature in an assembly.
SNAPI StrongNameSignatureSize(BYTE    *pbPublicKeyBlob,    // [in] public key blob
                              ULONG    cbPublicKeyBlob,
                              DWORD   *pcbSize);           // [out] size of the signature in bytes


SNAPI_(DWORD) GetHashFromAssemblyFile(LPCSTR szFilePath, // [IN] location of file to be hashed
                                      unsigned int *piHashAlg, // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                                      BYTE   *pbHash,    // [OUT] hash buffer
                                      DWORD  cchHash,    // [IN]  max size of buffer
                                      DWORD  *pchHash);  // [OUT] length of hash byte array
    
SNAPI_(DWORD) GetHashFromAssemblyFileW(LPCWSTR wszFilePath, // [IN] location of file to be hashed
                                       unsigned int *piHashAlg, // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                                       BYTE   *pbHash,    // [OUT] hash buffer
                                       DWORD  cchHash,    // [IN]  max size of buffer
                                       DWORD  *pchHash);  // [OUT] length of hash byte array
    
SNAPI_(DWORD) GetHashFromFile(LPCSTR szFilePath, // [IN] location of file to be hashed
                              unsigned int *piHashAlg,   // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                              BYTE   *pbHash,    // [OUT] hash buffer
                              DWORD  cchHash,    // [IN]  max size of buffer
                              DWORD  *pchHash);  // [OUT] length of hash byte array
    
SNAPI_(DWORD) GetHashFromFileW(LPCWSTR wszFilePath, // [IN] location of file to be hashed
                               unsigned int *piHashAlg,   // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                               BYTE   *pbHash,    // [OUT] hash buffer
                               DWORD  cchHash,    // [IN]  max size of buffer
                               DWORD  *pchHash);  // [OUT] length of hash byte array
    
SNAPI_(DWORD) GetHashFromHandle(HANDLE hFile,      // [IN] handle of file to be hashed
                                unsigned int *piHashAlg,   // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                                BYTE   *pbHash,    // [OUT] hash buffer
                                DWORD  cchHash,    // [IN]  max size of buffer
                                DWORD  *pchHash);  // [OUT] length of hash byte array

SNAPI_(DWORD) GetHashFromBlob(BYTE   *pbBlob,       // [IN] pointer to memory block to hash
                              DWORD  cchBlob,       // [IN] length of blob
                              unsigned int *piHashAlg,  // [IN/OUT] constant specifying the hash algorithm (set to 0 if you want the default)
                              BYTE   *pbHash,       // [OUT] hash buffer
                              DWORD  cchHash,       // [IN]  max size of buffer
                              DWORD  *pchHash);     // [OUT] length of hash byte array

#ifdef __cplusplus
}
#endif

#endif
