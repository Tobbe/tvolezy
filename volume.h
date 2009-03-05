#ifndef VOLUME_H_
#define VOLUME_H_

#include "tvesettings.h"
#include <windows.h>

class Volume
{
private:
	MIXERLINE ml;
	MIXERCONTROL mc;
	MIXERLINECONTROLS mlcVol;
	MIXERLINECONTROLS mlcMute;
	MIXERCONTROLDETAILS mcdVol;
	MIXERCONTROLDETAILS mcdMute;
	MIXERCONTROLDETAILS_UNSIGNED mcdu;
	MIXERCONTROLDETAILS_BOOLEAN mcdb;
	const TveSettings &settings;
	int error;

	void setupMixerStructs();
	void setupMixerControlDetails(HMIXER &mixer, MIXERLINECONTROLS &mlc, MIXERCONTROLDETAILS &mcd);
	void change(int steps);
	bool isMuted();
	void setMuted(bool mute);

public:
	enum {ERROR_NOERROR,
		ERROR_OPENMIXER, 
		ERROR_LINEINFO, 
		ERROR_LINECONTROLS,
		ERROR_CONTROLDETAILS,
		ERROR_SETDETAILS};

	Volume(const TveSettings &settings);
	bool up(int steps);
	bool down(int steps);
	bool toggleMute();
	int getError() const;
};

#endif