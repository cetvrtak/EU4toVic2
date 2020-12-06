#include "../../Configuration.h"
#include "ProvinceDetails.h"
#include "ParserHelpers.h"

mappers::ProvinceDetails::ProvinceDetails(const std::string& filename)
{
	registerKeys();
	parseFile(filename);
	clearRegisteredKeywords();

	const auto& date = theConfiguration.getVic2StartDate();
	bookmarks[date] = makeNewBookmark(date);
}

mappers::ProvinceDetails::ProvinceDetails(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ProvinceDetails::registerKeys()
{
	registerKeyword("owner", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString ownerStr(theStream);
			owner = ownerStr.getString();
		});
	registerKeyword("controller", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString controllerStr(theStream);
			controller = controllerStr.getString();
		});
	registerKeyword("add_core", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString coreStr(theStream);
			cores.insert(coreStr.getString());
		});
	registerKeyword("trade_goods", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString rgoTypeStr(theStream);
			rgoType = rgoTypeStr.getString();
		});
	registerKeyword("life_rating", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleInt lifeRatingInt(theStream);
			lifeRating = lifeRatingInt.getInt();
		});
	registerKeyword("terrain", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString terrainStr(theStream);
			terrain = terrainStr.getString();
		});
	registerKeyword("colonial", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleInt colonialInt(theStream);
			colonial = colonialInt.getInt();
		});
	registerKeyword("colony", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleInt colonyInt(theStream);
			colonyLevel = colonyInt.getInt();
		});
	registerKeyword("naval_base", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleInt navalBaseLevelInt(theStream);
			navalBaseLevel = navalBaseLevelInt.getInt();
		});
	registerKeyword("fort", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleInt fortLevelInt(theStream);
			fortLevel = fortLevelInt.getInt();
		});
	registerKeyword("railroad", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleInt railLevelInt(theStream);
			railLevel = railLevelInt.getInt();
		});
	registerKeyword("is_slave", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString ignoredStr(theStream);
			slaveState = true;
		});
	registerKeyword("state_building", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::stringOfItem buildingStr(theStream);
			buildings.push_back(buildingStr.getString());
		});
	registerRegex("[0-9.]+", [this](const std::string& date, std::istream& theStream)
		{
			mappers::ProvinceDetails newBookmark(theStream);
			newBookmark.bookmarkDate = date;
			bookmarks[date] = newBookmark;
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);
}

mappers::ProvinceDetails mappers::ProvinceDetails::makeNewBookmark(const std::string& date) const
{
	mappers::ProvinceDetails newBookmark;
	newBookmark.bookmarkDate = date;
	newBookmark.owner = owner;
	newBookmark.controller = controller;
	newBookmark.rgoType = rgoType;
	newBookmark.terrain = terrain;
	newBookmark.climate = climate;
	newBookmark.lifeRating = lifeRating;
	newBookmark.colonial = colonial;
	newBookmark.colonyLevel = colonyLevel;
	newBookmark.navalBaseLevel = navalBaseLevel;
	newBookmark.fortLevel = fortLevel;
	newBookmark.railLevel = railLevel;
	newBookmark.slaveState = slaveState;
	newBookmark.cores = cores;
	newBookmark.buildings = buildings;

	return newBookmark;
}