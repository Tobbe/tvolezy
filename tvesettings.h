#ifndef TVE_SETTINGS_H_
#define TVE_SETTINGS_H_

#include <string>

struct TveSettings
{
	bool showErrors;
	bool unmuteOnVolUp;
	bool unmuteOnVolDown;
	std::string volumeChangedCommand;
	std::string muteChangedCommand;
};

#endif