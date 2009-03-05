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

	void setupMixerStructs();
	void setupMixerControlDetails(HMIXER &mixer, MIXERLINECONTROLS &mlc, MIXERCONTROLDETAILS &mcd);
	void change(int steps);
	void reportError(LPCSTR msg);
	bool isMuted();
	void setMuted(bool mute);

public:
	Volume(const TveSettings &settings);
	void up(int steps);
	void down(int steps);
	void toggleMute();
};

#endif