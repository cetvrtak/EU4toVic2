#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Country& country)
{
	if (country.details.capital > 0)
	{
		output << "capital = " << country.details.capital << "\n";
	}
	output << "primary_culture = " << country.details.primaryCulture << "\n";
	for (const auto& culture: country.details.acceptedCultures)
	{
		output << "culture = " << culture << "\n";
	}
	output << "religion = " << country.details.religion << "\n";
	output << "government = " << country.details.government << "\n";
	if (country.modHistory)
	{
		if (country.modHistory->getPlurality() == 0)
		{
			output << "plurality = 0.0\n";
		}
		else
		{
			output << "plurality = " << country.modHistory->getPlurality() << "\n";
		}
	}
	else
	{
		output << "plurality = " << country.details.plurality << "\n";
	}
	output << "nationalvalue = " << country.details.nationalValue << "\n";
	output << "literacy = " << country.details.literacy << "\n";
	if (country.details.nonStateCultureLiteracy > 0)
	{
		output << "non_state_culture_literacy = " << country.details.nonStateCultureLiteracy << "\n";
	}
	if (country.details.civilized)
	{
		output << "civilized = yes\n";
	}
	if (country.modHistory && country.modHistory->getPrestige() > 0)
	{
		output << "prestige = " << country.modHistory->getPrestige() << "\n";
	}
	else if (country.details.prestige > 0)
	{
		output << "prestige = " << country.details.prestige << "\n";
	}
	if (!country.details.isReleasableVassal)
	{
		output << "is_releasable_vassal = no\n";
	}
	if (!country.details.politicalReforms.empty())
	{
		output << "\n";
		output << "# Political reforms\n";
		for (const auto& [reform, level]: country.details.politicalReforms)
		{
			output << reform << " = " << level << "\n";
		}
	}
	else if (country.reforms)
	{
		output << *country.reforms;
	}
	if (!country.details.socialReforms.empty())
	{
		output << "\n";
		output << "# Social Reforms\n";
		for (const auto& [reform, level]: country.details.socialReforms)
		{
			output << reform << " = " << level << "\n";
		}
	}
	else
	{
		output << "\n";
		output << "# Social Reforms\n";
		output << "wage_reform = no_minimum_wage\n";
		output << "work_hours = no_work_hour_limit\n";
		output << "safety_regulations = no_safety\n";
		output << "health_care = no_health_care\n";
		output << "unemployment_subsidies = no_subsidies\n";
		output << "pensions = no_pensions\n";
		output << "school_reforms = no_schools\n";
	}

	if (const auto& modReforms = country.getModReforms(); !modReforms.empty())
	{
		output << "\n";
		output << "# New Reforms\n";
		for (const auto& modReform: modReforms)
		{
			output << modReform.first << " = " << modReform.second << "\n";
		}
		output << "\n";
	}

	for (const auto& flag: country.details.countryFlags)
	{
		output << "set_country_flag = " << flag << "\n";
	}
	for (const auto& govtFlag: country.details.govtFlags)
	{
		output << "govt_flag " << govtFlag << "\n";
	}
	for (const auto& decision: country.decisions)
	{
		output << "decision = " << decision << "\n";
	}

	output << "\n";
	output << "ruling_party = " << country.details.rulingParty << "\n";
	if (!country.details.upperHouse.empty())
	{
		output << "upper_house " << country.details.upperHouse << "\n";
	}
	else
	{
		output << "upper_house = \n";
		output << "{\n";
		output << "	fascist = 0\n";
		output << "	liberal = " << country.details.upperHouseLiberal << "\n";
		output << "	conservative = " << country.details.upperHouseConservative << "\n";
		output << "	reactionary = " << country.details.upperHouseReactionary << "\n";
		output << "	anarcho_liberal = 0\n";
		output << "	socialist = 0\n";
		output << "	communist = 0\n";
		output << "}\n";
	}
	output << "\n";
	output << "\n";
	if (country.modHistory)
	{
		for (const auto& [reform, level]: country.details.uncivReforms)
		{
			output << reform << " = " << level << "\n";
		}
	}
	else if (!country.details.civilized && country.uncivReforms)
	{
		output << *country.uncivReforms;
	}
	output << "\n";
	output << "# Technologies\n";
	for (const auto& tech: country.techs)
	{
		output << tech << " = 1\n";
	}
	output << "\n";
	output << "# Starting Consciousness\n";
	if (country.modHistory)
	{
		output << "consciousness = " << country.modHistory->getConsciousness() << "\n";
		output << "nonstate_consciousness = " << country.modHistory->getNonStateConsciousness() << "\n";
	}
	else
	{
		output << "consciousness = " << country.details.consciousness << "\n";
		output << "nonstate_consciousness = " << country.details.nonstateConsciousness << "\n";
	}

	if (!country.details.techSchool.empty())
	{
		output << "\n";
		output << "schools = " << country.details.techSchool << "\n";
	}

	if (!country.details.foreignInvestment.empty())
	{
		output << "\n";
		output << "foreign_investment " << country.details.foreignInvestment << "\n";
	}

	if (!country.details.oob)
	{
		output << "\n";
		output << "oob = \"" << (country.tag + "_OOB.txt") << "\"\n";
	}
	else if (!country.details.oob->empty())
	{
		output << "\n";
		output << "oob = \"" << *country.details.oob << "\"\n";
	}

	for (const auto& [bookmarkDate, bookmarkDetails]: country.details.bookmarks)
	{
		output << bookmarkDate << " " << bookmarkDetails << "\n";
	}

	if (country.details.holyRomanEmperor)
	{
		output << "set_country_flag = emperor_hre\n";
	}
	else if (country.details.inHRE)
	{
		output << "set_country_flag = member_hre\n";
	}
	if (country.details.celestialEmperor)
	{
		output << "set_country_flag = celestial_emperor\n";
	}
	return output;
}

void V2::Country::outputCommons(std::ostream& output)
{
	output << "graphical_culture = ";
	auto graphicalCulture = modCommons.getGraphicalCulture();
	if (graphicalCulture.empty())
		graphicalCulture = details.graphicalCulture;
	if (!graphicalCulture.empty())
		output << graphicalCulture << '\n';
	else
		output << "UsGC\n"; // default to US graphics
	
	if (nationalColors.getMapColor())
		output << "color " << *nationalColors.getMapColor() << "\n";
	else if (!modCommons.getColor().empty())
		output << "color " << modCommons.getColor() << '\n';
	else if (!details.color.empty())
		output << "color " << details.color << '\n';

	auto parties = modCommons.getParties();
	if (parties.empty())
		parties = details.parties;
	for (const auto& party: parties)
		output << party;

	auto unitNames = modCommons.getUnitNames();
	if (unitNames.empty())
		unitNames = details.unitNames;
	if (!unitNames.empty())
		output << "unit_names" << unitNames;
}

void V2::Country::outputOOB(std::ostream& output)
{
	output << "#Sphere of Influence\n";
	output << "\n";
	for (const auto& relation: relations)
	{
		output << relation.second;
	}
	output << "\n";
	output << "#Leaders\n";
	for (const auto& leader: leaders)
	{
		output << leader;
	}
	output << "\n";
	output << "#Armies\n";
	for (const auto& army: armies)
	{
		output << army;
	}
}
