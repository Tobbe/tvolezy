#ifndef VOL_XP_H_
#define VOL_XP_H_

#include "tvesettings.h"
#include "volume.h"
#include <windows.h>

class VolXP : public Volume
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

	void setupMixerStructs();
	void setupMixerControlDetails(HMIXER &mixer, MIXERLINECONTROLS &mlc, MIXERCONTROLDETAILS &mcd);
	void change(int steps);
	bool isMuted();
	void setMuted(bool mute);

public:
	VolXP(const TveSettings &settings);
	bool up(int steps);
	bool down(int steps);
	bool toggleMute();
	int getError() const;
};

#endif