#ifndef HPM_TAGS_LINK
#define HPM_TAGS_LINK value
#include "Parser.h"

namespace V2
{
class HpmTagsLink: commonItems::parser
{
  public:
	HpmTagsLink(std::istream& theStream);

	[[nodiscard]] const auto& getTag() const { return tag; }
	[[nodiscard]] const auto& getLoc() const { return loc; }

  private:
	std::string tag;
	std::string loc;
};
} // namespace V2
#endif