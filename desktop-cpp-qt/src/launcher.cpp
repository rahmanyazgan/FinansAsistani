#include <windows.h>
#include <shlobj.h>
#include <string>

static bool writeBinaryFile(const std::wstring& path, const char* data, DWORD size) {
    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    BOOL ok = WriteFile(hFile, data, size, &written, NULL);
    CloseHandle(hFile);
    return ok && (written == size);
}

static DWORD readStamp(const std::wstring& path) {
    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return 0;
    DWORD stamp = 0, read = 0;
    ReadFile(hFile, &stamp, sizeof(stamp), &read, NULL);
    CloseHandle(hFile);
    return read == sizeof(stamp) ? stamp : 0;
}

static void writeStamp(const std::wstring& path, DWORD stamp) {
    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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

    std::wstring targetDir = std::wstring(localApp) + L"\\FinansAsistani_app";
    std::wstring targetExe = targetDir + L"\\FinansAsistani.exe";
    std::wstring stampFile = targetDir + L"\\.version";

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
    DWORD exeAttr = GetFileAttributesW(targetExe.c_str());
    bool needExtract = (exeAttr == INVALID_FILE_ATTRIBUTES) || (size != existingStamp);

    if (needExtract) {
        CreateDirectoryW(targetDir.c_str(), NULL);
        std::wstring zipPath = targetDir + L"\\payload.zip";
        if (writeBinaryFile(zipPath, pData, size)) {
            std::wstring cmd = L"tar.exe -xf \"" + zipPath + L"\" -C \"" + targetDir + L"\"";
            STARTUPINFOW si = { sizeof(si) };
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_HIDE;
            PROCESS_INFORMATION pi;
            if (CreateProcessW(NULL, &cmd[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            DeleteFileW(zipPath.c_str());
            writeStamp(stampFile, size);
        }
    }

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    std::wstring cmdLine = L"\"" + targetExe + L"\"";

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv && argc > 1) {
        for (int i = 1; i < argc; ++i) {
            cmdLine += L" \"";
            cmdLine += argv[i];
            cmdLine += L"\"";
        }
    }
    if (argv) LocalFree(argv);

    if (!CreateProcessW(targetExe.c_str(), &cmdLine[0], NULL, NULL, FALSE, 0, NULL, targetDir.c_str(), &si, &pi)) {
        MessageBoxW(NULL, L"Finans Asistanı başlatılamadı!", L"Hata", MB_ICONERROR);
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
