#include "ParserHelpers.h"
#include "PartyType.h"

mappers::PartyType::PartyType(std::istream& theStream)
{
	registerKeyword("name", [this](const std::string& language, std::istream& theStream) 
		{
			const commonItems::singleString nameStr(theStream);
			name = nameStr.getString();
		});
	registerKeyword("ideology", [this](const std::string& language, std::istream& theStream)
		{
			const commonItems::singleString ideoStr(theStream);
			ideology = ideoStr.getString();
		});
	registerKeyword("start_date", [this](const std::string& language, std::istream& theStream)
		{
			const commonItems::singleString dateStr(theStream);
			start_date = date(dateStr.getString());
		});
	registerKeyword("end_date", [this](const std::string& language, std::istream& theStream)
		{
			const commonItems::singleString dateStr(theStream);
			end_date = date(dateStr.getString());
		});
	registerRegex("[a-z_]+", [this](const std::string& policy, std::istream& theStream) 
		{
			const commonItems::singleString positionStr(theStream);
			policies.insert(std::make_pair(policy, positionStr.getString()));
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}
