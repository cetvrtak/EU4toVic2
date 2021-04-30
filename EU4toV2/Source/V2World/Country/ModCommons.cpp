#include "ModCommons.h"
#include "../../Configuration.h"
#include "../../Mappers/PartyTypes/PartyTypeMapper.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"

V2::ModCommons::ModCommons(const std::string& filename)
{
	registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringOfItem colorStr(theStream);
		color = colorStr.getString();
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
		const UnitNames newUnitNames(theStream);
		unitNames = std::make_shared<UnitNames>(newUnitNames);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	const auto& file = determineFilePath(filename);
	if (file)
		parseFile(*file);
	clearRegisteredKeywords();

	setPartyDates();
}

std::optional<std::string> V2::ModCommons::determineFilePath(const std::string& filename)
{
	const auto& mod = theConfiguration.getVic2ModPath() + "/" + theConfiguration.getVic2ModName();
	if (Utils::DoesFileExist(mod + "/common/countries/" + filename))
		return mod + "/common/countries/" + filename;
	else
		return std::nullopt;
}

void V2::ModCommons::setPartyDates()
{
	mappers::PartyTypeMapper partyTypeMapper("configurables/" + theConfiguration.getVic2ModName() + "/party_blobs.txt");
	std::map<std::string, bool> isActivated;
	std::set<std::string> activeBlobIdeologies;

	for (const auto& [blobIdeology, partyType]: partyTypeMapper.getPartyTypeMap())
	{
		if (partyType.getStartDate() < theConfiguration.getLastEU4Date())
			activeBlobIdeologies.insert(blobIdeology);
	}

	for (auto& party: parties)
	{
		const auto& ideology = party.getIdeology();
		const auto& blobParty = partyTypeMapper.getPartyTypeByIdeology(ideology);
		if (!blobParty)
		{
			Log(LogLevel::Warning) << party.getName() << " has undefined ideology!";
			continue;
		}
		const auto& startDate = blobParty->getStartDate();

		if (activeBlobIdeologies.count(ideology) && !party.isActiveOn(startDate) && !isActivated[ideology])
		{
			party.setStartDate(startDate.toString());
		}
		isActivated[ideology] = true;

		if (!activeBlobIdeologies.count(ideology) && party.isActiveOn(startDate))
		{
			party.setStartDate(startDate.toString());
		}
	}
}