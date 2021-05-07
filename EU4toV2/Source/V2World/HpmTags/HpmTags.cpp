#include "HpmTags.h"
#include "HpmTagsLink.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

V2::HpmTags::HpmTags()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const HpmTagsLink newMapping(theStream);
		theMap.insert(std::make_pair(newMapping.getTag(), newMapping.getLoc()));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	parseFile("configurables/hpm_tags.txt");
	clearRegisteredKeywords();
}