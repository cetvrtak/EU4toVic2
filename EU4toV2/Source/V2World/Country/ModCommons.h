#ifndef MOD_COMMONS_H
#define MOD_COMMONS_H

#include "../Party/Party.h"
#include "../../Mappers/PartyTypes/PartyTypeMapper.h"
#include "Color.h"
#include "Parser.h"

namespace V2
{
	class ModCommons: commonItems::parser
	{
	public:
		ModCommons();
		ModCommons(const std::string& filePath);

		void setCountryColors(const commonItems::Color& countryColors) { color = countryColors; }
		void setPartyDates();
		void setPartyIssues(const mappers::PartyTypeMapper& partyBlob);

		[[nodiscard]] const auto& getColor() const { return color; }
		[[nodiscard]] const auto& getGraphicalCulture() const { return graphicalCulture; }
		[[nodiscard]] const auto& getParties() const { return parties; }
		[[nodiscard]] const auto& getUnitNames() const { return unitNames; }
	
	private:
		commonItems::Color color;
		std::string graphicalCulture;
		std::vector<Party> parties;
		std::string unitNames;
	};
}


#endif // MOD_COMMONS_H