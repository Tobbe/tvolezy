#ifndef VOL_VISTA_H_
#define VOL_VISTA_H_

#include "volume.h"
#include "tvesettings.h"
#include <mmdeviceapi.h>
#include <endpointvolume.h>

class VolVista : public Volume
{
private:
	HRESULT hr;
	IAudioEndpointVolume *endpointVolume;

	void change(int steps);

public:
	VolVista(const TveSettings &settings);
	~VolVista();
	bool up(int steps);
	bool down(int steps);
	bool toggleMute();
	int getError() const;
};

#endif