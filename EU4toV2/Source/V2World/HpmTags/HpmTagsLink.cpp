#include "HpmTagsLink.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

V2::HpmTagsLink::HpmTagsLink(std::istream& theStream)
{
	registerKeyword("tag", [this](const std::string& unused, std::istream& theStream) {
		tag = commonItems::singleString(theStream).getString();
	});
	registerKeyword("loc", [this](const std::string& unused, std::istream& theStream) {
		loc = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	parseStream(theStream);
	clearRegisteredKeywords();
}