#ifndef EU4EMPIRE_H
#define EU4EMPIRE_H

#include "Parser.h"
#include <set>

namespace EU4
{
	class EU4Empire :  commonItems::parser
	{
	public:
		explicit EU4Empire(std::istream& theStream);
		
		[[nodiscard]] const auto& getEmperor() const { return emperor; }
		[[nodiscard]] const auto& getHREReforms() const { return reforms; }

	private:
		std::string emperor;
		std::set<std::string> reforms;
	};
}

#endif // EU4EMPIRE_H