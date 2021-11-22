#ifndef EVENTS_H
#define EVENTS_H
#include "Parser.h"
#include <set>



namespace V2
{
class Events: commonItems::parser
{
  public:
	Events() = default;
	explicit Events(const std::string& path);

	[[nodiscard]] const auto& getEventIds() const { return eventIds; }

  private:
	void registerKeys(const std::string& file);

	std::map<int, std::string> eventIds;
};
} // namespace V2



#endif // EVENTS_H