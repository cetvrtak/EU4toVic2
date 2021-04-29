#ifndef WORLD_H
#define WORLD_H

#include "../EU4World/Country/EU4Country.h"
#include "../Mappers/Adjacency/AdjacencyMapper.h"
#include "../Mappers/AfricaReset/AfricaResetMapper.h"
#include "../Mappers/CountryFlags/CountryFlags.h"
#include "../Mappers/CountryMappings/CountryMappings.h"
#include "../Mappers/CulturalUnions/CulturalUnionMapper.h"
#include "../Mappers/CultureGroups/CultureGroups.h"
#include "../Mappers/CultureMapper/CultureMapper.h"
#include "../Mappers/FactoryTypes/FactoryTypeMapper.h"
#include "../Mappers/Geography/ClimateMapper.h"
#include "../Mappers/Geography/Continents.h"
#include "../Mappers/Geography/TerrainDataMapper.h"
#include "../Mappers/GovernmentMapper/GovernmentMapper.h"
#include "../Mappers/Issues/Issues.h"
#include "../Mappers/Issues/ModReforms.h"
#include "../Mappers/LeaderTraits/LeaderTraitMapper.h"
#include "../Mappers/MinorityPops/MinorityPopMapper.h"
#include "../Mappers/NavalBases/NavalBaseMapper.h"
#include "../Mappers/PartyNames/PartyNameMapper.h"
#include "../Mappers/PartyTypes/PartyTypeMapper.h"
#include "../Mappers/PortProvinces/PortProvinces.h"
#include "../Mappers/ProvinceMappings/ProvinceMapper.h"
#include "../Mappers/RGORandomization/BucketList.h"
#include "../Mappers/RegimentCosts/RegimentCostsMapper.h"
#include "../Mappers/RegionLocalizations/RegionLocalizations.h"
#include "../Mappers/ReligionMapper/ReligionMapper.h"
#include "../Mappers/StartingInventionMapper/StartingInventionMapper.h"
#include "../Mappers/StateMapper/StateMapper.h"
#include "../Mappers/TechSchools/TechSchoolMapper.h"
#include "../Mappers/Technologies/Technologies.h"
#include "../Mappers/Unreleasables/Unreleasables.h"
#include "../Mappers/WarGoalMapper/WarGoalMapper.h"
#include "Country/Country.h"
#include "Country/CountryPopLogger.h"
#include "Diplomacy/Diplomacy.h"
#include "Events/Events.h"
#include "MappingChecker/MappingChecker.h"
#include "Output/ModFile.h"
#include "Province/Province.h"
#include "Province/ProvinceNameParser.h"
#include "VanillaWorld/VanillaWorld.h"
#include "War/War.h"
#include <list>
#include <memory>
#include <set>

namespace mappers
{
class VersionParser;
class TechGroupsMapper;
} // namespace mappers

namespace V2
{
class World
{
  public:
	World(const EU4::World& sourceWorld,
		 const mappers::IdeaEffectMapper& ideaEffectMapper,
		 const mappers::TechGroupsMapper& techGroupsMapper,
		 const mappers::VersionParser& versionParser);

  private:
	int nativePopulationBefore;
	int nativePopulationAfter;
	int minorsBefore;
	int minorsAfter;
	void determineFrCountries();
	void outputFrFiles() const;
	void makeFrPops();
	std::shared_ptr<Pop> expelNonNatives(const std::shared_ptr<V2::Province>& province, std::shared_ptr<Pop> pop);
	const std::string getGroupName(const std::string& culture) const;

	std::set<std::string> frCores;
	std::map<std::string, std::shared_ptr<Country>> hpmCountries;

	long totalWorldPopulation = 0;
	int stateId = 0; // ID counter for generated states
	bool isRandomWorld = false;

	std::map<std::string, std::list<int>> popRegions;
	std::map<int, std::shared_ptr<Province>> provinces;
	std::map<std::string, std::shared_ptr<Country>> countries;
	std::map<std::string, std::shared_ptr<Country>> potentialCountries;
	std::map<std::string, std::shared_ptr<Country>> dynamicCountries;
	std::vector<War> wars;
	std::vector<std::pair<std::string, EU4::HistoricalEntry>> historicalData; // HoI4 export dynasty+rulers
	std::set<std::string> neoCultureLocalizations;									  // raw strings for output.
	std::map<int, int> stateMap;
	std::map<int, int> provinceMap;
	std::vector<std::string> reassignedTags;

	[[nodiscard]] std::optional<std::string> determineProvinceOwnership(const std::vector<int>& eu4ProvinceNumbers, const EU4::World& sourceWorld) const;
	[[nodiscard]] std::shared_ptr<Province> getProvince(int provID) const;
	[[nodiscard]] std::shared_ptr<Country> getCountry(const std::string& tag) const;
	[[nodiscard]] unsigned int countCivilizedNations() const;
	[[nodiscard]] const auto& getStateMap() const { return stateMap; }
	[[nodiscard]] const auto& getProvinceMap() const { return provinceMap; }
	[[nodiscard]] const auto& getModReforms() const { return modReforms; }

	static std::optional<std::string> determineProvinceControllership(const std::vector<int>& eu4ProvinceNumbers, const EU4::World& sourceWorld);
	std::shared_ptr<Country> createOrLocateCountry(const std::string& V2Tag, const EU4::Country& sourceCountry);
	static std::set<std::string> discoverProvinceFilenames();
	static std::set<std::string> discoverModProvinceFilenames();

	void dropStates(const mappers::TechGroupsMapper& techGroupsMapper);
	void dropCores();
	void importProvinces();
	void importModProvinces();
	void shuffleRgos();
	void importDefaultPops();
	void importPopsFromFile(const std::string& filename, const mappers::MinorityPopMapper& minorityPopMapper);
	void importPopsFromProvince(int provinceID, const mappers::PopTypes& popType, const mappers::MinorityPopMapper& minorityPopMapper);
	void importPotentialCountries();
	void importPotentialCountry(const std::string& file, const std::string& line, bool dynamicCountry);
	void initializeCultureMappers();
	void convertCountries(const EU4::World& sourceWorld, const mappers::IdeaEffectMapper& ideaEffectMapper);
	void convertProvinces(const EU4::World& sourceWorld, const mappers::TechGroupsMapper& techGroupsMapper, const EU4::Regions& eu4Regions);
	void updateProvinceHistory();
	void initializeCountries(const EU4::World& sourceWorld, const mappers::IdeaEffectMapper& ideaEffectMapper);
	void convertNationalValues();
	void convertPrestige();
	void addAllPotentialCountries();
	void setupColonies();
	void setupStates();
	void convertUncivReforms(const EU4::World& sourceWorld, const mappers::TechGroupsMapper& techGroupsMapper);
	void convertTechs(const EU4::World& sourceWorld);
	void allocateFactories(const EU4::World& sourceWorld);
	void setupPops(const EU4::World& sourceWorld);
	void addUnions(const EU4::World& sourceWorld);
	void decentralizeHRE(const EU4::World& sourceWorld);
	void convertArmies();
	void output(const mappers::VersionParser& versionParser) const;
	void createModFile() const;
	void outputPops() const;
	static void outputVersion(const mappers::VersionParser& versionParser);
	void modifyDefines() const;
	void outputCommonCountries() const;
	void outputLocalisation() const;
	void outputProvinces() const;
	void outputCountries() const;
	void outputWars() const;
	void outputHistory() const;
	void outputCultures() const;
	void outputNeoCultures() const;
	void verifyCountriesWritten() const;
	void convertWars(const EU4::World& sourceWorld);
	void transcribeHistoricalData();
	void transcribeNeoCultures();
	void modifyPrimaryAndAcceptedCultures();
	void addAcceptedCultures(const EU4::Regions& eu4Regions);
	void addReligionCulture();

	void addStateMapping(int origID, int modID) { stateMap.insert(std::make_pair(origID, modID)); }
	int findBestMatch(const std::pair<int, std::map<int, int>>& state);
	void sortStateMap(std::map<int, std::map<int, int>>& freqMap, std::vector<int>& sortedStates);
	void updateStateMap(std::map<int, std::map<int, int>>& freqMap, int modStateID);
	void addProvinceMapping(int origID, int modID) { provinceMap.insert(std::make_pair(origID, modID)); }
	void copyModFiles() const;
	void outputStateMap(std::string srcFile, std::string outFile) const;
	void outStateMap(std::string outFile) const;
	void outProvinceMap(std::string outFile) const;
	void outEvents() const;
	void convertEvents();
	void drawProvinceMap();
	void drawStateMap();
	bool verifyMap(std::map<int, int> theMap) const;
	void mapUnlocalized(const std::vector<int>& vanillaProvs, std::map<int, std::string> locProvs, std::ofstream& output);
	void mapUnchanged(const std::vector<int>& vanillaProvs,
		 const mappers::ProvinceMapper& vanillaMapper,
		 const mappers::ProvinceMapper& modMapper,
		 std::ofstream& output);
	void mapLeftovers(const std::vector<int>& vanillaProvs,
		 const mappers::ProvinceMapper& vanillaMapper,
		 const mappers::ProvinceMapper& modMapper,
		 std::ofstream& output);
	int getActualStateID(int provID, const std::map<int, std::set<int>>& theMap) const;
	void convertCountryFlags();
	void updateCountryDetails();
	std::vector<std::string> getIssues(const std::string& issueCategory);
	void identifyReassignedTags();
	bool isTagReassigned(const std::string& tag) const;
	void updateFlags() const;
	void outputGTFO(std::map<std::string, std::shared_ptr<Country>> countries) const;
	void outputReturnCores(std::map<std::string, std::shared_ptr<V2::Country>> countries) const;
	void outputTechnologies() const;
	void removeRedundantFiles() const;
	int getModStateId(int province, std::ofstream& output);
	
	void updateCountryHistory();

	mappers::ProvinceMapper provinceMapper;
	mappers::Continents continentsMapper;
	mappers::CountryMappings countryMapper;
	mappers::AdjacencyMapper adjacencyMapper;
	mappers::ClimateMapper climateMapper;
	mappers::TerrainDataMapper terrainDataMapper;
	mappers::CultureMapper cultureMapper;
	mappers::CultureMapper slaveCultureMapper;
	mappers::GovernmentMapper governmentMapper;
	mappers::MinorityPopMapper minorityPopMapper;
	mappers::PartyNameMapper partyNameMapper;
	mappers::PartyTypeMapper partyTypeMapper;
	mappers::RegimentCostsMapper regimentCostsMapper;
	mappers::ReligionMapper religionMapper;
	mappers::StateMapper stateMapper;
	mappers::TechSchoolMapper techSchoolMapper;
	mappers::CulturalUnionMapper culturalUnionMapper;
	mappers::CulturalUnionMapper culturalNationalitiesMapper;
	mappers::FactoryTypeMapper factoryTypeMapper;
	mappers::Unreleasables unreleasablesMapper;
	mappers::LeaderTraitMapper leaderTraitMapper;
	mappers::NavalBaseMapper navalBaseMapper;
	mappers::BucketList bucketShuffler;
	mappers::PortProvinces portProvincesMapper;
	mappers::WarGoalMapper warGoalMapper;
	mappers::StartingInventionMapper startingInventionMapper;
	mappers::CultureGroups cultureGroupsMapper;
	mappers::RegionLocalizations regionLocalizations;
	mappers::AfricaResetMapper africaResetMapper;
	ProvinceNameParser provinceNameParser;
	CountryPopLogger countryPopLogger;
	MappingChecker mappingChecker;
	ModFile modFile;
	Diplomacy diplomacy;
	std::map<std::string, std::string> events;
	VanillaWorld vanillaWorld;
	mappers::CountryFlags countryFlags;
	mappers::Issues issues;
	mappers::ModReforms modReforms;
	mappers::Technologies technologies;
};

std::ostream& operator<<(std::ostream& output, const std::vector<std::pair<std::string, EU4::HistoricalEntry>>& historicalData);
} // namespace V2

#endif // WORLD_H
