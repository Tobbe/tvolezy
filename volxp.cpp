#include "volxp.h"
#include "tvesettings.h"
#include <windows.h>

VolXP::VolXP(const TveSettings &settings) : settings(settings)
{
	error = ERROR_NOERROR;
	setupMixerStructs();
}

void VolXP::setupMixerStructs()
{
	// General
	ZeroMemory(&ml, sizeof(ml));
	ml.cbStruct = sizeof(MIXERLINE);
	ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

	ZeroMemory(&mc, sizeof(mc));

	// Volume
	ZeroMemory(&mlcVol, sizeof(mlcVol));
	mlcVol.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mlcVol.cbStruct = sizeof(MIXERLINECONTROLS);
	mlcVol.dwLineID = ml.dwLineID;
	mlcVol.cControls = ml.cControls;
	mlcVol.cbmxctrl = sizeof(MIXERCONTROL);
	mlcVol.pamxctrl = &mc;

	ZeroMemory(&mcdVol, sizeof(mcdVol));
	ZeroMemory(&mcdu, sizeof(mcdu));
	mcdVol.paDetails = &mcdu;
	mcdVol.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mcdVol.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mcdVol.dwControlID = mlcVol.dwControlID;
	mcdVol.cChannels = 1;
	mcdVol.cMultipleItems = 0;

	// Mute
	memcpy(&mlcMute, &mlcVol, sizeof(mlcMute));
	mlcMute.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;

	memcpy(&mcdMute, &mcdVol, sizeof(mcdMute));
	ZeroMemory(&mcdb, sizeof(mcdb));
	mcdMute.paDetails = &mcdb;
	mcdMute.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
}

void VolXP::setupMixerControlDetails(HMIXER &mixer, MIXERLINECONTROLS &mlc, MIXERCONTROLDETAILS &mcd)
{
	if (mixerOpen(&mixer, 0, NULL, 0, MIXER_OBJECTF_HMIXER) != MMSYSERR_NOERROR)
	{
		error = ERROR_OPENMIXER;
		return;
	}

	if (mixerGetLineInfo((HMIXEROBJ)mixer, &ml, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
	{
		error = ERROR_LINEINFO;
		return;
	}

	mlc.dwLineID = ml.dwLineID;

	if (mixerGetLineControls((HMIXEROBJ)mixer, &mlc,
		MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) !=
		MMSYSERR_NOERROR)
	{
		error = ERROR_LINECONTROLS;
		return;
	}

	mcd.dwControlID = mc.dwControlID;

	if (mixerGetControlDetails((HMIXEROBJ)mixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		error = ERROR_CONTROLDETAILS;
		return;
	}
}

void VolXP::change(int steps)
{
	HMIXER mixer;
	setupMixerControlDetails(mixer, mlcVol, mcdVol);

	//full volume at 65535
	mcdu.dwValue = max(0, min((long)mcdu.dwValue + steps, 65535));

	if (mixerSetControlDetails((HMIXEROBJ)mixer, &mcdVol, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		error = ERROR_SETDETAILS;
		return;
	}

	mixerClose(mixer);
}

bool VolXP::isMuted()
{
	HMIXER mixer;
	setupMixerControlDetails(mixer, mlcMute, mcdMute);

	return mcdb.fValue != 0;
}

void VolXP::setMuted(bool mute)
{
	HMIXER mixer;
	setupMixerControlDetails(mixer, mlcMute, mcdMute);

	mcdb.fValue = mute;

	if (mixerSetControlDetails((HMIXEROBJ)mixer, &mcdMute, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		error = ERROR_SETDETAILS;
		return;
	}

	mixerClose(mixer);
}

bool VolXP::up(int steps)
{
	change(steps);

	if (settings.unmuteOnVolUp)
	{
		setMuted(false);
	}

	return error == ERROR_NOERROR;
}

bool VolXP::down(int steps)
{
	change(-steps);

	if (settings.unmuteOnVolDown)
	{
		setMuted(false);
	}

	return error == ERROR_NOERROR;
}

bool VolXP::toggleMute()
{
	setMuted(!isMuted());

	return error == ERROR_NOERROR;
}

int VolXP::getError() const
{
	return error;
}
