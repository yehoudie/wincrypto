#include "sha256.h"

#include <stdio.h>
#include <errno.h>

#include "debug.h"
#include "files.h"
#include "filesW.h"



Sha256Ctxt sha256;

const char* vs = "1.0.1";
const char* last_changed = "18.05.2021";


int compare(int argc, WCHAR** argv);
void printColored(CHAR* value, WORD attributes);

int runList(int argc, WCHAR** argv);

void printUsage()
{
    printf("Sha256\n");
    printf("Version: %s\n", vs);
    printf("Last changed: %s\n\n", last_changed);
    printf("Usage: sha256 [/h] [/c] path ...\n");
}

void printHelp()
{
    printUsage();
    wprintf(L"\n");
    wprintf(L"Options:\n");
    wprintf(L" /c: Compare path1 with path2 or path1 with shaValue.\n");
    wprintf(L" /h: Print this\n");
    wprintf(L" path: One or more pathes to files or dirs for sha256 calculation.\n");
}


int __cdecl wmain(int argc, WCHAR** argv)
{
    int s = 0;
    BOOL compare_f = FALSE;
    
    if (argc < 2)
    {
        printUsage();
        return 1;
    }
    if ( argv[1][0] == '/' )
    {
        if ( argv[1][1] == 'h' && argv[1][2] == 0 )
        {
            printHelp();
            return;
        }
        if ( argv[1][1] == 'c' && argv[1][2] == 0 )
        {
            compare_f = TRUE;
        }
    }

    initSha256(&sha256);

    if ( compare_f )
    {
        s = compare(argc, argv);
    }
    else
    {
        s = runList(argc, argv);
    }
    

    cleanSha256(&sha256);

    return s;
}

int compare(int argc, WCHAR** argv)
{
    WCHAR full_path1[MAX_PATH];
    WCHAR* base_name1 = NULL;
    BYTE hash1_bytes[32];

    WCHAR full_path2[MAX_PATH];
    WCHAR* base_name2 = NULL;
    BYTE hash2_bytes[32];
    WCHAR* ptr = NULL;
    
    SIZE_T i;
    SIZE_T j;

    if ( argc < 4 )
    {
        wprintf(L"ERROR: You have to provide two pathes or a path and a value to compare!");
        return -1;
    }
    
    debug_winfo(L"file1: %ws\n", argv[2]);
    memset(full_path1, 0, MAX_PATH*2);
    getFullPathName(argv[2], full_path1, &base_name1);
    debug_winfo(L" - full_path1: %ws\n", full_path1);
    debug_winfo(L" - base_name1: %ws\n", base_name1);

    if ( !fileExists(full_path1) )
    {
        wprintf(L"ERROR: Path \"%s\" does not exist!", full_path1);
        return -2;
    }

    sha256File(full_path1, hash1_bytes);
    printSha256(hash1_bytes, sha256.hash_size, full_path1);

    ptr = argv[3];
    debug_winfo(L"file2: %ws\n", ptr);
    memset(full_path2, 0, MAX_PATH*2);
    getFullPathName(ptr, full_path2, &base_name2);
    
    if ( fileExists(full_path2) )
    {
        sha256File(full_path2, hash2_bytes);
        printSha256(hash2_bytes, sha256.hash_size, full_path2);
        if ( memcmp(hash1_bytes, hash2_bytes, sha256.hash_size) == 0 )
        {
            printColored("equal!", FOREGROUND_GREEN);
        }
        else
        {
            printColored("not equal!", FOREGROUND_RED);
        }
    }
    else
    {
        CHAR byte[3];
        SIZE_T n = wcslen(ptr);
        if ( n != SHA256_STR_LN )
        {
            wprintf(L"ERROR: \"%s\" is neither an existing file nor a valid sha256 value!", ptr);
            return -3;
        }

        byte[2] = 0;
        for ( i = 0, j = 0; i < sha256.hash_size; i++, j+=2 )
        {
            byte[0] = (CHAR)ptr[j];
            byte[1] = (CHAR)ptr[j + 1];
            __try {
                hash2_bytes[i] = (BYTE)strtoull(byte, NULL, 16);
            }
            __except ( EXCEPTION_EXECUTE_HANDLER )
            {
                return -1;
            }
        }

        wprintf(L"and\n");
        wprintf(L"%s\n", ptr);
        if ( memcmp(hash1_bytes, hash2_bytes, sha256.hash_size) == 0 )
        {
            printColored("equal!", FOREGROUND_GREEN);
        }
        else
        {
            printColored("not equal!", FOREGROUND_RED);
        }
    }

    return 0;
}

void printColored(CHAR* value, WORD attributes)
{
    HANDLE hStdout;
    WORD wOldColorAttrs;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
    wOldColorAttrs = csbiInfo.wAttributes;
    SetConsoleTextAttribute(hStdout, attributes);
    printf("%s\n", value);
    SetConsoleTextAttribute(hStdout, wOldColorAttrs);
}

int runList(int argc, WCHAR** argv)
{
    int s = 0;
    WCHAR full_path[MAX_PATH];
    WCHAR* base_name = NULL;
    int files_i = 1;
    int i;
    BYTE hash_bytes[32];

    for (i = files_i; i < argc; i++)
    {
        debug_winfo(L"file[%d]: %ws\n", i, argv[i]);
        memset(full_path, 0, MAX_PATH*2);
        getFullPathName(argv[i], full_path, &base_name);
        debug_winfo(L" - full_path: %ws\n", full_path);
        debug_winfo(L" - base_name: %ws\n", base_name);

        if (fileExists(full_path))
        {
            s = sha256File(full_path, hash_bytes);
            printSha256(hash_bytes, sha256.hash_size, full_path);
        }
        else if (dirExists(full_path))
        {
            s = sha256Dir(full_path);
        }
        else
        {
            wprintf(L"ERROR: Path \"%s\" does not exist!", full_path);
        }
    }

    return 0;
}

int sha256File(WCHAR* file, BYTE *hash_bytes)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    FILE* fp = NULL;
    size_t file_size = 0;

    BYTE buffer[BUFFER_SIZE];
    size_t offset = 0;

    int s = 0;
    int errsv;

    s = createHash(&sha256);
    if (s != 0)
        return s;

    //wprintf(L"file: %s\n", file);
    errsv = _wfopen_s(&fp, file, L"rb");
    if (errsv!=0 || fp==NULL)
    {
        wprintf(L"ERROR (0x%x): could not open file!\n", errsv);
        return 1;
    }
    file_size = getFileSize(file);

    size_t bytes_read;
    size_t to_read;
    while (offset < file_size)
    {
        to_read = BUFFER_SIZE;
        if (offset + to_read > file_size)
            to_read = file_size - offset;
        //wprintf(L" - to_read: 0x%x\n", to_read);
        errno = 0;
        bytes_read = fread(buffer, 1, to_read, fp);
        errsv = errno;
        if ((bytes_read == 0 || bytes_read != to_read) && errsv != 0)
        {
            wprintf(L"ERROR (0x%x): Reading bytes failed!\n", errsv);
            goto Cleanup;
        }
        //wprintf(L" - read: 0x%x\n", n);

        status = BCryptHashData(sha256.hash, buffer, (DWORD)bytes_read, 0);
        if (!NT_SUCCESS(status))
        {
            wprintf(L"Error (0x%x): BCryptHashData\n", status);
            goto Cleanup;
        }

        if (to_read < BUFFER_SIZE)
            break;

        offset += to_read;
        // wprintf(L" - 0x%zx / 0x%zx\n", offset, file_size);
    }

    //close the hash
    status = BCryptFinishHash(sha256.hash, hash_bytes, sha256.hash_size, 0);
    if (!NT_SUCCESS(status))
    {
        wprintf(L"Error (0x%x): BCryptFinishHash\n", status);
        goto Cleanup;
    }

    //wprintf(L"Success!\n");

Cleanup:
    if (fp)
    {
        fclose(fp);
    }

    return s;
}

void fileCB(wchar_t* file)
{
    BYTE hash_bytes[32];
    debug_winfo(L"fileCB: %ws\n", file);
    sha256File(file, hash_bytes);
    printSha256(hash_bytes, sha256.hash_size, file);
}

int sha256Dir(WCHAR* dir)
{
    debug_winfo(L"sha256Dir(%s)\n", dir);
    int s = 0;
    FileCallback cb;
    cb = &fileCB;
    //CHAR* types[2] = { ".exe", ".dll" };

    actOnFilesInDir(dir, cb, NULL, true);

    return s;
}

void printSha256(PBYTE bytes, DWORD size, WCHAR* file)
{
    DWORD i;
    wprintf(L"SHA256 of %s:\n", file);
    for (i = 0; i < size; i++)
    {
        wprintf(L"%02x", bytes[i]);
    }
    wprintf(L"\n");
}

int initSha256(PSha256Ctxt ctxt)
{
    int s = 0;
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    HANDLE heap = GetProcessHeap();

    memset(ctxt, 0, sizeof(Sha256Ctxt));

    //open an algorithm handle
    status = BCryptOpenAlgorithmProvider(
        &(ctxt->alg),
        BCRYPT_SHA256_ALGORITHM,
        NULL,
        0);
    if (!NT_SUCCESS(status))
    {
        wprintf(L"Error (0x%x): BCryptOpenAlgorithmProvider\n", status);
        cleanSha256(ctxt);
        return 1;
    }

    //calculate the size of the buffer to hold the hash object
    status = BCryptGetProperty(
        ctxt->alg,
        BCRYPT_OBJECT_LENGTH,
        (PBYTE) & (ctxt->hash_object_size),
        sizeof(DWORD),
        &(ctxt->data_size),
        0);
    if (!NT_SUCCESS(status))
    {
        wprintf(L"Error (0x%x): BCryptGetProperty\n", status);
        cleanSha256(ctxt);
        return 2;
    }

    //wprintf(L"cbHashObject: 0x%lx\n", cbHashObject);
    //wprintf(L"cbData: 0x%lx\n", cbData);

    // allocate the hash object on the heap
    ctxt->hash_object = (PBYTE)HeapAlloc(heap, 0, ctxt->hash_object_size);
    if (NULL == ctxt->hash_object)
    {
        wprintf(L"ERROR: memory allocation failed\n");
        cleanSha256(ctxt);
        return 3;
    }

    // calculate the length of the hash
    status = BCryptGetProperty(
        ctxt->alg,
        BCRYPT_HASH_LENGTH,
        (PBYTE) & (ctxt->hash_size),
        sizeof(DWORD),
        &(ctxt->data_size),
        0);
    if (!NT_SUCCESS(status))
    {
        wprintf(L"Error 0x%x returned by BCryptGetProperty\n", status);
        cleanSha256(ctxt);
        return 4;
    }
    //wprintf(L"cbHash: 0x%lx\n", cbHash);
    //wprintf(L"cbData: 0x%lx\n", cbData);

    
    //wprintf(L"hHash: 0x%p\n", hHash);
    //wprintf(L"pbHashObject: 0x%p\n", pbHashObject);

    return s;
}

int createHash(PSha256Ctxt ctxt)
{
    if (ctxt->hash)
    {
        BCryptDestroyHash(ctxt->hash);
        ctxt->hash = NULL;
    }

    NTSTATUS status = STATUS_UNSUCCESSFUL;
    status = BCryptCreateHash(
        ctxt->alg,
        &(ctxt->hash),
        ctxt->hash_object,
        ctxt->hash_object_size,
        NULL,
        0,
        0);
    if (!NT_SUCCESS(status))
    {
        wprintf(L"Error (0x%x): BCryptCreateHash\n", status);
        cleanSha256(ctxt);
        return 6;
    }
    return status;
}

int cleanSha256(PSha256Ctxt ctxt)
{
    HANDLE heap = GetProcessHeap();

    if (ctxt->alg)
    {
        BCryptCloseAlgorithmProvider(ctxt->alg, 0);
        ctxt->alg = NULL;
    }

    if (ctxt->hash)
    {
        BCryptDestroyHash(ctxt->hash);
        ctxt->hash = NULL;
    }

    if (ctxt->hash_object)
    {
        HeapFree(heap, 0, ctxt->hash_object);
        ctxt->hash_object = NULL;
    }

    return 0;
}
