#include "Culture.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "ParserHelpers.h"
#include <random>

mappers::Culture::Culture(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (theConfiguration.isHpmEnabled() && !hpmLastNames.empty())
		lastNames = hpmLastNames;
}

void mappers::Culture::registerKeys()
{
	registerKeyword("hpm", [this](const std::string& unused, std::istream& theStream) {
		hpm = commonItems::singleString(theStream).getString();
	});
	registerKeyword("primary", [this](const std::string& unused, std::istream& theStream) {
		primaryTag = commonItems::singleString(theStream).getString();
	});
	registerKeyword("male_names", [this](const std::string& unused, std::istream& theStream) {
		maleNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("female_names", [this](const std::string& unused, std::istream& theStream) {
		femaleNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("first_names", [this](const std::string& unused, std::istream& theStream) {
		firstNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("last_names", [this](const std::string& unused, std::istream& theStream) {
		lastNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("hpm_last_names", [this](const std::string& unused, std::istream& theStream) {
		hpmLastNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("dynasty_names", [this](const std::string& unused, std::istream& theStream) {
		dynastyNames = commonItems::stringList(theStream).getStrings();
	});
	registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
		color = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerKeyword("radicalism", [this](const std::string& unused, std::istream& theStream) {
		radicalism = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::Culture::transmogrify(const std::optional<commonItems::Color>& mainColor, const size_t groupSize)
{
	firstNames = maleNames;
	lastNames = dynastyNames;

	int r = groupSize * (mainColor->getRgbComponents()[0]) % 255;
	int g = groupSize * (mainColor->getRgbComponents()[1]) % 255;
	int b = groupSize * (mainColor->getRgbComponents()[2]) % 255;
	color = commonItems::Color(std::array<int, 3>{r, g, b});
	primaryTag.clear();
}
