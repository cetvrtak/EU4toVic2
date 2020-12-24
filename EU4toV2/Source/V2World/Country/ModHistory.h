#ifndef MOD_HISTORY_H
#define MOD_HISTORY_H

#include "Parser.h"
#include <set>

namespace V2
{
class ModHistory: commonItems::parser
{
  public:
	ModHistory() = default;
	ModHistory(const std::string& filename);
	ModHistory(std::istream& theStream);

	void registerKeys();
	std::optional<std::string> determineFilePath(const std::string& filename);

	void addPoliticalReform(const std::string& reformName) { politicalReforms.insert(std::make_pair(reformName, refsTechsInvs[reformName])); }
	void addSocialReform(const std::string& reformName) { socialReforms.insert(std::make_pair(reformName, refsTechsInvs[reformName])); }
	void addNewReform(const std::string& reformName) { newReforms.insert(std::make_pair(reformName, refsTechsInvs[reformName])); }
	void addUncivReform(const std::string& reformName) { uncivReforms.insert(std::make_pair(reformName, refsTechsInvs[reformName])); }
	void addTech(const std::string& techName) { technologies.push_back(techName); }
	void addInvention(const std::string& inventionName) { inventions.push_back(inventionName); }

	[[nodiscard]] const auto& getPoliticalReforms() const { return politicalReforms; }
	[[nodiscard]] const auto& getSocialReforms() const { return socialReforms; }
	[[nodiscard]] const auto& getNewReforms() const { return newReforms; }
	[[nodiscard]] const auto& getUncivReforms() const { return uncivReforms; }
	[[nodiscard]] const auto& getTechnologies() const { return technologies; }
	[[nodiscard]] const auto& getInventions() const { return inventions; }

	[[nodiscard]] const auto& getCapital() const { return capital; }
	[[nodiscard]] const auto& getPrimaryCulture() const { return primaryCulture; }
	[[nodiscard]] const auto& getAcceptedCultures() const { return acceptedCultures; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	[[nodiscard]] const auto& getGovernment() const { return government; }
	[[nodiscard]] const auto& getPlurality() const { return plurality; }
	[[nodiscard]] const auto& getNationalValue() const { return nationalValue; }
	[[nodiscard]] const auto& getLiteracy() const { return literacy; }
	[[nodiscard]] const auto& getNonStateCultureLiteracy() const { return nonStateCultureLiteracy; }
	[[nodiscard]] const auto& getCivilized() const { return civilized; }
	[[nodiscard]] const auto& isReleasableVassal() const { return releasableVassal; }
	[[nodiscard]] const auto& getPrestige() const { return prestige; }
	[[nodiscard]] const auto& getRefsTechsInvs() const { return refsTechsInvs; }
	[[nodiscard]] const auto& getCountryFlags() const { return countryFlags; }
	[[nodiscard]] const auto& getDecisions() const { return decisions; }
	[[nodiscard]] const auto& getRulingParty() const { return rulingParty; }
	[[nodiscard]] const auto& getUpperHouse() const { return upperHouse; }
	[[nodiscard]] const auto& getConsciousness() const { return consciousness; }
	[[nodiscard]] const auto& getNonStateConsciousness() const { return nonStateConsciousness; }
	[[nodiscard]] const auto& getTechSchool() const { return techSchool; }
	[[nodiscard]] const auto& getForeignInvestment() const { return foreignInvestment; }
	[[nodiscard]] const auto& getOOB() const { return OOB; }
	[[nodiscard]] const auto& getBookmarks() const { return bookmarks; }

  private:
	std::map<std::string, std::string> politicalReforms;
	std::map<std::string, std::string> socialReforms;
	std::map<std::string, std::string> newReforms;
	std::map<std::string, std::string> uncivReforms;
	std::vector<std::string> technologies;
	std::vector<std::string> inventions;

	int capital;
	std::string primaryCulture;
	std::set<std::string> acceptedCultures;
	std::string religion;
	std::string government;
	double plurality;
	std::string nationalValue;
	double literacy;
	double nonStateCultureLiteracy;
	std::string civilized;
	std::string releasableVassal;
	int prestige;
	std::map<std::string, std::string> refsTechsInvs;
	std::vector<std::string> countryFlags;
	std::vector<std::string> decisions;
	std::string rulingParty;
	std::string upperHouse;
	double consciousness;
	double nonStateConsciousness;
	std::string techSchool;
	std::string foreignInvestment;
	std::string OOB;
	std::multimap<std::string, ModHistory> bookmarks;
};
} // namespace V2


#endif // MOD_HISTORY_H