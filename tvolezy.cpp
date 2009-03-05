#define STRICT
#define NOCRYPT

#include <windows.h>
#include "tvolezy.h"
#include "volume.h"

HWND hWnd;
HWND hWndParent;
LPCSTR className = "tVolEzyWndClass";
LPCSTR revID = "tVolEzy 0.1 by Tobbe";
TveSettings settings;
Volume vol(settings);

void bangVol(HWND caller, const char *bangName, const char *args);
void bangToggleMute(HWND caller, const char *bangName, const char *args);
void reportVolumeError();
void reportError(LPCSTR msg);
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

// Actual main function
extern "C" int initModuleEx(HWND parentWnd, HINSTANCE dllInst, LPCSTR szPath)
{
	szPath=szPath;
	hWndParent = parentWnd;

	// Get tVolEzy settings
	settings.showErrors = GetRCBoolDef("tVolEzyShowErrors", TRUE) == TRUE;
	settings.unmuteOnVolUp = GetRCBoolDef("tVolEzyUnmuteOnVolUp", TRUE) == TRUE;
	settings.unmuteOnVolDown = GetRCBoolDef("tVolEzyUnmuteOnVolDown", FALSE) == TRUE;

	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = (WNDPROC)wndProc;
	wc.hInstance = dllInst;
	wc.lpszClassName = className;
	if (!RegisterClass(&wc))
	{
		reportError("Error registering tVolEzy window class");
		return 1;
	}

	hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, className, "", WS_CHILD,
		0, 0, 0, 0, hWndParent, NULL, dllInst, NULL);
	if (hWnd == NULL)
	{
		reportError("Error creating tVolEzy window");
		UnregisterClass("", dllInst);
		return 1;
	}

	// Register our bangs with LiteStep
	AddBangCommandEx("!tVolEzyUp", bangVol);
	AddBangCommandEx("!tVolEzyDown", bangVol);
	AddBangCommandEx("!tVolEzyToggleMute", bangToggleMute);

	// Register message for version info
	UINT msgs[] = {LM_GETREVID, 0};
	SendMessage(hWndParent, LM_REGISTERMESSAGE, (WPARAM)hWnd, (LPARAM)msgs);

	return 0;
}

void bangVol(HWND caller, const char* bangName, const char* args)
{
	int steps = 0;
	if (args && args[0] != '\0')
	{
		steps = atoi(args);
	}

	if (bangName[8] == 'D' || bangName[8] == 'd')
	{
		if (!vol.down(steps))
		{
			reportVolumeError();
		}
	}
	else
	{
		if (!vol.up(steps))
		{
			reportVolumeError();
		}
	}
}

void bangToggleMute(HWND caller, const char* bangName, const char* args)
{
	if(!vol.toggleMute())
	{
		reportVolumeError();
	}
}

void reportVolumeError()
{
	switch (vol.getError())
	{
		case Volume::ERROR_OPENMIXER:
			reportError("Could not open mixer");
			break;
		case Volume::ERROR_LINEINFO:
			reportError("Could not get line info");
			break;
		case Volume::ERROR_LINECONTROLS:
			reportError("Could not get line controls");
			break;
		case Volume::ERROR_CONTROLDETAILS:
			reportError("Could not get control details");
			break;
		case Volume::ERROR_SETDETAILS:
			reportError("Could not set control details");
			break;
		default:
			break;
	}
}

void reportError(LPCSTR msg)
{
	if (settings.showErrors)
	{
		MessageBox(NULL, msg, "tVolEzy error", MB_OK | MB_ICONERROR);
	}
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
		case LM_GETREVID:
			lstrcpy((LPSTR)lParam, revID);
			return strlen((LPTSTR)lParam);

		case WM_CLOSE:
			return 0;

		case WM_SYSCOMMAND:
			switch (wParam){
				case SC_CLOSE:
					return 0;
				default:
					break;
			}
			break;

		case WM_DESTROY:
			hWnd = NULL;
			return 0;

		case WM_APPCOMMAND:
			MessageBox(hWndParent, "Error registering tVolEzy window class", "tVolEzy error", MB_OK);
			return 1;

		default:
			break;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

// -------------------------------------------------------
// cleanup (opposite of init())
extern "C" void quitModule(HINSTANCE dllInst)
{
	RemoveBangCommand("!tVolEzyUp");
	RemoveBangCommand("!tVolEzyDown");
	RemoveBangCommand("!tVolEzyToggleMute");
}