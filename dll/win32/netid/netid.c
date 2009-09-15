/*
 * PROJECT:     ReactOS System Control Panel Applet
 * LICENSE:     GPL - See COPYING in the top level directory
 * FILE:        dll/win32/netid/netid.c
 * PURPOSE:     Network ID Page
 * COPYRIGHT:   Thomas Weidenmueller <w3seek@reactos.org>
 *              Dmitry Chapyshev <dmitry@reactos.org>
 *
 */

#include <windows.h>
#include <tchar.h>
#include <lm.h>
#include <prsht.h>
#include "resource.h"

static HINSTANCE hDllInstance;


static
INT_PTR CALLBACK
DNSSuffixPropDlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDCANCEL:
                case IDOK:
                    EndDialog(hDlg, LOWORD(wParam));
                    break;
            }
        }
        break;
    }

    return FALSE;
}

static VOID
SetRadioBtnState(HWND hDlg, BOOL IsDomain)
{
    SendDlgItemMessage(hDlg, 1008, BM_SETCHECK, (WPARAM)IsDomain, 0);
    SendDlgItemMessage(hDlg, 1004, BM_SETCHECK, (WPARAM)!IsDomain, 0);
    EnableWindow(GetDlgItem(hDlg, 116), IsDomain);
    EnableWindow(GetDlgItem(hDlg, 1007), !IsDomain);
}

static VOID
DisableControls(HWND hDlg)
{
    EnableWindow(GetDlgItem(hDlg, 1008), FALSE);
    EnableWindow(GetDlgItem(hDlg, 1004), FALSE);
    EnableWindow(GetDlgItem(hDlg, 116), FALSE);
    EnableWindow(GetDlgItem(hDlg, 1007), FALSE);
}

static
INT_PTR CALLBACK
NetworkPropDlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
        case WM_INITDIALOG:
        {
            LPWKSTA_INFO_101 wki = NULL;
            DWORD Size = MAX_COMPUTERNAME_LENGTH + 1;
            TCHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1];
            TCHAR MsgText[MAX_PATH * 2];
            LPWSTR JoinName;
            NETSETUP_JOIN_STATUS JoinStatus;

            if (LoadString(hDllInstance, 25, MsgText, sizeof(MsgText) / sizeof(TCHAR)))
                SetDlgItemText(hDlg, 1017, MsgText);

            if (GetComputerName(ComputerName, &Size))
            {
                SetDlgItemText(hDlg, 1002, ComputerName);
                SetDlgItemText(hDlg, 1001, ComputerName);
            }

            if (NetGetJoinInformation(NULL, &JoinName, &JoinStatus) != NERR_Success)
            {
                SetRadioBtnState(hDlg, FALSE);

                if (NetWkstaGetInfo(NULL,
                                    101,
                                    (LPBYTE*)&wki) == NERR_Success)
                {
                    SetDlgItemText(hDlg,
                                   1007,
                                   wki->wki101_langroup);
                }
                else DisableControls(hDlg);

                if (wki) NetApiBufferFree(wki);
            }
            else
            {
                switch (JoinStatus)
                {
                    case NetSetupDomainName:
                        SetDlgItemText(hDlg, 116, JoinName);
                        SetRadioBtnState(hDlg, TRUE);
                        break;

                    case NetSetupWorkgroupName:
                        SetDlgItemText(hDlg, 1007, JoinName);
                        SetRadioBtnState(hDlg, FALSE);
                        break;

                    case NetSetupUnjoined:
                        break;

                    case NetSetupUnknownStatus:
                    default:
                    {
                        SetRadioBtnState(hDlg, FALSE);

                        if (NetWkstaGetInfo(NULL,
                                            101,
                                            (LPBYTE*)&wki) == NERR_Success)
                        {
                            SetDlgItemText(hDlg,
                                           1007,
                                           wki->wki101_langroup);
                        }
                        else DisableControls(hDlg);

                        if (wki) NetApiBufferFree(wki);
                        break;
                    }
                }

                if (JoinName) NetApiBufferFree(JoinName);
            }
        }
        break;

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case 1008: /* Domain radio button */
                case 1004: /* Workgroup radio button */
                    if (SendDlgItemMessage(hDlg, 1008, BM_GETCHECK, 0, 0) == BST_CHECKED)
                        SetRadioBtnState(hDlg, TRUE);
                    else
                        SetRadioBtnState(hDlg, FALSE);
                    break;

                case 1003:
                    DialogBox(hDllInstance,
                              MAKEINTRESOURCE(IDD_PROPPAGEDNSANDNETBIOS),
                              hDlg,
                              DNSSuffixPropDlgProc);
                    break;

                case IDOK:
                {
                    DWORD Size = MAX_COMPUTERNAME_LENGTH + 1;
                    TCHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1];
                    TCHAR NewComputerName[MAX_COMPUTERNAME_LENGTH + 1];

                    if (GetComputerName(ComputerName, &Size))
                    {
                        if (GetWindowText(GetDlgItem(hDlg, 1002),
                                          NewComputerName,
                                          (MAX_COMPUTERNAME_LENGTH + 1)))
                        {
                            if (_tcscmp(ComputerName, NewComputerName) != 0)
                            {
                                if (!SetComputerName(NewComputerName))
                                {
                                    TCHAR szMsgText[MAX_PATH];

                                    LoadString(hDllInstance, 4001, szMsgText, sizeof(szMsgText) / sizeof(TCHAR));

                                    MessageBox(0, szMsgText, NULL, MB_OK | MB_ICONERROR);
                                }
                                else
                                {
                                    TCHAR szMsgTitle[MAX_PATH], szMsgText[MAX_PATH];

                                    LoadString(hDllInstance, 4000, szMsgTitle, sizeof(szMsgTitle) / sizeof(TCHAR));
                                    LoadString(hDllInstance, 24, szMsgText, sizeof(szMsgText) / sizeof(TCHAR));

                                    MessageBox(hDlg, szMsgText, szMsgTitle, MB_OK | MB_ICONINFORMATION);
                                }
                            }
                        }
                    }

                    EndDialog(hDlg, LOWORD(wParam));
                }
                break;

                case IDCANCEL:
                    EndDialog(hDlg, LOWORD(wParam));
                    break;
            }
        }
        break;
    }

    return FALSE;
}

static INT_PTR CALLBACK
NetIDPageProc(IN HWND hwndDlg,
              IN UINT uMsg,
              IN WPARAM wParam,
              IN LPARAM lParam)
{
    INT_PTR Ret = 0;

    UNREFERENCED_PARAMETER(lParam);

    switch (uMsg)
    {
        case WM_INITDIALOG:
        {
            /* Display computer name */
            LPWKSTA_INFO_101 wki = NULL;
            DWORD Size = MAX_COMPUTERNAME_LENGTH + 1;
            TCHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1];
            if (GetComputerName(ComputerName,&Size))
            {
                SetDlgItemText(hwndDlg,
                               IDC_COMPUTERNAME,
                               ComputerName);
            }
            if (NetWkstaGetInfo(NULL,
                                101,
                                (LPBYTE*)&wki) == NERR_Success)
            {
                SetDlgItemText(hwndDlg,
                               IDC_WORKGROUPDOMAIN_NAME,
                               wki->wki101_langroup);
            }

            if (wki) NetApiBufferFree(wki);

            Ret = TRUE;
            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_NETWORK_PROPERTY:
                    DialogBox(hDllInstance,
                              MAKEINTRESOURCE(IDD_PROPPAGECOMPNAMECHENGE),
                              hwndDlg,
                              NetworkPropDlgProc);
                break;
            }
            break;
    }

    return Ret;
}

HPROPSHEETPAGE WINAPI
CreateNetIDPropertyPage(VOID)
{
    PROPSHEETPAGE psp = {0};

    psp.dwSize = sizeof(psp);
    psp.dwFlags = PSP_DEFAULT;
    psp.hInstance = hDllInstance;
    psp.pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGECOMPUTER);
    psp.pfnDlgProc = NetIDPageProc;

    return CreatePropertySheetPage(&psp);
}

BOOL WINAPI
DllMain(IN HINSTANCE hinstDLL,
        IN DWORD dwReason,
        IN LPVOID lpvReserved)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            hDllInstance = hinstDLL;
            DisableThreadLibraryCalls(hinstDLL);
            break;
    }

    return TRUE;
}
