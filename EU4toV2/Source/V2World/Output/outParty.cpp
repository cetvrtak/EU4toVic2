#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Party& party)
{
	output << '\n';
	output << "party = {\n";
	output << "\tname = \"" << party.name << "\"\n";
	output << party.partyDetails;
	output << "}\n";
	return output;
}

std::ostream& mappers::operator<<(std::ostream& output, const PartyType& partyDetails)
{
	output << "\tstart_date = " << partyDetails.start_date << '\n';
	output << "\tend_date = " << partyDetails.end_date << "\n\n";
	output << "\tideology = " << partyDetails.ideology << "\n\n";
	for (const auto& policy: partyDetails.policies)
	{
		output << "\t" << policy.first << " = " << policy.second << '\n';
	}
	return output;
}
