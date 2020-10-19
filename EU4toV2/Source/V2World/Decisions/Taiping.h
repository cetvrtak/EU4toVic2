#ifndef TAIPING_CSA_H
#define TAIPING_CSA_H

#include "Parser.h"

namespace V2
{
class Taiping: commonItems::parser
{
  public:
	Taiping() = default;
	explicit Taiping(std::istream& theStream);

	[[nodiscard]] const auto& getTagEffectMap() const { return tagEffectMap; }
	[[nodiscard]] const auto& getNonCountrySpecificEffects() const { return nonCountrySpecificEffects; }
	[[nodiscard]] const auto& getTag() const { return tag; }
	[[nodiscard]] const auto& getCountryCores() const { return countryCores; }

  private:
  	std::map<std::string, std::string> tagEffectMap;
  	std::vector<std::string> nonCountrySpecificEffects;
	std::vector<std::string> countryCores;
  	std::string tag;
};

class Limit: commonItems::parser
{
public:
	Limit() = default;
	Limit(std::istream& theStream);

	[[nodiscard]] const auto& getTag() const { return tag; }

private:
	std::string tag;	
};
} // namespace V2


#endif // TAIPING_CSA_H