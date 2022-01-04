#include "utility.h"

#include <string.h>

#define VC_EXTRALEAN
#include <windows.h>

bool fileExists(const char *filename) {
    DWORD dwAttrib = GetFileAttributesA(filename);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

wchar_t *wstrFromStr(const char *str, size_t len) {
    if (len == 0) len = strlen(str);

    // get size first
    int finalLen = MultiByteToWideChar(
        CP_ACP, 0,
        str, (int)len,
        NULL, 0
    );

    wchar_t *finalStr = new wchar_t[finalLen + 1];
    if (!finalStr) return nullptr;

    finalLen = MultiByteToWideChar(
        CP_ACP, 0,
        str, (int)len,
        finalStr, finalLen
    );
    finalStr[finalLen] = '\0';

    return finalStr;
}
