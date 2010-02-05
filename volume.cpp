#include "volume.h"

Volume::Volume(const TveSettings &settings) :
	settings(settings), error(ERROR_NOERROR), volChangedCallback(0), muteChangedCallback(0)
{
}

Volume::~Volume()
{
}

void Volume::setVolChangedCallback(void (*volChangedCallback)(int))
{
	this->volChangedCallback = volChangedCallback;
}

void Volume::setMuteChangedCallback(void (*muteChangedCallback)(bool))
{
	this->muteChangedCallback = muteChangedCallback;
}