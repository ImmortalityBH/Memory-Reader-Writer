#include <iostream>
#include <Windows.h>
#include <string>
#include "resource.h"

#define MENU_FILE_TERMINATE_PROCESS 1
#define MENU_FILE_EXIT 2
#define MENU_HELP_ABOUT 3

#define SAVE_PROCESS_INFO 4
#define READ_MEM 5
#define WRITE_MEM 6

HWND procWindow = nullptr;
HANDLE handle = nullptr; //process handle
DWORD procID = 0;
std::wstring procName = L"";
unsigned int address;
int newValue = 0;
int readValue = 0;

HWND hWnd, hEditProcName, hEditNewValue, hEditMemory, hStaticRead;
HMENU hMenu;
HBRUSH hbr;

const int WIDTH = 800, HEIGHT=  400;
LPCWSTR className = L"WindowClass";
LPCWSTR windowTitle = L"Memory Trainer";

void AddMenus(HWND hWnd);
void AddControls(HWND hWnd);
void Terminate_Proc(HANDLE handle, int exitCode);
void CreateShorcuts(HWND hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(wc);
	wc.style = NULL;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	hbr = CreateSolidBrush(RGB(173, 216, 230));
	wc.hbrBackground = hbr;//(HBRUSH)COLOR_WINDOW;	
	wc.lpszClassName = className;
	wc.lpszMenuName = nullptr;
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));;

	if (!RegisterClassEx(&wc))
	{
		MessageBeep(MB_ICONERROR);
		MessageBoxW(nullptr, L"Window registration failed.", L"Error", MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindowW(className, windowTitle, 
		WS_OVERLAPPED| WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX, 
		100, 100, WIDTH, HEIGHT,
		nullptr, nullptr, hInstance, NULL);

	MSG msg = {};

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_HOTKEY:
		if (wParam == MENU_FILE_TERMINATE_PROCESS)
		{
			Terminate_Proc(handle, 0);
		} 
		else if (wParam == MENU_HELP_ABOUT)
		{
			MessageBoxW(hWnd, L"Made by PP Gang 2020", L"Memory Trainer 1.0", MB_OK);
		}
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case MENU_FILE_TERMINATE_PROCESS:
			Terminate_Proc(handle, 0);
			break;
		case MENU_FILE_EXIT:
			DestroyWindow(hWnd);
			break;
		case MENU_HELP_ABOUT:
			MessageBoxW(hWnd, L"Made by PP Gang 2020", L"Memory Trainer 1.0", MB_OK);
			break;
		case SAVE_PROCESS_INFO:
		{
			wchar_t text[100];
			//GetWindowTextW(hEditProcID, text, 100);
			//procName = text;
			//procID = std::stoi(procName);
			GetWindowTextW(hEditMemory, text, 100);
			std::wstring tempMem = text;
			if (tempMem.empty()) 
			{
				address = 0;
			}
			else
			{
				address = std::stoi(tempMem);
			}
			
			GetWindowTextW(hEditProcName, text, 100);
			procName = text;
			
			std::wstring msgBoxCaption = L"ProcName: ";
			msgBoxCaption.append(procName);
			

			
			procWindow = FindWindowW(nullptr, procName.c_str());
			if (procWindow == nullptr)
			{
				MessageBeep(MB_ICONERROR);
				MessageBoxW(nullptr, L"Operation failed. Please try again.", L"Error", MB_ICONERROR);
			}
			else if (address == 0)
			{
				MessageBeep(MB_ICONERROR);
				MessageBoxW(nullptr, L"Please enter a valid memory location.", L"Error", MB_ICONERROR);
			}
			else
			{
				GetWindowThreadProcessId(procWindow, &procID);
				msgBoxCaption.append(L", ProcID: ");
				msgBoxCaption.append(std::to_wstring(procID));
				handle = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_TERMINATE, FALSE, procID);
				MessageBeep(MB_OK);
				MessageBoxW(nullptr, msgBoxCaption.c_str(), L"Proccess Information Updated.", MB_OK);
			}
			
			break;
		}
		case READ_MEM:
		{
			//HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
			if (procID == NULL)
			{
				MessageBeep(MB_ICONERROR);
				MessageBoxW(nullptr, L"Operation failed. ProcID is NULL", L"Error", MB_ICONERROR);
			}
			else
			{
				if (ReadProcessMemory(handle, reinterpret_cast<PBYTE*>(address), &readValue, sizeof(int), 0))
				{
					std::wstring readValueStr = L"Read value: ";
					readValueStr.append(std::to_wstring(readValue));
					SetWindowTextW(hStaticRead, readValueStr.c_str());
					MessageBeep(MB_OK);
					MessageBoxW(nullptr, L"Operation successful.", L"Success", MB_OK);
				}
				else
				{
					MessageBeep(MB_ICONERROR);
					MessageBoxW(nullptr, L"Reading operation failed.", L"Error", MB_ICONERROR);
				}
			}
			break;
		}
		case WRITE_MEM:
		{
			//HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

			wchar_t text[100];
			GetWindowTextW(hEditNewValue, text, 100);
			std::wstring tempValue = text;
			newValue = std::stoi(tempValue);

			if (procID == NULL)
			{
				MessageBeep(MB_ICONERROR);
				MessageBoxW(nullptr, L"Operation failed. ProcID is NULL", L"Error", MB_ICONERROR);
			}
			else
			{
				if (WriteProcessMemory(handle, reinterpret_cast<LPVOID>(address), &newValue, sizeof(newValue), 0))
				{
					MessageBeep(MB_OK);
					MessageBoxW(nullptr, L"Operation successful.", L"Success", MB_OK);
				}
				else
				{
					MessageBeep(MB_ICONERROR);
					MessageBoxW(nullptr, L"Writing operation failed.", L"Error", MB_ICONERROR);
				}
			}
			break;
		}
			
		}
		break;
	case WM_CREATE:
		CreateShorcuts(hWnd);
		AddMenus(hWnd);
		AddControls(hWnd);
		break;
	case WM_DESTROY:
		DeleteObject(hbr);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
}

void AddMenus(HWND hWnd)
{
	hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();
	HMENU hHelpMenu = CreateMenu();

	AppendMenu(hFileMenu, MF_STRING, MENU_FILE_TERMINATE_PROCESS, L"&Terminate Process\tAlt + X");
	RegisterHotKey(hWnd, MENU_FILE_TERMINATE_PROCESS, MOD_ALT | MOD_NOREPEAT, 0x58u);
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, nullptr);
	AppendMenu(hFileMenu, MF_STRING, MENU_FILE_EXIT, L"&Exit\tAlt + F4");
	AppendMenu(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hFileMenu), L"File");

	AppendMenu(hHelpMenu, MF_STRING, MENU_HELP_ABOUT, L"&About\tCtrl + F1");
	RegisterHotKey(hWnd, MENU_HELP_ABOUT, MOD_CONTROL | MOD_NOREPEAT, VK_F1);
	AppendMenu(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hHelpMenu), L"Help");

	SetMenu(hWnd, hMenu);
}

void AddControls(HWND hWnd)
{
	//labels
	CreateWindowW(L"Static", L"Process Name:", WS_VISIBLE | WS_CHILD, 25, 25, 65, 30, hWnd, nullptr, nullptr, nullptr);
	CreateWindowW(L"Static", L"Memory Location", WS_VISIBLE | WS_CHILD, 25, 25 + 65, 65, 30, hWnd, nullptr, nullptr, nullptr);
	hStaticRead = CreateWindowW(L"Static", L" Read value: ", WS_VISIBLE | WS_CHILD, 350, 25, 125, 30, hWnd, nullptr, nullptr, nullptr);
	CreateWindowW(L"Static", L"Write value: ", WS_VISIBLE | WS_CHILD | SS_CENTER, 550, 25, 80, 30, hWnd, nullptr, nullptr, nullptr);

	//textfields
	hEditProcName =  CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 100, 25, 100, 30, hWnd, nullptr, nullptr, nullptr);
	hEditMemory = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 100, 25 + 65, 100, 30, hWnd, nullptr, nullptr, nullptr);
	hEditNewValue = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 650, 25, 100, 30, hWnd, nullptr, nullptr, nullptr);
	//buttons
	CreateWindowW(L"Button", L"Save", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 25 + 65 + 65, 100, 35, hWnd, reinterpret_cast<HMENU>(SAVE_PROCESS_INFO), nullptr, nullptr);
	CreateWindowW(L"Button", L"Read", WS_VISIBLE | WS_CHILD | WS_BORDER, 350, 25 + 65, 100, 35, hWnd, reinterpret_cast<HMENU>(READ_MEM), nullptr, nullptr);
	CreateWindowW(L"Button", L"Write", WS_VISIBLE | WS_CHILD | WS_BORDER, 650, 25 + 65, 100, 35, hWnd, reinterpret_cast<HMENU>(WRITE_MEM), nullptr, nullptr);
}

void Terminate_Proc(HANDLE handle, int exitCode)
{
	if (handle == nullptr)
	{
		//do nothing
	}
	else
	{
		if (!TerminateProcess(handle, exitCode)) {
			MessageBeep(MB_ICONERROR);
			MessageBoxW(nullptr, L"Process termination unsuccessful.", L"Error", MB_ICONERROR);
		}
	}
}

void CreateShorcuts(HWND hWnd)
{
	RegisterHotKey(hWnd, MENU_FILE_TERMINATE_PROCESS, MOD_ALT | MOD_NOREPEAT, 0x58u);
	RegisterHotKey(hWnd, MENU_HELP_ABOUT, MOD_CONTROL | MOD_NOREPEAT, VK_F1);
}
