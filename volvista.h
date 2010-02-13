#ifndef VOL_VISTA_H_
#define VOL_VISTA_H_

#include "volume.h"
#include "tvesettings.h"
#include <mmdeviceapi.h>
#include <endpointvolume.h>

class VolVista : public Volume
{
private:
	class VolumeNotification : public IAudioEndpointVolumeCallback
	{
	private:
		LONG refCount; 
		int currentVolume;
		const VolVista &parent;
	protected:
		~VolumeNotification();
	public:
		VolumeNotification(const VolVista& outer);
		STDMETHODIMP_(ULONG)AddRef();
		STDMETHODIMP_(ULONG)Release(); 
		STDMETHODIMP QueryInterface(REFIID IID, void **returnValue);
		STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA notificationData);
	};

	HRESULT hr;
	IAudioEndpointVolume *endpointVolume;
	VolumeNotification *volNotification;

	void change(int steps);
	void init();
	void checkAudioEndpointResultForError(HRESULT hr);
	void checkActivateResultForError(HRESULT hr);

public:
	VolVista(const TveSettings &settings);
	~VolVista();
	bool up(int steps);
	bool down(int steps);
	bool toggleMute();
	int getError() const;
};

#endif