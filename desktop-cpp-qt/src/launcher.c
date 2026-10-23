#include <windows.h>
#include <shlobj.h>

static BOOL writeBinaryFile(const WCHAR* path, const char* data, DWORD size) {
    HANDLE hFile = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return FALSE;
    DWORD written = 0;
    BOOL ok = WriteFile(hFile, data, size, &written, NULL);
    CloseHandle(hFile);
    return ok && (written == size);
}

static DWORD readStamp(const WCHAR* path) {
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return 0;
    DWORD stamp = 0, read = 0;
    ReadFile(hFile, &stamp, sizeof(stamp), &read, NULL);
    CloseHandle(hFile);
    return read == sizeof(stamp) ? stamp : 0;
}

static void writeStamp(const WCHAR* path, DWORD stamp) {
    HANDLE hFile = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(hFile, &stamp, sizeof(stamp), &written, NULL);
        CloseHandle(hFile);
    }
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    WCHAR localApp[MAX_PATH];
    if (FAILED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, localApp))) {
        return 1;
    }

    WCHAR targetDir[MAX_PATH];
    WCHAR targetExe[MAX_PATH];
    WCHAR stampFile[MAX_PATH];
    WCHAR zipPath[MAX_PATH];
    WCHAR cmd[MAX_PATH * 3];

    wsprintfW(targetDir, L"%s\\FinansAsistani_app", localApp);
    wsprintfW(targetExe, L"%s\\FinansAsistani.exe", targetDir);
    wsprintfW(stampFile, L"%s\\.version", targetDir);
    wsprintfW(zipPath, L"%s\\payload.zip", targetDir);

    HRSRC hRes = FindResourceW(NULL, MAKEINTRESOURCEW(101), MAKEINTRESOURCEW(10));
    if (!hRes) {
        MessageBoxW(NULL, L"Gömülü uygulama verisi bulunamadı!", L"Hata", MB_ICONERROR);
        return 1;
    }

    DWORD size = SizeofResource(NULL, hRes);
    HGLOBAL hData = LoadResource(NULL, hRes);
    const char* pData = (const char*)LockResource(hData);
    if (!pData || size == 0) {
        MessageBoxW(NULL, L"Kaynak verisi okunamadı!", L"Hata", MB_ICONERROR);
        return 1;
    }

    DWORD existingStamp = readStamp(stampFile);
    DWORD exeAttr = GetFileAttributesW(targetExe);
    BOOL needExtract = (exeAttr == INVALID_FILE_ATTRIBUTES) || (size != existingStamp);

    if (needExtract) {
        CreateDirectoryW(targetDir, NULL);
        if (writeBinaryFile(zipPath, pData, size)) {
            wsprintfW(cmd, L"tar.exe -xf \"%s\" -C \"%s\"", zipPath, targetDir);
            STARTUPINFOW si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_HIDE;
            PROCESS_INFORMATION pi;
            ZeroMemory(&pi, sizeof(pi));

            if (CreateProcessW(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            DeleteFileW(zipPath);
            writeStamp(stampFile, size);
        }
    }

    // Hedef uygulamayı başlat
    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    WCHAR cmdLine[MAX_PATH * 2];
    wsprintfW(cmdLine, L"\"%s\"", targetExe);

    if (!CreateProcessW(targetExe, cmdLine, NULL, NULL, FALSE, 0, NULL, targetDir, &si, &pi)) {
        MessageBoxW(NULL, L"Finans Asistanı başlatılamadı!", L"Hata", MB_ICONERROR);
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
