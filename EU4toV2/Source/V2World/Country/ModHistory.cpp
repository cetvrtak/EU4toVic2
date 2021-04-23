#include "ModHistory.h"
#include "../../Configuration.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"

V2::ModHistory::ModHistory(const std::string& tag)
{
	registerKeys();
	const auto& mod = theConfiguration.getVic2ModPath() + "/" + theConfiguration.getVic2ModName();
	for (const auto& filename: Utils::GetAllFilesInFolder(mod + "/history/countries"))
	{
		if (filename.substr(0, 3) != tag)
			continue;
		parseFile(mod + "/history/countries/" + filename);
	}
	clearRegisteredKeywords();
}

V2::ModHistory::ModHistory(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V2::ModHistory::registerKeys()
{
	registerKeyword("capital", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt capitalStr(theStream);
		capital = capitalStr.getInt();
	});
	registerKeyword("primary_culture", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString primaryCultureStr(theStream);
		primaryCulture = primaryCultureStr.getString();
	});
	registerKeyword("culture", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString acceptedCulturesStr(theStream);
		acceptedCultures.insert(acceptedCulturesStr.getString());
	});
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString religionStr(theStream);
		religion = religionStr.getString();
	});
	registerKeyword("government", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString governmentStr(theStream);
		government = governmentStr.getString();
	});
	registerKeyword("plurality", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleDouble pluralityStr(theStream);
		plurality = pluralityStr.getDouble();
	});
	registerKeyword("nationalvalue", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString nationalValueStr(theStream);
		nationalValue = nationalValueStr.getString();
	});
	registerKeyword("literacy", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleDouble literacyStr(theStream);
		literacy = literacyStr.getDouble();
	});
	registerKeyword("non_state_culture_literacy", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleDouble nonStateCultureLiteracyStr(theStream);
		nonStateCultureLiteracy = nonStateCultureLiteracyStr.getDouble();
	});
	registerKeyword("civilized", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString civilizedStr(theStream);
		civilized = civilizedStr.getString();
	});
	registerKeyword("is_releasable_vassal", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString isReleasableVassalStr(theStream);
		releasableVassal = isReleasableVassalStr.getString();
	});
	registerKeyword("prestige", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt prestigeStr(theStream);
		prestige = prestigeStr.getInt();
	});
	registerKeyword("set_country_flag", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString countryFlagsStr(theStream);
		countryFlags.insert(countryFlagsStr.getString());
	});
	registerKeyword("govt_flag", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringOfItem govtFlagsStr(theStream);
		govtFlags.insert(govtFlagsStr.getString());
	});
	registerKeyword("decision", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString decisionsStr(theStream);
		decisions.insert(decisionsStr.getString());
	});
	registerKeyword("ruling_party", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString rulingPartyStr(theStream);
		rulingParty = rulingPartyStr.getString();
	});
	registerKeyword("upper_house", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringOfItem upperHouseStr(theStream);
		upperHouse = upperHouseStr.getString();
	});
	registerKeyword("consciousness", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt consciousnessStr(theStream);
		consciousness = consciousnessStr.getInt();
	});
	registerKeyword("nonstate_consciousness", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt nonStateConsciousnessStr(theStream);
		nonStateConsciousness = nonStateConsciousnessStr.getInt();
	});
	registerKeyword("schools", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString techSchoolStr(theStream);
		techSchool = techSchoolStr.getString();
	});
	registerKeyword("foreign_investment", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringOfItem foreignInvestmentStr(theStream);
		foreignInvestment = foreignInvestmentStr.getString();
	});
	registerKeyword("oob", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString oobStr(theStream);
		oob = oobStr.getString();
	});
	registerRegex("[0-9.]+", [this](const std::string& date, std::istream& theStream) {
		const commonItems::stringOfItem newBookmarkStr(theStream);
		bookmarks.insert(std::make_pair(date, newBookmarkStr.getString()));
	});
	registerRegex("[a-z_]+", [this](const std::string& key, std::istream& theStream) {
		const commonItems::singleString refTechsInvsStr(theStream);
		refsTechsInvs.push_back(std::make_pair(key, refTechsInvsStr.getString()));
	});
}