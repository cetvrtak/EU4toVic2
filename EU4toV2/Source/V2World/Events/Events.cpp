#include "Events.h"
#include "CommonRegexes.h"
#include "Event.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"


V2::Events::Events(const std::string& path)
{
	for (const auto& file: commonItems::GetAllFilesInFolderRecursive(path))
	{
		registerKeys(file);
		parseFile(path + "/" + file);
		clearRegisteredKeywords();
	}
}

void V2::Events::registerKeys(const std::string& file)
{
	registerRegex("country_event|province_event", [this, file](const std::string& unused, std::istream& theStream) {
		const Event newEvent(theStream);
		eventIds.insert(std::make_pair(newEvent.getId(), file));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}