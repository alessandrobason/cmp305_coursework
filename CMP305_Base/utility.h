#pragma once

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(ptr) if(ptr) { ptr->Release(); ptr = nullptr; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) if(ptr) { delete ptr; ptr = nullptr; }
#endif

#ifndef ARR_LEN
#define ARR_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

bool fileExists(const char *filename);
wchar_t *wstrFromStr(const char *str, size_t len = 0);