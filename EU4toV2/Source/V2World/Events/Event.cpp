#include "Event.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V2::Event::Event(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V2::Event::registerKeys()
{
	registerKeyword("id", [this](const std::string& unused, std::istream& theStream) {
		id = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}