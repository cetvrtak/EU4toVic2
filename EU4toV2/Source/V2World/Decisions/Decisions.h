#ifndef DECISIONS_H
#define DECISIONS_H

#include "Decision.h"
#include "Parser.h"

namespace V2
{
class Decisions: commonItems::parser
{
  public:
	Decisions() = default;
	explicit Decisions(const std::string& filename);
	explicit Decisions(std::istream& theStream);

	[[nodiscard]] const auto& getDecisions() const { return decisions; }

  private:
	void registerKeys();

	std::map<std::string, V2::Decision> decisions;
};
} // namespace V2


#endif // DECISIONS_H