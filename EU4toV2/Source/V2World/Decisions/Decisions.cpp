#include "Decisions.h"
#include "ParserHelpers.h"
#include "OSCompatibilityLayer.h"
#include "Log.h"

V2::Decisions::Decisions(const std::string& filename)
{
	registerKeyword("political_decisions", [this](const std::string& unused, std::istream& theStream) {
		Decisions newDecisions(theStream);
		decisions = newDecisions.getDecisions();
	});
	parseFile("blankMod/output/decisions/" + filename);
	clearRegisteredKeywords();
}

V2::Decisions::Decisions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V2::Decisions::registerKeys()
{
	registerRegex("[a-zA-z0-9_]+", [this](const std::string& decisionTitle, std::istream& theStream) {
		Decision newDecision(theStream);
		newDecision.setTitle(decisionTitle);
		decisions.insert(make_pair(decisionTitle, newDecision));
	});
}

void V2::Decisions::updateDecisions(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	if (const auto& theDecision = decisions.find("centralize_hre"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load centralize_hre decision";
	else
	{
		std::string potential;
		potential += "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tAND = {\n";
		potential += "\t\t\t\t\tNOT = { has_global_flag = liberal_revolutions_should_now_fire }\n";
		potential += "\t\t\t\t\thas_country_flag = emperor_hre\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t\tAND = {\n";
		potential += "\t\t\t\t\thas_country_flag = union_is_hre\n";
		potential += "\t\t\t\t\tNOT = {\n";
		potential += "\t\t\t\t\t\tcapital_scope = { continent = africa }\n";
		potential += "\t\t\t\t\t\tcapital_scope = { continent = north_america }\n";
		potential += "\t\t\t\t\t\tcapital_scope = { continent = south_america }\n";
		potential += "\t\t\t\t\t\tcapital_scope = { continent = oceania }\n";
		if (countries.find("FSA") != countries.end())
			potential += "\t\t\t\t\t\t#tag = FSA\n";
		if (countries.find("CSA") != countries.end())
			potential += "\t\t\t\t\t\ttag = CSA\n";
		if (countries.find("MGL") != countries.end())
			potential += "\t\t\t\t\t\ttag = MGL\n";
		if (countries.find("SPQ") != countries.end())
			potential += "\t\t\t\t\t\ttag = SPQ\n";
		if (countries.find("SLA") != countries.end())
			potential += "\t\t\t\t\t\ttag = SLA\n";
		potential += "\t\t\t\t\t\ttag = HRE\n";
		potential += "\t\t\t\t\t}\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = HRE\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_two_sicilies"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_two_sicilies decision";
	else
	{
		std::string potential;
		potential += "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tAND = {\n";
		potential += "\t\t\t\t\tprimary_culture = south_italian\n";
		if (countries.find("SCY") != countries.end())
			potential += "\t\t\t\t\tNOT = { exists = SCY }\n";
		if (countries.find("NAP") != countries.end())
			potential += "\t\t\t\t\tNOT = { exists = NAP }\n";
		potential += "\t\t\t\t}\n";
		if (countries.find("NAP") != countries.end())
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = NAP\n";
			potential += "\t\t\t\t\tNOT = { exists = SCY }\n";
			potential += "\t\t\t\t}\n";
		}
		if (countries.find("SCY") != countries.end())
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = SCY\n";
			potential += "\t\t\t\t\tNOT = { exists = NAP }\n";
			potential += "\t\t\t\t}\n";
		}
		potential += "\t\t\t}\n";
		if (countries.find("SIC") != countries.end())
			potential += "\t\t\tNOT = { exists = SIC }\n";
		if (countries.find("ITA") != countries.end())
			potential += "\t\t\tNOT = { exists = ITA }\n";
		if (countries.find("SPQ") != countries.end())
			potential += "\t\t\tNOT = { tag = SPQ }\n";
		if (countries.find("BYZ") != countries.end())
			potential += "\t\t\tNOT = { tag = BYZ }\n";
		if (countries.find("PAP") != countries.end())
			potential += "\t\t\tNOT = { tag = PAP }\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("mongol_khagan"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load mongol_khagan decision";
	else
	{
		if (countries.find("KHA") == countries.end())
			decisions.erase(theDecision);
	}
}