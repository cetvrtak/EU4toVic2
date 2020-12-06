#ifndef PROVINCE_DETAILS_H
#define PROVINCE_DETAILS_H

#include "Parser.h"
#include <set>

namespace mappers
{
	class ProvinceDetails : commonItems::parser
	{
	public:
		ProvinceDetails() = default;
		explicit ProvinceDetails(const std::string& filename);
		explicit ProvinceDetails(std::istream& theStream);
		
		mappers::ProvinceDetails dumpIntoBookmark(const std::string& date) const;
		void resetBookmark(const std::set<std::string>& baseCores);
		mappers::ProvinceDetails merge(const mappers::ProvinceDetails& newBookmark, const std::string& date);
		[[nodiscard]] const std::string& getBookmarkDate() { return bookmarkDate; }

		// These values are open to ease management.
		// This is a storage container for V2::Province.
		std::string owner;
		std::string controller;
		std::string rgoType;
		std::string terrain;
		std::string climate;
		std::set<std::string> cores;
		std::set<std::string> remCores;
		int lifeRating = 0;
		std::optional<int> colonial;
		std::optional<int> colonyLevel;
		int navalBaseLevel = 0;
		int fortLevel = 0;
		int railLevel = 0;
		bool slaveState = false;
		std::vector<std::string> buildings;
		std::map<std::string, mappers::ProvinceDetails> bookmarks;
		std::string bookmarkDate;

		friend std::ostream& operator<<(std::ostream& output, const ProvinceDetails& provinceDetails);

	private:
		void registerKeys();
	};
}

#endif // PROVINCE_DETAILS_H
