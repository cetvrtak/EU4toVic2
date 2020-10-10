#include "ModCommons.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"

V2::ModCommons::ModCommons(){};

V2::ModCommons::ModCommons(const std::string& filePath)
{
	registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
		auto color = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerKeyword("graphical_culture", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString graphicalCultureStr(theStream);
		graphicalCulture = graphicalCultureStr.getString();
	});
	registerKeyword("party", [this](const std::string& unused, std::istream& theStream) {
		const mappers::PartyType newPartyType(theStream);
		const Party newParty(newPartyType);
		parties.push_back(newParty);
	});
	registerKeyword("unit_names", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringOfItem unitNamesStr(theStream);
		unitNames = unitNamesStr.getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	parseFile(filePath);
	clearRegisteredKeywords();
}

void V2::ModCommons::setPartyDates()
{
	bool conservativeSet = false, liberalSet = false, reactionarySet = false;

	for (auto& party: parties)
	{
		if (party.getIdeology() == "conservative" && !conservativeSet)
		{
			party.setStartDate("1000.1.1");
			conservativeSet = true;
			continue;
		}
		if (party.getIdeology() == "liberal" && !liberalSet)
		{
			party.setStartDate("1680.1.1");
			liberalSet = true;
			continue;
		}
		if (party.getIdeology() == "reactionary" && !reactionarySet)
		{
			party.setStartDate("1790.1.1");
			reactionarySet = true;
		}
	}
}

void V2::ModCommons::setPartyIssues(const mappers::PartyTypeMapper& partyBlob)
{
	for (auto& party: parties)
	{
		const auto& ideology = party.getIdeology();

		if (party.getSocialPolicy().empty())
			party.setSocialPolicy(partyBlob.getPartyTypeByIdeology(ideology)->getSocialPolicy());
	}	
}