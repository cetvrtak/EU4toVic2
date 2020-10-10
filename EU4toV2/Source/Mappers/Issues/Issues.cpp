#include "Issues.h"
#include "ParserHelpers.h"
#include <Log.h>
#include "OSCompatibilityLayer.h"
#include "../../Configuration.h"

mappers::Issues::Issues()
{
	Log(LogLevel::Info) << "Parsing issues";
	registerKeys();
	const auto& filePath = determineSrcFile();
	parseFile(filePath);
	clearRegisteredKeywords();
}

void mappers::Issues::registerKeys()
{
	registerRegex("[a-z_]+", [this](const std::string& categoryName, std::istream& theStream) {
		Log(LogLevel::Info) << " -> " << categoryName;
		commonItems::stringsOfItemNames issuesStr(theStream);
		const auto& issues = issuesStr.getStrings();
		categories.insert(std::make_pair(categoryName, issues));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::string mappers::Issues::determineSrcFile()
{
	std::string filePath;
	if (const auto& mod = theConfiguration.getVic2ModName();
		 !mod.empty() && Utils::DoesFileExist(theConfiguration.getVic2ModPath() + "/" + mod + "/common/issues.txt"))
	{
		filePath = theConfiguration.getVic2ModPath() + "/" + mod + "/common/issues.txt";
	}
	else
	{
		filePath = theConfiguration.getVic2Path() + "/common/issues.txt";
	}
	return filePath;
}