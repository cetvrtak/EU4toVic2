#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Province& province)
{
	for (const auto& bookmark: province.bookmarks)
	{
		output << *bookmark << "\n";
	}

	return output;
}

std::ostream& V2::operator<<(std::ostream& output, const std::optional<std::pair<int, std::vector<std::shared_ptr<Pop>>>>& pops)
{
	if (!pops) return output;
	if (!pops->first) return output;

	output << pops->first << " = {\n";
	for (const auto& pop : pops->second)
	{
		output << *pop;
	}
	output << "}\n";

	return output;
}