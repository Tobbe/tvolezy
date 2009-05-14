#include "volvista.h"
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <stdio.h>

VolVista::VolVista(const TveSettings &settings) : Volume(settings)
{
	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	endpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	defaultDevice->Release();
	defaultDevice = NULL; 
}

VolVista::~VolVista()
{
	if (endpointVolume)
	{
		endpointVolume->Release();
	}

	CoUninitialize();
}

void VolVista::change(int steps)
{
	steps;
}

bool VolVista::up(int steps)
{
	change(steps);
	return true;
}

bool VolVista::down(int steps)
{
	change(-steps);
	return true;
}

bool VolVista::toggleMute()
{
	return true;
}

int VolVista::getError() const
{
	return ERROR_NOERROR;
}