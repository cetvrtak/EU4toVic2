#include "StateMapper.h"
#include "Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"

mappers::StateMapper::StateMapper()
{
	LOG(LogLevel::Info) << "Parsing state region structure.";
	registerKeys();

	std::string filename;
	if (commonItems::DoesFileExist("./blankMod/output/map/region.txt"))
	{
		filename = "./blankMod/output/map/region.txt";
	}
	else
	{
		filename = theConfiguration.getVic2Path() + "/map/region.txt";
	}

	parseFile(filename);
	clearRegisteredKeywords();
}


mappers::StateMapper::StateMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

mappers::StateMapper::StateMapper(const std::string& filename)
{
	registerKeys();
	parseFile(filename);
	clearRegisteredKeywords();
}

void mappers::StateMapper::registerKeys()
{
	registerRegex("[A-Z]{3}_[0-9]+", [this](const std::string& id, std::istream& theStream) 
		{
			const commonItems::intList provinceList(theStream);

			std::set<int> provinces;
			for (auto province : provinceList.getInts()) provinces.insert(province);
			for (auto province : provinces) stateProvincesMap.insert(std::make_pair(province, provinces));
			stateMap.insert(std::make_pair(id, provinces));
		});
}

std::set<int> mappers::StateMapper::getAllProvincesInState(const int province) const
{
	const auto& mapping = stateProvincesMap.find(province);
	if (mapping != stateProvincesMap.end()) return mapping->second;
	std::set<int> empty;
	return empty;
}

std::set<int> mappers::StateMapper::getProvincesByStateID(const int stateID) const
{
	const auto& mapping = stateMap.find(stateID);
	if (mapping != stateMap.end()) return mapping->second;
	std::set<int> empty;
	return empty;
}