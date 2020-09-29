#ifndef COUNTRY_FLAGS_H
#define COUNTRY_FLAGS_H

#include "Parser.h"

namespace mappers
{
	class CountryFlags: commonItems::parser
	{
	public:
		CountryFlags();

		void registerKeys();

		[[nodiscard]] const auto& getFlags() const { return flags; }
	
	private:
		std::string eu4Flag;
		std::string v2Flag;
		std::map<std::string, std::string> flags;
	};
}


#endif // COUNTRY_FLAGS_H