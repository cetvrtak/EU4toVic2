#ifndef PARTY_TYPE_MAPPER_H
#define PARTY_TYPE_MAPPER_H

#include "Parser.h"
#include "PartyType.h"
#include <map>
#include <string>
#include <optional>

namespace mappers
{
	class PartyTypeMapper: commonItems::parser
	{
	public:
		PartyTypeMapper();
		explicit PartyTypeMapper(std::istream& theStream);
		explicit PartyTypeMapper(const std::string& filename);
		
		[[nodiscard]] std::optional<PartyType> getPartyTypeByIdeology(const std::string& ideology) const;
		[[nodiscard]] const auto& getPartyTypeMap() const { return partyTypeMap; }

	private:
		void registerKeys();
		std::map<std::string, PartyType> partyTypeMap;
	};
}

#endif // PARTY_TYPE_MAPPER_H