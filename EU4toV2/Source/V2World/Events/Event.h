#ifndef EVENT_H
#define EVENT_H
#include "Parser.h"


namespace V2
{

class Event: commonItems::parser
{
  public:
	Event(std::istream& theStream);

	[[nodiscard]] const auto& getId() const { return id; }

  private:
	void registerKeys();

	int id;
};

} // namespace V2



#endif