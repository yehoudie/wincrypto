#ifndef CRYPTO_SHA256_H
#define CRYPTO_SHA256_H


#include <windows.h>
#include <bcrypt.h>



#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)

#define BUFFER_SIZE (0x1000)


#define SHA256_STR_LN (64)


typedef struct Sha256Ctxt {
    BCRYPT_ALG_HANDLE alg;
    BCRYPT_HASH_HANDLE hash;
    NTSTATUS status;
    DWORD data_size;
    DWORD hash_size;
    DWORD hash_object_size;
    PBYTE hash_object;
} Sha256Ctxt, *PSha256Ctxt;



int initSha256(PSha256Ctxt ctxt);
int cleanSha256(PSha256Ctxt ctxt);
int createHash(PSha256Ctxt ctxt);

int sha256File(WCHAR* file, BYTE *hash_bytes);
int sha256Dir(WCHAR* dir);
void printSha256(PBYTE bytes, DWORD size, WCHAR* file);

#endif
