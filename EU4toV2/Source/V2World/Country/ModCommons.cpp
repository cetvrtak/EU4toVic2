#include "ModCommons.h"
#include "../../Configuration.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"

V2::ModCommons::ModCommons(const std::string& filename)
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

	const auto& file = determineFilePath(filename);
	if (file)
		parseFile(*file);
	clearRegisteredKeywords();
}

std::optional<std::string> V2::ModCommons::determineFilePath(const std::string& filename)
{
	const auto& mod = theConfiguration.getVic2ModPath() + "/" + theConfiguration.getVic2ModName();
	if (Utils::DoesFileExist(mod + "/common/countries/" + filename))
		return mod + "/common/countries/" + filename;
	else
		return std::nullopt;
}