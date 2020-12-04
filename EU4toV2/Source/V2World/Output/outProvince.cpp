#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Province& province)
{
	if (!province.details.owner.empty())
	{
		output << "owner = " << province.details.owner << "\n";
	}
	if (!province.details.controller.empty())
	{
		output << "controller = " << province.details.controller << "\n";
	}
	for (const auto& core: province.details.cores)
	{
		output << "add_core = " << core << "\n";
	}
	if (!province.details.rgoType.empty())
	{
		output << "trade_goods = " << province.details.rgoType << "\n";
	}
	if (province.details.lifeRating > 0)
	{
		output << "life_rating = " << province.details.lifeRating << "\n";
	}
	if (!province.details.terrain.empty())
	{
		output << "terrain = " << province.details.terrain << "\n";
	}
	if (province.details.colonial > 0)
	{
		output << "colonial = " << province.details.colonial << "\n";
	}
	if (province.details.navalBaseLevel > 0)
	{
		output << "naval_base = " << province.details.navalBaseLevel << "\n";
	}
	if (province.details.fortLevel > 0)
	{
		output << "fort = " << province.details.fortLevel << "\n";
	}
	if (province.details.railLevel > 0)
	{
		output << "railroad = " << province.details.railLevel << "\n";
	}
	if (province.details.slaveState)
	{
		output << "is_slave = yes\n";
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