#ifndef HPM_TAGS
#define HPM_TAGS value
#include "Parser.h"

namespace V2
{
	class HpmTags: commonItems::parser
	{
	public:
		HpmTags();

		[[nodiscard]] const auto& getHpmTagsMap() const { return theMap; }

	private:
		std::map<std::string, std::string> theMap;		
	};
}
#endif