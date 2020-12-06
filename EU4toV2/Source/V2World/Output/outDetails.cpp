#include "../../Configuration.h"
#include "output.h"

std::ostream& mappers::operator<<(std::ostream& output, const ProvinceDetails& provinceDetails)
{
	if (provinceDetails.bookmarkDate == theConfiguration.getLastEU4Date().toString())
	{
		if (!provinceDetails.owner.empty())
		{
			output << "owner = " << provinceDetails.owner << "\n";
		}
		if (!provinceDetails.controller.empty())
		{
			output << "controller = " << provinceDetails.controller << "\n";
		}
		for (const auto& core: provinceDetails.cores)
		{
			output << "add_core = " << core << "\n";
		}
		if (!provinceDetails.rgoType.empty())
		{
			output << "trade_goods = " << provinceDetails.rgoType << "\n";
		}
		if (provinceDetails.lifeRating > 0)
		{
			output << "life_rating = " << provinceDetails.lifeRating << "\n";
		}
		if (!provinceDetails.terrain.empty())
		{
			output << "terrain = " << provinceDetails.terrain << "\n";
		}
		if (provinceDetails.colonial > 0)
		{
			output << "colonial = " << provinceDetails.colonial << "\n";
		}
		if (provinceDetails.navalBaseLevel > 0)
		{
			output << "naval_base = " << provinceDetails.navalBaseLevel << "\n";
		}
		if (provinceDetails.fortLevel > 0)
		{
			output << "fort = " << provinceDetails.fortLevel << "\n";
		}
		if (provinceDetails.railLevel > 0)
		{
			output << "railroad = " << provinceDetails.railLevel << "\n";
		}
		if (provinceDetails.slaveState)
		{
			output << "is_slave = yes\n";
		}
		for (const auto& building: provinceDetails.buildings)
		{
			output << "state_building " << building << "\n";
		}
	}
	else
	{
		output << provinceDetails.bookmarkDate << " = {\n";

		if (!provinceDetails.owner.empty())
		{
			output << "\towner = " << provinceDetails.owner << "\n";
		}
		if (!provinceDetails.controller.empty())
		{
			output << "\tcontroller = " << provinceDetails.controller << "\n";
		}
		for (const auto& core: provinceDetails.cores)
		{
			output << "\tadd_core = " << core << "\n";
		}
		if (!provinceDetails.rgoType.empty())
		{
			output << "\ttrade_goods = " << provinceDetails.rgoType << "\n";
		}
		if (provinceDetails.lifeRating > 0)
		{
			output << "\tlife_rating = " << provinceDetails.lifeRating << "\n";
		}
		if (!provinceDetails.terrain.empty())
		{
			output << "\tterrain = " << provinceDetails.terrain << "\n";
		}
		if (provinceDetails.colonial > 0)
		{
			output << "\tcolonial = " << provinceDetails.colonial << "\n";
		}
		if (provinceDetails.navalBaseLevel > 0)
		{
			output << "\tnaval_base = " << provinceDetails.navalBaseLevel << "\n";
		}
		if (provinceDetails.fortLevel > 0)
		{
			output << "\tfort = " << provinceDetails.fortLevel << "\n";
		}
		if (provinceDetails.railLevel > 0)
		{
			output << "\trailroad = " << provinceDetails.railLevel << "\n";
		}
		if (provinceDetails.slaveState)
		{
			output << "\tis_slave = yes\n";
		}
		for (const auto& building: provinceDetails.buildings)
		{
			output << "\tstate_building " << building << "\n";
		}

		output << "}\n";
	}

	return output;
}