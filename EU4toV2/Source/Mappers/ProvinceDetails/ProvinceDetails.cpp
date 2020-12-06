#include "../../Configuration.h"
#include "ProvinceDetails.h"
#include "ParserHelpers.h"

mappers::ProvinceDetails::ProvinceDetails(const std::string& filename)
{
	registerKeys();
	parseFile(filename);
	clearRegisteredKeywords();

	const auto& date = theConfiguration.getVic2StartDate();
	bookmarks[date] = dumpIntoBookmark(date);
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
	registerKeyword("remove_core", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleString coreStr(theStream);
			remCores.insert(coreStr.getString());
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
			colonial = std::make_optional(colonialInt.getInt());
		});
	registerKeyword("colony", [this](const std::string& unused, std::istream& theStream)
		{
			const commonItems::singleInt colonyInt(theStream);
			colonyLevel = std::make_optional(colonyInt.getInt());
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
			if (!bookmarks.count(date))
			{
				newBookmark.bookmarkDate = date;
				bookmarks[date] = newBookmark;
			}
			else
				bookmarks[date] = merge(newBookmark, date);
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);
}

mappers::ProvinceDetails mappers::ProvinceDetails::dumpIntoBookmark(const std::string& date) const
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
	newBookmark.remCores = remCores;
	newBookmark.buildings = buildings;

	return newBookmark;
}

void mappers::ProvinceDetails::resetBookmark(const std::set<std::string>& baseCores)
{
	if (owner.empty()) owner = "---";
	if (controller.empty() || owner.empty()) controller = "---";

	for (const auto& core: baseCores)
	{
		if (cores.find(core) == cores.end())
			remCores.insert(core);
	}
}

mappers::ProvinceDetails mappers::ProvinceDetails::merge(const mappers::ProvinceDetails& newBookmark, const std::string& date)
{
	auto bookmark = bookmarks[date];

	if (!newBookmark.owner.empty()) bookmark.owner = newBookmark.owner;
	if (!newBookmark.controller.empty()) bookmark.controller = newBookmark.controller;
	if (!newBookmark.rgoType.empty()) bookmark.rgoType = newBookmark.rgoType;
	if (newBookmark.terrain.empty()) bookmark.terrain = newBookmark.terrain;
	if (newBookmark.climate.empty()) bookmark.climate = newBookmark.climate;
	if (newBookmark.lifeRating > 0) bookmark.lifeRating = newBookmark.lifeRating;
	if (newBookmark.colonial) bookmark.colonial = newBookmark.colonial;
	if (newBookmark.colonyLevel) bookmark.colonyLevel = newBookmark.colonyLevel;
	if (newBookmark.navalBaseLevel > 0) bookmark.navalBaseLevel = newBookmark.navalBaseLevel;
	if (newBookmark.fortLevel > 0) bookmark.fortLevel = newBookmark.fortLevel;
	if (newBookmark.railLevel > 0) bookmark.railLevel = newBookmark.railLevel;
	if (newBookmark.slaveState) bookmark.slaveState = newBookmark.slaveState;

	for (const auto& core: newBookmark.cores)
	{
		if (bookmark.cores.find(core) == bookmark.cores.end())
			bookmark.cores.insert(core);
	}
	for (const auto& core: newBookmark.remCores)
	{
		if (bookmark.remCores.find(core) == bookmark.remCores.end())
			bookmark.remCores.insert(core);
	}
	for (const auto& building: newBookmark.buildings)
	{
		if (std::find(bookmark.buildings.begin(), bookmark.buildings.end(), building) == bookmark.buildings.end())
			bookmark.buildings.push_back(building);
	}

	return bookmark;
}