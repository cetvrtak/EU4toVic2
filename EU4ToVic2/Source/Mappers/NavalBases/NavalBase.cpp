#include "NavalBase.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::NavalBase::NavalBase(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::NavalBase::registerKeys()
{
	registerKeyword("naval_base", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		found = true;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
