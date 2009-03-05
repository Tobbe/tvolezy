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

void __cdecl bangVolUp(HWND caller, const char *args);
void __cdecl bangVolDown(HWND caller, const char *args);
void __cdecl bangToggleMute(HWND caller, const char *args);
void reportVolumeError();
void reportError(LPCSTR msg);
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

// Actual main function
extern "C" int __cdecl initModuleEx(HWND parentWnd, HINSTANCE dllInst, LPCSTR szPath)
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
	wc.style = CS_NOCLOSE;
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
		UnregisterClass(className, dllInst);
		return 1;
	}

	// Register our bangs with LiteStep
	AddBangCommand("!tVolEzyUp", bangVolUp);
	AddBangCommand("!tVolEzyDown", bangVolDown);
	AddBangCommand("!tVolEzyToggleMute", bangToggleMute);

	// Register message for version info
	UINT msgs[] = {LM_GETREVID, 0};
	SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)hWnd, (LPARAM)msgs);

	return 0;
}

void __cdecl bangVolUp(HWND caller, const char* args)
{
	int steps = 0;
	if (args && args[0] != '\0')
	{
		steps = atoi(args);
	}

	if (!vol.up(steps))
	{
		reportVolumeError();
	}
}

void __cdecl bangVolDown(HWND caller, const char* args)
{
	int steps = 0;
	if (args && args[0] != '\0')
	{
		steps = atoi(args);
	}

	if (!vol.down(steps))
	{
		reportVolumeError();
	}
}

void __cdecl bangToggleMute(HWND caller, const char* args)
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

		case WM_DESTROY:
			hWnd = NULL;
			return 0;

		default:
			break;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

// -------------------------------------------------------
// cleanup (opposite of init())
extern "C" void __cdecl quitModule(HINSTANCE dllInst)
{
	RemoveBangCommand("!tVolEzyUp");
	RemoveBangCommand("!tVolEzyDown");
	RemoveBangCommand("!tVolEzyToggleMute");

	if (hWnd != NULL)
	{
		UINT msgs[] = {LM_GETREVID, 0};
		SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)hWnd, (LPARAM)msgs);

		DestroyWindow(hWnd);
	}

	UnregisterClass(className, dllInst);
}