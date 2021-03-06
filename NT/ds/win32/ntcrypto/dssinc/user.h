/* user.h */

/*********************************/
/* Definitions                   */
/*********************************/


/*********************************/
/* Structure Definitions         */
/*********************************/


/*********************************/
/* Function Definitions          */
/*********************************/

extern DWORD
logonUser(
    LPCSTR pszUserID,
    DWORD dwFlags,
    DWORD dwProvType,
    LPCSTR szProvName,
    HCRYPTPROV *phUID);

extern DWORD
logoffUser(
    Context_t *context);

// Read the user record
extern DWORD
readUserKeys(
    IN Context_t *pContext,
    IN DWORD dwKeysetType);

extern DWORD
writeUserKeys(
    Context_t *context);

//
// Routine : ProtectPrivKey
//
// Description : Encrypts the private key and persistently stores it.
//

extern DWORD
ProtectPrivKey(
    IN OUT Context_t *pContext,
    IN LPWSTR szPrompt,
    IN DWORD dwFlags,
    IN BOOL fSigKey);

//
// Routine : UnprotectPrivKey
//
// Description : Decrypts the private key.  If the fAlwaysDecrypt flag is set
//               then it checks if the private key is already in the buffer
//               and if so then it does not decrypt.
//

extern DWORD
UnprotectPrivKey(
    IN OUT Context_t *pContext,
    IN LPWSTR szPrompt,
    IN BOOL fSigKey,
    IN BOOL fAlwaysDecrypt);

