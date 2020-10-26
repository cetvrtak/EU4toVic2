#include "CountryMapping.h"
#include "ParserHelpers.h"

mappers::CountryMapping::CountryMapping(std::istream& theStream)
{
	registerKeyword("eu4", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString eu4str(theStream);
		eu4Tag = eu4str.getString();
		std::transform(eu4Tag.begin(), eu4Tag.end(), eu4Tag.begin(), ::toupper);
	});
	registerKeyword("v2", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString v2str(theStream);
		vic2Tag = v2str.getString();
		std::transform(vic2Tag.begin(), vic2Tag.end(), vic2Tag.begin(), ::toupper);
	});
	registerKeyword("vic", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString eu4str(theStream);
		vanillaTag = eu4str.getString();
		std::transform(vanillaTag.begin(), vanillaTag.end(), vanillaTag.begin(), ::toupper);
	});
	registerKeyword("mod", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString v2str(theStream);
		modTag = v2str.getString();
		std::transform(modTag.begin(), modTag.end(), modTag.begin(), ::toupper);
	});
	registerKeyword("old", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString oldStr(theStream);
		oldTag = oldStr.getString();
		std::transform(oldTag.begin(), oldTag.end(), oldTag.begin(), ::toupper);
	});
	registerKeyword("new", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString newStr(theStream);
		newTag = newStr.getString();
		std::transform(newTag.begin(), newTag.end(), newTag.begin(), ::toupper);
	});
	registerKeyword("reform", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString reformStr(theStream);
		reforms.insert(reformStr.getString());
	});
	registerKeyword("flag", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString flagStr(theStream);
		flags.insert(flagStr.getString());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}
