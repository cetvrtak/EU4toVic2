#include "ColonialTagsMapper.h"
#include "Parser.h"
#include <fstream>
#include "ParserHelpers.h"
#include "../../Configuration.h"

mappers::ColonialTagMapper::ColonialTagMapper()
{
	registerKeys();
	std::string configFolder = "configurables";
	if (const auto& mod = theConfiguration.getVic2ModName(); !mod.empty())
		configFolder = "configurables/" + mod;
	parseFile(configFolder + "/colonial_tags.txt");
	clearRegisteredKeywords();
}

mappers::ColonialTagMapper::ColonialTagMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ColonialTagMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream)
		{
			const ColonialTag colonialBlock(theStream);
			colonyList.push_back(colonialBlock.getColonyTag());
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);
}