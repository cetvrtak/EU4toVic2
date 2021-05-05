#include "StartingTechMapper.h"
#include "../../Configuration.h"
#include "../Technologies/Technologies.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::StartingTechMapper::StartingTechMapper()
{
	LOG(LogLevel::Info) << "Parsing starting techs.";
	registerKeyword("tech", [this](const std::string& techGroup, std::istream& theStream) {
		const StartingTech startingTech(theStream);
		startingTechs.emplace_back(startingTech);
	});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);

	std::string configFolder = "configurables";
	if (const auto& mod = theConfiguration.getVic2ModName(); !mod.empty())
		configFolder = "configurables/" + mod;

	parseFile(configFolder + "/starting_technologies.txt");
	clearRegisteredKeywords();
}

std::vector<std::string> mappers::StartingTechMapper::getTechsForGroupAndScore(const std::string& group,
	 const double score,
	 const std::map<std::string, std::string> idNameMap) const
{
	std::vector<std::string> toReturn;
	for (const auto& tech: startingTechs)
		if (tech.getGroup() == group && score >= tech.getScore())
		{
			toReturn.emplace_back(tech.getName(idNameMap));
		}
	return toReturn;
}