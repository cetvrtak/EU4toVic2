#include "Decisions.h"
#include "Taiping.h"
#include "../Country/Country.h"
#include "ParserHelpers.h"
#include "OSCompatibilityLayer.h"
#include "Log.h"
#include <fstream>

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
		std::string potential = "= {\n";
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
		std::string potential = "= {\n";
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
	else if (countries.find("KHA") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("taiping_and_csa"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load taiping_and_csa decision";
	else
	{
		std::istringstream effectString(theDecision->second.getEffect());
		Taiping taiping(effectString);

		std::vector<std::string> heimaten = {"PER", "TUR", "RUS", "SLA"};
		std::string effect = "= {\n";
		for (const auto& nonCountrySpecificEffect: taiping.getNonCountrySpecificEffects())
		{
			effect += "\t\t\t" + nonCountrySpecificEffect + "\n";
		}
		for (const auto& tag: taiping.getTagEffectMap())
		{
			if (countries.find(tag.first) != countries.end()
				 && std::find(heimaten.begin(), heimaten.end(), tag.first) == heimaten.end()
				 && tag.first != "CHI")
			{
				effect += "\t\t\t" + tag.second + "\n";
			}
		}
		for (const auto& coreEffect: taiping.getCountryCores())
		{
			effect += "\t\t\t" + coreEffect + "\n";
		}

		//Heimaten
		std::vector<std::string> coresToRemove = {"GEO", "ARM", "AZB"};
		bool anythingToRemove = false;
		for (const auto& tag: coresToRemove)
		{
			if (countries.find(tag) != countries.end())
			{
				anythingToRemove = true;
				break;
			}
		}
		if (anythingToRemove)
		{
			for (const auto& tag: heimaten)
			{
				if (countries.find(tag) == countries.end())
					continue;
				effect += "\t\t\t" + tag + " = {\n";
				effect += "\t\t\t\tany_owned_province = {\n";
				effect += "\t\t\t\t\tlimit = {\n";
				effect += "\t\t\t\t\t\tNOT = {\n";
				effect += "\t\t\t\t\t\t\tregion = RUS_1090 #Georgia\n";
				effect += "\t\t\t\t\t\t\tregion = RUS_1098 #Armenia\n";
				effect += "\t\t\t\t\t\t\tregion = RUS_1102 #Azerbaijan\n";
				effect += "\t\t\t\t\t\t}\n";
				effect += "\t\t\t\t\t}\n";
				if (countries.find("GEO") != countries.end())
					effect += "\t\t\t\t\tremove_core = GEO\n";
				if (countries.find("ARM") != countries.end())
					effect += "\t\t\t\t\tremove_core = ARM\n";
				if (countries.find("AZB") != countries.end())
					effect += "\t\t\t\t\tremove_core = AZB\n";
				effect += "\t\t\t\t}\n";
				effect += "\t\t\t}\n";
			}
		}

		if (countries.find("CHI") != countries.end())
		{
			effect += "\t\t\tCHI = {\n";
			effect += "\t\t\t\tany_owned_province = {\n";
			effect += "\t\t\t\t\tlimit = {\n";
			effect += "\t\t\t\t\t\tNOT = {\n";
			if (countries.find("MGL") != countries.end())
				effect += "\t\t\t\t\t\t\tis_core = MGL\n";
			effect += "\t\t\t\t\t\t\tregion = CHI_2608 #Mongolia proper\n";
			effect += "\t\t\t\t\t\t}\n";
			effect += "\t\t\t\t\t}\n";
			effect += "\t\t\t\t}\n";
			if (countries.find("KHA") != countries.end())
				effect += "\t\t\t\tremove_core = KHA\n";
			if (countries.find("MGL") != countries.end())
				effect += "\t\t\t\tremove_core = MGL\n";
			effect += "\t\t\t\tadd_core = IMG\n";
			effect += "\t\t\t}\n";
		}

		effect += "\t\t\tset_global_flag = taiping_and_csa\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_china"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_china decision";
	else if (countries.find("CHI") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_netherlands"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_netherlands decision";
	else if (countries.find("NET") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = dutch\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = NET\n";
		if (countries.find("FRA") != countries.end())
			potential += "\t\t\t\ttag = FRA\n";
		if (countries.find("SPQ") != countries.end())
			potential += "\t\t\t\ttag = SPQ\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_belgium"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_belgium decision";
	else if (countries.find("BEL") == countries.end()
			 || (countries.find("FLA") == countries.end() && countries.find("WLL") == countries.end()))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = flemish\n";
		potential += "\t\t\t\tprimary_culture = wallonian\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = BEL\n";
		if (countries.find("LUX") != countries.end())
			potential += "\t\t\t\ttag = LUX\n";
		if (countries.find("SPQ") != countries.end())
			potential += "\t\t\t\ttag = SPQ\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tOR = {\n";
		if (countries.find("FLA") != countries.end())
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = FLA\n";
			if (countries.find("WLL") != countries.end())
				potential += "\t\t\t\t\tWLL = { exists = no }\n";
			potential += "\t\t\t\t}\n";
		}
		if (countries.find("WLL") != countries.end())
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = WLL\n";
			if (countries.find("FLA") != countries.end())
				potential += "\t\t\t\t\tFLA = { exists = no }\n";
			potential += "\t\t\t\t}\n";
		}
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_spain"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_spain decision";
	else if (countries.find("SPA") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = spanish\n";
		potential += "\t\t\t\tprimary_culture = catalan\n";
		potential += "\t\t\t\tprimary_culture = galician\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = SPA\n";
		if (countries.find("SPC") != countries.end())
			potential += "\t\t\t\texists = SPC\n";
		if (countries.find("CAT") != countries.end())
			potential += "\t\t\t\ttag = CAT\n";
		if (countries.find("SPQ") != countries.end())
			potential += "\t\t\t\ttag = SPQ\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tprestige = 5\n";
		allow += "\t\t\tOR = {\n";
		if (countries.find("SPC") != countries.end())
		{
			allow += "\t\t\t\tSPC = { #Carlist Spain\n";
			allow += "\t\t\t\t\tall_core = {\n";
			allow += "\t\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\t\towned_by = THIS\n";
			allow += "\t\t\t\t\t\t\towner = {\n";
			allow += "\t\t\t\t\t\t\t\tin_sphere = THIS\n";
			allow += "\t\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t}\n";
			allow += "\t\t\t\t}\n";
		}
		allow += "\t\t\t\tSPA = { #Default Spain\n";
		allow += "\t\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t\t}\n";
		allow += "\t\t\t\t\t\t}\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = THIS\n";
		effect += "\t\t\t\t\tNOT = {\n";
		effect += "\t\t\t\t\t\ttag = THIS\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t\tcapital_scope = { continent = europe }\n";
		effect += "\t\t\t\t\tOR = {\n";
		effect += "\t\t\t\t\t\tprimary_culture = spanish\n";
		effect += "\t\t\t\t\t\tprimary_culture = catalan\n";
		effect += "\t\t\t\t\t\tprimary_culture = galician\n";
		effect += "\t\t\t\t\t\tprimary_culture = basque\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tannex_to = THIS\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = SPA\n";
		if (countries.find("SPC") != countries.end())
			effect += "\t\t\t\tremove_core = SPC #Carlist Spain\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = SPA\n";
		effect += "\t\t}\t\t\t\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_ukraine"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_ukraine decision";
	else if (countries.find("UKR") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = ukrainian\n";
		potential += "\t\t\tNOT = {\n";
		if (countries.find("RUT") != countries.end())
			potential += "\t\t\t\texists = RUT\n";
		potential += "\t\t\t\texists = UKR\n";
		if (countries.find("RUS") != countries.end())
			potential += "\t\t\t\ttag = RUS\n";
		if (countries.find("SLA") != countries.end())
			potential += "\t\t\t\ttag = SLA\n";
		if (countries.find("CRI") != countries.end())
			potential += "\t\t\t\ttag = CRI\n";
		if (countries.find("SPQ") != countries.end())
			potential += "\t\t\t\ttag = SPQ\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_philippines"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_philippines decision";
	else if (countries.find("PHI") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = PHI\n";
		if (countries.find("SPA") != countries.end())
			potential += "\t\t\t\ttag = SPA\n";
		if (countries.find("USA") != countries.end())
			potential += "\t\t\t\ttag = USA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tprimary_culture = filipino\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_malaya"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_malaya decision";
	else if (countries.find("MLY") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_aztec"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_aztec decision";
	else if (countries.find("AZT") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = nahua\n";
		potential += "\t\t\t\tprimary_culture = zapotec\n";
		potential += "\t\t\t\tprimary_culture = tarascan\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = AZT\n";
		if (countries.find("MEX") != countries.end())
			potential += "\t\t\t\texists = MEX\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_usa"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_usa decision";
	else if (countries.find("USA") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = yankee\n";
		potential += "\t\t\t\tprimary_culture = dixie\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = USA\n";
		if (countries.find("CSA") != countries.end())
			potential += "\t\t\t\texists = CSA\n";
		if (countries.find("TEX") != countries.end())
			potential += "\t\t\t\ttag = TEX\n";
		if (countries.find("CAL") != countries.end())
			potential += "\t\t\t\ttag = CAL\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_southafrica"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_southafrica decision";
	else if (countries.find("SAF") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_madagascar"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_madagascar decision";
	else if (countries.find("MAD") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("mandate_of_heaven"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load mandate_of_heaven decision";
	else if (countries.find("CHI") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("become_hyderabad"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load become_hyderabad decision";
	else if (countries.find("HYD") == countries.end() || countries.find("DEC") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("become_slovenia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load become_slovenia decision";
	else if (countries.find("SLO") == countries.end()
			 || (countries.find("AQU") == countries.end() && countries.find("CRT") == countries.end()))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		if (countries.find("AQU") != countries.end())
			potential += "\t\t\t\ttag = AQU\n";
		if (countries.find("CRT") != countries.end())
			potential += "\t\t\t\ttag = CRT\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tis_culture_group = south_slavic\n";
		potential += "\t\t\tNOT = { exists = SLO }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("latin_empire_upgrade"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load latin_empire_upgrade decision";
	else if (countries.find("SPQ") == countries.end() || countries.find("LTN") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\ttag = LTN\n";
		if (countries.find("BYZ") != countries.end())
			potential += "\t\t\tNOT = { exists = BYZ }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("become_longobardia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load become_longobardia decision";
	else if (countries.find("LGB") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = lombard\n";
		potential += "\t\t\tNOT = { exists = LGB }\n";
		if (countries.find("LOM") != countries.end())
			potential += "\t\t\tNOT = { exists = LOM }\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_japan"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_japan decision";
	else if (countries.find("JAP") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tprestige = 5\n";
		allow += "\t\t\tJAP = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tis_empty = yes\n";
		if (countries.find("RYU") != countries.end())
			allow += "\t\t\t\t\t\towned_by = RYU\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t\tprimary_culture = japanese\n";
		allow += "\t\t\t\t\t\t}\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tNOT = { primary_culture = japanese }\n";
		allow += "\t\t\t\t\t\t}\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("form_the_undeads"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_the_undeads decision";
	else if (countries.find("UND") == countries.end() || countries.find("ZOM") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_ireland"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_ireland decision";
	else if (countries.find("IRE") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = irish\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = IRE\n";
		if (countries.find("ENG") != countries.end())
			potential += "\t\t\t\ttag = ENG\n";
		if (countries.find("ENL") != countries.end())
			potential += "\t\t\t\ttag = ENL\n";
		if (countries.find("SCO") != countries.end())
			potential += "\t\t\t\ttag = SCO\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tprestige = 5\n";
		allow += "\t\t\t263 = {\n";
		allow += "\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t\tIRE = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tNOT = { province_id = 263 }\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (countries.find("ENG") != countries.end())
			allow += "\t\t\t\t\t\towned_by = ENG\n";
		if (countries.find("ENL") != countries.end())
			allow += "\t\t\t\t\t\towned_by = ENL\n";
		if (countries.find("SCO") != countries.end())
			allow += "\t\t\t\t\t\towned_by = SCO\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("form_greece"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_greece decision";
	else if (countries.find("GRE") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = greek\n";
		potential += "\t\t\tNOT = {\n";
		if (countries.find("BYZ") != countries.end())
			potential += "\t\t\t\texists = BYZ\n";
		if (countries.find("SPQ") != countries.end())
			potential += "\t\t\t\texists = SPQ\n";
		if (countries.find("GRE") != countries.end())
			potential += "\t\t\t\texists = GRE\n";
		if (countries.find("TUR") != countries.end())
			potential += "\t\t\t\ttag = TUR\n";
		if (countries.find("ITA") != countries.end())
			potential += "\t\t\t\ttag = ITA\n";
		if (countries.find("MCD") != countries.end())
			potential += "\t\t\t\ttag = MCD\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tprestige = 5\n";
		allow += "\t\t\t834 = {\n";
		allow += "\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t\tGRE = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tNOT = { province_id = 834 }\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (countries.find("TUR") != countries.end())
			allow += "\t\t\t\t\t\towned_by = TUR\n";
		if (countries.find("MCD") != countries.end())
			allow += "\t\t\t\t\t\towned_by = MCD\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("form_turkestan"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_turkestan decision";
	else if (countries.find("TKS") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tcivilized = yes\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = turkmen\n";
		potential += "\t\t\t\tprimary_culture = uzbek\n";
		potential += "\t\t\t\tprimary_culture = kirgiz\n";
		potential += "\t\t\t\tprimary_culture = tajik\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = TKS\n";
		if (countries.find("KAZ") != countries.end())
			potential += "\t\t\t\ttag = KAZ\n";
		if (countries.find("XIN") != countries.end())
			potential += "\t\t\t\ttag = XIN\n";
		if (countries.find("PER") != countries.end())
			potential += "\t\t\t\ttag = PER\n";
		if (countries.find("RUS") != countries.end())
			potential += "\t\t\t\ttag = RUS\n";
		if (countries.find("SLA") != countries.end())
			potential += "\t\t\t\ttag = SLA\n";
		if (countries.find("CHI") != countries.end())
			potential += "\t\t\t\ttag = CHI\n";
		if (countries.find("YUA") != countries.end())
			potential += "\t\t\t\ttag = YUA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tprestige = 5\n";
		allow += "\t\t\tTKS = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (countries.find("KAZ") != countries.end())
			allow += "\t\t\t\t\t\towned_by = KAZ\n";
		if (countries.find("XIN") != countries.end())
			allow += "\t\t\t\t\t\towned_by = XIN\n";
		if (countries.find("PER") != countries.end())
			allow += "\t\t\t\t\t\towned_by = PER\n";
		if (countries.find("RUS") != countries.end())
			allow += "\t\t\t\t\t\towned_by = RUS\n";
		if (countries.find("SLA") != countries.end())
			allow += "\t\t\t\t\t\towned_by = SLA\n";
		if (countries.find("CHI") != countries.end())
			allow += "\t\t\t\t\t\towned_by = CHI\n";
		if (countries.find("YUA") != countries.end())
			allow += "\t\t\t\t\t\towned_by = YUA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("form_occitania"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_occitania decision";
	else if (countries.find("OCC") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = occitan\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = OCC\n";
		if (countries.find("FRA") != countries.end())
			potential += "\t\t\t\ttag = FRA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tprestige = 5\n";
		allow += "\t\t\t455 = {\n";
		allow += "\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t\tOCC = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tNOT = { province_id = 455 }\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (countries.find("FRA") != countries.end())
			allow += "\t\t\t\t\t\towned_by = FRA\n";
		if (countries.find("SPA") != countries.end())
			allow += "\t\t\t\t\t\towned_by = SPA\n";
		if (countries.find("CAT") != countries.end())
			allow += "\t\t\t\t\t\towned_by = CAT\n";
		if (countries.find("NAV") != countries.end())
			allow += "\t\t\t\t\t\towned_by = NAV\n";
		if (countries.find("PAP") != countries.end())
			allow += "\t\t\t\t\t\towned_by = PAP\n";
		if (countries.find("ITA") != countries.end())
			allow += "\t\t\t\t\t\towned_by = ITA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = THIS\n";
		effect += "\t\t\t\t\tNOT = {\n";
		effect += "\t\t\t\t\t\ttag = THIS\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t\tcapital_scope = { continent = europe }\n";
		effect += "\t\t\t\t\tprimary_culture = occitan\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tannex_to = THIS\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = OCC\n";
		if (countries.find("FRA") != countries.end())
			effect += "\t\t\t\tremove_core = FRA\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = OCC\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_the_suebi"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_the_suebi decision";
	else if (countries.find("SUE") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = suebi\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = SUE\n";
		if (countries.find("GLC") != countries.end())
			potential += "\t\t\t\ttag = GLC\n";
		if (countries.find("POR") != countries.end())
			potential += "\t\t\t\ttag = POR\n";
		if (countries.find("SPA") != countries.end())
			potential += "\t\t\t\ttag = SPA\n";
		if (countries.find("SPC") != countries.end())
			potential += "\t\t\t\ttag = SPC\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = THIS\n";
		effect += "\t\t\t\t\tNOT = {\n";
		effect += "\t\t\t\t\t\ttag = THIS\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t\tcapital_scope = { continent = europe }\n";
		effect += "\t\t\t\t\tprimary_culture = suebi\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tannex_to = THIS\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = SUE\n";
		if (countries.find("SPA") != countries.end())
			effect += "\t\t\t\tremove_core = SPA\n";
		if (countries.find("POR") != countries.end())
			effect += "\t\t\t\tremove_core = POR\n";
		if (countries.find("GLC") != countries.end())
			effect += "\t\t\t\tremove_core = GLC\n";
		if (countries.find("SPC") != countries.end())
			effect += "\t\t\t\tremove_core = SPC\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = SUE\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_visigothia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_visigothia decision";
	else if (countries.find("VGO") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = visigothic\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = VGO\n";
		if (countries.find("POR") != countries.end())
			potential += "\t\t\t\ttag = POR\n";
		if (countries.find("SPA") != countries.end())
			potential += "\t\t\t\ttag = SPA\n";
		if (countries.find("SPC") != countries.end())
			potential += "\t\t\t\ttag = SPC\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = THIS\n";
		effect += "\t\t\t\t\tNOT = {\n";
		effect += "\t\t\t\t\t\ttag = THIS\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t\tcapital_scope = { continent = europe }\n";
		effect += "\t\t\t\t\tprimary_culture = visigothic\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tannex_to = THIS\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = VGO\n";
		if (countries.find("SPA") != countries.end())
			effect += "\t\t\t\tremove_core = SPA\n";
		if (countries.find("POR") != countries.end())
			effect += "\t\t\t\tremove_core = POR\n";
		if (countries.find("SPC") != countries.end())
			effect += "\t\t\t\tremove_core = SPC\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = VGO\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_siberia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_siberia decision";
	else if (countries.find("SIB") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = siberian\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = SIB\n";
		if (countries.find("RUS") != countries.end())
			potential += "\t\t\t\ttag = RUS\n";
		if (countries.find("SLA") != countries.end())
			potential += "\t\t\t\ttag = SLA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = THIS\n";
		effect += "\t\t\t\t\tNOT = {\n";
		effect += "\t\t\t\t\t\ttag = THIS\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t\tcapital_scope = { continent = asia }\n";
		effect += "\t\t\t\t\tprimary_culture = siberian\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tannex_to = THIS\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = SIB\n";
		if (countries.find("RUS") != countries.end())
			effect += "\t\t\t\tremove_core = RUS\n";
		if (countries.find("SLA") != countries.end())
			effect += "\t\t\t\tremove_core = SLA\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = SIB\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_indochina"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_indochina decision";
	else if (countries.find("IDO") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_anglo_saxony"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_anglo_saxony decision";
	else if (countries.find("AES") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = anglo_saxon\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = AES\n";
		if (countries.find("ENG") != countries.end())
			potential += "\t\t\t\ttag = ENG\n";
		if (countries.find("ENL") != countries.end())
			potential += "\t\t\t\ttag = ENL\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string effect = "= {\n";
		effect += "\t\t\tcivilized = no\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = THIS\n";
		effect += "\t\t\t\t\tNOT = {\n";
		effect += "\t\t\t\t\t\ttag = THIS\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t\tcapital_scope = { continent = europe }\n";
		effect += "\t\t\t\t\tprimary_culture = anglo_saxon\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tannex_to = THIS\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = AES\n";
		if (countries.find("ENG") != countries.end())
			effect += "\t\t\t\tremove_core = ENG\n";
		if (countries.find("ENL") != countries.end())
			effect += "\t\t\t\tremove_core = ENL\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = AES\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_frankia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_frankia decision";
	else if (countries.find("FRK") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = frankish\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = FRK\n";
		if (countries.find("FRA") != countries.end())
			potential += "\t\t\t\ttag = FRA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = THIS\n";
		effect += "\t\t\t\t\tNOT = {\n";
		effect += "\t\t\t\t\t\ttag = THIS\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t\tcapital_scope = { continent = europe }\n";
		effect += "\t\t\t\t\tprimary_culture = frankish\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tannex_to = THIS\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = FRK\n";
		if (countries.find("FRA") != countries.end())
			effect += "\t\t\t\tremove_core = FRA\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = FRK\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_britannia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_britannia decision";
	else if (countries.find("ENR") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_gallia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_gallia decision";
	else if (countries.find("FRR") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_laessinia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_laessinia decision";
	else if (countries.find("AUR") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_euskadi"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_euskadi decision";
	else if (countries.find("BSQ") == countries.end() ||
			 (countries.find("NAV") == countries.end() && countries.find("GYN") == countries.end()))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		if (countries.find("NAV") != countries.end())
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = NAV\n";
			potential += "\t\t\t\t\tgovernment = democracy\n";
			potential += "\t\t\t\t}\n";
		}
		if (countries.find("GYN") != countries.end())
		{
			potential += "\t\t\t\tAND = { #Gascony, if Navarre doesn't exist\n";
			potential += "\t\t\t\t\ttag = GYN\n";
			potential += "\t\t\t\t\tNOT = { exists = NAV }\n";
			potential += "\t\t\t\t}\n";
		}
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = BSQ\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tprestige = 5\n";
		allow += "\t\t\t492 = {\n";
		allow += "\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t\tBSQ = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tNOT = { province_id = 492 }\n";
		allow += "\t\t\t\t\tOR = {\n";
		if (countries.find("GYN") != countries.end())
			allow += "\t\t\t\t\t\towned_by = GYN\n";
		if (countries.find("NAV") != countries.end())
			allow += "\t\t\t\t\t\towned_by = NAV\n";
		if (countries.find("SPA") != countries.end())
			allow += "\t\t\t\t\t\towned_by = SPA\n";
		if (countries.find("FRA") != countries.end())
			allow += "\t\t\t\t\t\towned_by = FRA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = BSQ\n";
		if (countries.find("SPA") != countries.end())
			effect += "\t\t\t\tremove_core = SPA\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tchange_tag = BSQ\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_nepal"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_nepal decision";
	else if (countries.find("NEP") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_persia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_persia decision";
	else if (countries.find("PER") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = persian\n";
		potential += "\t\t\t\tprimary_culture = tajik\n";
		potential += "\t\t\t\tprimary_culture = gypsy\n";
		potential += "\t\t\t\tprimary_culture = kurdish\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = PER\n";
		if (countries.find("ARA") != countries.end())
			potential += "\t\t\t\ttag = ARA\n";
		if (countries.find("MUG") != countries.end())
			potential += "\t\t\t\ttag = MUG\n";
		if (countries.find("TIM") != countries.end())
			potential += "\t\t\t\ttag = TIM\n";
		if (countries.find("ALL") != countries.end())
			potential += "\t\t\t\ttag = ALL\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tchange_tag = PER\n";
		effect += "\t\t\tcapital = 1121 #Tehran\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = PER\n";
		effect += "\t\t\t\t\tNOT = {\n";
		effect += "\t\t\t\t\t\ttag = PER\n";
		if (countries.find("TAJ") != countries.end())
			effect += "\t\t\t\t\t\ttag = TAJ #Tajikistan would be nonsense\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t\tOR = {\n";
		effect += "\t\t\t\t\t\tprimary_culture = persian\n";
		effect += "\t\t\t\t\t\tprimary_culture = tajik\n";
		effect += "\t\t\t\t\t\tprimary_culture = gypsy\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tannex_to = PER\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_owned = { add_core = PER }\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_kurdistan"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_kurdistan decision";
	else if (countries.find("KDS") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = kurdish\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = KDS\n";
		if (countries.find("PER") != countries.end())
			potential += "\t\t\t\ttag = PER\n";
		if (countries.find("ARA") != countries.end())
			potential += "\t\t\t\ttag = ARA\n";
		if (countries.find("TUR") != countries.end())
			potential += "\t\t\t\ttag = TUR\n";
		if (countries.find("IRQ") != countries.end())
			potential += "\t\t\t\ttag = IRQ\n";
		if (countries.find("SYR") != countries.end())
			potential += "\t\t\t\ttag = SYR\n";
		if (countries.find("ALL") != countries.end())
			potential += "\t\t\t\ttag = ALL\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("provincia_britannia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load provincia_britannia decision";
	else if (countries.find("ENR") == countries.end() && countries.find("SPQ") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("provincia_gallia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load provincia_gallia decision";
	else if (countries.find("FRR") == countries.end() && countries.find("SPQ") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("provincia_noricum"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load provincia_noricum decision";
	else if (countries.find("AUR") == countries.end() && countries.find("SPQ") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_LVN"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_LVN decision";
	else if (countries.find("LVN") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = LVN\n";
		if (countries.find("ITA") != countries.end())
			potential += "\t\t\t\ttag = ITA\n";
		if (countries.find("ITP") != countries.end())
			potential += "\t\t\t\ttag = ITP\n";
		if (countries.find("SPQ") != countries.end())
			potential += "\t\t\t\ttag = SPQ\n";
		if (countries.find("BYZ") != countries.end())
			potential += "\t\t\t\ttag = BYZ\n";
		if (countries.find("LGB") != countries.end())
			potential += "\t\t\t\ttag = LGB\n";
		if (countries.find("PAP") != countries.end())
			potential += "\t\t\t\ttag = PAP\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tOR = {\n";
		if (countries.find("LOM") != countries.end())
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\tNOT = { exists = VEN }\n";
			potential += "\t\t\t\t\ttag = LOM\n";
			potential += "\t\t\t\t}\n";
		}
		if (countries.find("VEN") != countries.end())
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\tNOT = { exists = LOM }\n";
			potential += "\t\t\t\t\ttag = VEN\n";
			potential += "\t\t\t\t}\n";
		}
		potential += "\t\t\t\tAND = {\n";
		if (countries.find("VEN") != countries.end())
			potential += "\t\t\t\t\tNOT = { exists = VEN }\n";
		if (countries.find("LOM") != countries.end())
			potential += "\t\t\t\t\tNOT = { exists = LOM }\n";
		potential += "\t\t\t\t\tOR = {\n";
		potential += "\t\t\t\t\t\tcapital_scope = { state_id = 726 }\n";
		potential += "\t\t\t\t\t\tcapital_scope = { state_id = 729 }\n";
		potential += "\t\t\t\t\t\tcapital_scope = { state_id = 734 }\n";
		potential += "\t\t\t\t\t}\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tAUS_726 = { owned_by = THIS }\n";
		allow += "\t\t\tAUS_729 = { owned_by = THIS }\n";
		if (countries.find("LOM") != countries.end())
		{
			allow += "\t\t\tLOM = {\n";
			allow += "\t\t\t\tall_core = { owned_by = THIS }\n";
			allow += "\t\t\t}\n";
		}
		if (countries.find("VEN") != countries.end())
		{
			allow += "\t\t\tVEN = {\n";
			allow += "\t\t\t\tall_core = { owned_by = THIS }\n";
			allow += "\t\t\t}\n";
		}
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tAUS_726 = { add_core = LVN }\n";
		effect += "\t\t\tAUS_729 = { add_core = LVN }\n";
		if (countries.find("LOM") != countries.end())
		{
			effect += "\t\t\tLOM = {\n";
			effect += "\t\t\t\tall_core = { add_core = LVN }\n";
			effect += "\t\t\t}\n";
		}
		if (countries.find("VEN") != countries.end())
		{
			effect += "\t\t\tVEN = {\n";
			effect += "\t\t\t\tall_core = { add_core = LVN }\n";
			effect += "\t\t\t}\n";
		}
		effect += "\t\t\tchange_tag = LVN\n";
		effect += "\t\t\tLVN = {\n";
		effect += "\t\t\t\tadd_accepted_culture = lombard\n";
		effect += "\t\t\t\tadd_accepted_culture = north_italian\n";
		effect += "\t\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("vassal_LVN"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load vassal_LVN decision";
	else if (countries.find("LVN") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = LVN\n";
		if (countries.find("ITA") != countries.end())
			potential += "\t\t\t\ttag = ITA\n";
		if (countries.find("ITP") != countries.end())
			potential += "\t\t\t\ttag = ITP\n";
		if (countries.find("SPQ") != countries.end())
			potential += "\t\t\t\ttag = SPQ\n";
		if (countries.find("BYZ") != countries.end())
			potential += "\t\t\t\ttag = BYZ\n";
		if (countries.find("LGB") != countries.end())
			potential += "\t\t\t\ttag = LGB\n";
		if (countries.find("PAP") != countries.end())
			potential += "\t\t\t\ttag = PAP\n";
		if (countries.find("ALI") != countries.end())
			potential += "\t\t\t\ttag = ALI #Yeah...\n";
		if (countries.find("VEN") != countries.end())
			potential += "\t\t\t\texists = VEN\n";
		if (countries.find("LOM") != countries.end())
			potential += "\t\t\t\texists = LOM\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\tcapital_scope = { state_id = 726 }\n";
		potential += "\t\t\t\tcapital_scope = { state_id = 729 }\n";
		potential += "\t\t\t\tcapital_scope = { state_id = 734 }\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\tprimary_culture = north_italian\n";
		potential += "\t\t\t\tprimary_culture = lombard\n";
		potential += "\t\t\t\tprimary_culture = south_italian\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tAUS_726 = { owned_by = THIS }\n";
		potential += "\t\t\t\tAUS_729 = { owned_by = THIS }\n";
		if (countries.find("LOM") != countries.end())
		{
			potential += "\t\t\t\tLOM = {\n";
			potential += "\t\t\t\t\tany_core = { owned_by = THIS }\n";
			potential += "\t\t\t\t}\n";
		}
		if (countries.find("VEN") != countries.end())
		{
			potential += "\t\t\t\tVEN = {\n";
			potential += "\t\t\t\t\tany_core = { owned_by = THIS }\n";
			potential += "\t\t\t\t}\n";
		}
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tAUS_726 = { owned_by = THIS }\n";
		allow += "\t\t\tAUS_729 = { owned_by = THIS }\n";
		if (countries.find("LOM") != countries.end())
		{
			allow += "\t\t\tLOM = {\n";
			allow += "\t\t\t\tall_core = { owned_by = THIS }\n";
			allow += "\t\t\t}\n";
		}
		if (countries.find("VEN") != countries.end())
		{
			allow += "\t\t\tVEN = {\n";
			allow += "\t\t\t\tall_core = { owned_by = THIS }\n";
			allow += "\t\t\t}\n";
		}
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tAUS_726 = { add_core = LVN secede_province = LVN }\n";
		effect += "\t\t\tAUS_729 = { add_core = LVN secede_province = LVN }\n";
		if (countries.find("LOM") != countries.end())
		{
			effect += "\t\t\tLOM = {\n";
			effect += "\t\t\t\tall_core = { add_core = LVN secede_province = LVN }\n";
			effect += "\t\t\t}\n";
		}
		if (countries.find("VEN") != countries.end())
		{
			effect += "\t\t\tVEN = {\n";
			effect += "\t\t\t\tall_core = { add_core = LVN secede_province = LVN }\n";
			effect += "\t\t\t}\n";
		}
		effect += "\t\t\tLVN = {\n";
		effect += "\t\t\t\tadd_accepted_culture = lombard\n";
		effect += "\t\t\t\tadd_accepted_culture = north_italian\n";
		effect += "\t\t\t\tcivilized = yes\n";
		effect += "\t\t\t\treligion = ROOT\n";
		effect += "\t\t\t}\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("EGY_copt"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load EGY_copt decision";
	else if (countries.find("CEG") == countries.end() || countries.find("EGY") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("EGY_not_copt"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load EGY_not_copt decision";
	else if (countries.find("CEG") == countries.end() || countries.find("EGY") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_ITP"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_ITP decision";
	else if (countries.find("ITP") == countries.end() || countries.find("PAP") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\ttag = PAP\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\thas_country_flag = post_colonial_country\n";
		if (countries.find("ITA") != countries.end())
			potential += "\t\t\t\texists = ITA\n";
		potential += "\t\t\t}\n";
		if (countries.find("SPQ") != countries.end())
			potential += "\t\t\tNOT = { tag = SPQ }\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\tprestige = 45\n";
		allow += "\t\t\tnationalism_n_imperialism = 1\n";
		allow += "\t\t\tis_greater_power = yes\n";
		allow += "\t\t\twar = no\n";
		if (countries.find("ITA") != countries.end())
		{
			allow += "\t\t\tITA = {\n";
			allow += "\t\t\t\tall_core = {\n";
			allow += "\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\towned_by = THIS\n";
			if (countries.find("VEN") != countries.end())
			{
				allow += "\t\t\t\t\t\tAND = {\n";
				allow += "\t\t\t\t\t\t\tis_core = VEN\n";
				allow += "\t\t\t\t\t\t\towner = {\n";
				allow += "\t\t\t\t\t\t\t\tNOT = { is_culture_group = italian }\n";
				allow += "\t\t\t\t\t\t\t}\n";
				allow += "\t\t\t\t\t\t}\n";
			}
			if (countries.find("LOM") != countries.end())
			{
				allow += "\t\t\t\t\t\tAND = {\n";
				allow += "\t\t\t\t\t\t\tis_core = LOM\n";
				allow += "\t\t\t\t\t\t\towner = {\n";
				allow += "\t\t\t\t\t\t\t\tNOT = { is_culture_group = italian }\n";
				allow += "\t\t\t\t\t\t\t}\n";
				allow += "\t\t\t\t\t\t}\n";
			}
			allow += "\t\t\t\t\t\towner = {\n";
			allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
			allow += "\t\t\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\t\t\tis_vassal = no\n";
			allow += "\t\t\t\t\t\t\t\tvassal_of = THIS\n";
			allow += "\t\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t\t\twar = no\n";
			allow += "\t\t\t\t\t\t\tis_greater_power = no\n";
			allow += "\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t}\n";
			allow += "\t\t\t\t}\n";
			allow += "\t\t\t}\n";
		}
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 20\n";
		effect += "\t\t\tall_core = { remove_core = THIS }\n";
		if (countries.find("ITA") != countries.end())
		{
			effect += "\t\t\tITA = {\n";
			effect += "\t\t\t\tall_core = {\n";
			effect += "\t\t\t\t\tremove_core = ITA\n";
			effect += "\t\t\t\t\tadd_core = ITP\n";
			effect += "\t\t\t\t}\n";
			effect += "\t\t\t}\n";
		}
		effect += "\t\t\tchange_tag = ITP\n";
		effect += "\t\t\tcapital = 749\n";
		effect += "\t\t\tadd_accepted_culture = north_italian\n";
		effect += "\t\t\tadd_accepted_culture = south_italian\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tis_culture_group = italian\n";
		effect += "\t\t\t\t\tin_sphere = THIS\n";
		effect += "\t\t\t\t\tNOT = { has_country_flag = post_colonial_country }\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tcountry_event = 11101\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tNOT = { is_culture_group = italian }\n";
		effect += "\t\t\t\t\tOR = {\n";
		effect += "\t\t\t\t\t\thas_country_flag = objects_to_italy\n";
		if (countries.find("FRA") != countries.end())
			effect += "\t\t\t\t\t\ttag = FRA\n";
		if (countries.find("AUS") != countries.end())
			effect += "\t\t\t\t\t\ttag = AUS\n";
		if (countries.find("KUK") != countries.end())
			effect += "\t\t\t\t\t\ttag = KUK\n";
		effect += "\t\t\t\t\t\tAND = {\n";
		effect += "\t\t\t\t\t\t\tneighbour = ITP\n";
		effect += "\t\t\t\t\t\t\tis_greater_power = yes\n";
		effect += "\t\t\t\t\t\t}\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tcountry_event = 96165\n";
		effect += "\t\t\t}\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_SPQR"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_SPQR decision";
	else if (countries.find("SPQ") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		if (countries.find("BYZ") != countries.end())
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = BYZ\n";
			if (countries.find("LTN") != countries.end())
				potential += "\t\t\t\t\tNOT = { exists = LTN }\n";
			if (countries.find("HRE") != countries.end())
				potential += "\t\t\t\t\tNOT = { exists = HRE }\n";
			potential += "\t\t\t\t}\n";
		}
		if (countries.find("LTN") != countries.end())
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = LTN\n";
			if (countries.find("BYZ") != countries.end())
				potential += "\t\t\t\t\tNOT = { exists = BYZ }\n";
			if (countries.find("HRE") != countries.end())
				potential += "\t\t\t\t\tNOT = { exists = HRE }\n";
			potential += "\t\t\t\t}\n";
		}
		if (countries.find("HRE") != countries.end())
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = HRE\n";
			if (countries.find("LTN") != countries.end())
				potential += "\t\t\t\t\tNOT = { exists = LTN }\n";
			if (countries.find("BYZ") != countries.end())
				potential += "\t\t\t\t\tNOT = { exists = BYZ }\n";
			potential += "\t\t\t\t}\n";
		}
		potential += "\t\t\t\tprimary_culture = gaul\n";
		potential += "\t\t\t\tprimary_culture = laessin\n";
		potential += "\t\t\t\tprimary_culture = brithenig\n";
		potential += "\t\t\t\tprimary_culture = latin\n";
		potential += "\t\t\t\tprimary_culture = xliponian\n";
		potential += "\t\t\t\tprimary_culture = wenedyk\n";
		potential += "\t\t\t\tprimary_culture = bazramani\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { exists = SPQ }\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 50\n";
		effect += "\t\t\tall_core = { remove_core = THIS add_core = SPQ }\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = SPQ\n";
		if (countries.find("BYZ") != countries.end())
			effect += "\t\t\t\tremove_core = BYZ\n";
		if (countries.find("LTN") != countries.end())
			effect += "\t\t\t\tremove_core = LTN\n";
		if (countries.find("HRE") != countries.end())
			effect += "\t\t\t\tremove_core = HRE\n";
		if (countries.find("ITA") != countries.end())
			effect += "\t\t\t\tremove_core = ITA\n";
		if (countries.find("ITP") != countries.end())
			effect += "\t\t\t\tremove_core = ITP\n";
		if (countries.find("JER") != countries.end())
			effect += "\t\t\t\tremove_core = JER\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tchange_tag = SPQ\n";
		effect += "\t\t\tcapital = 749\n";
		effect += "\t\t\tadd_accepted_culture = latin\n";
		effect += "\t\t\tadd_accepted_culture = greek #Byzantines\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = THIS\n";
		effect += "\t\t\t\t\tNOT = { has_country_flag = post_colonial_country }\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tcountry_event = 11101\n";
		effect += "\t\t\t}\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_yemen"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_yemen decision";
	else if (countries.find("YEM") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tis_greater_power = no\n";
		potential += "\t\t\tprimary_culture = bedouin\n";
		potential += "\t\t\tcapital_scope = {\n";
		potential += "\t\t\t\tstate_id = 1173 #Yemen\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = YEM\n";
		if (countries.find("ARA") != countries.end())
			potential += "\t\t\t\ttag = ARA\n";
		if (countries.find("ALL") != countries.end())
			potential += "\t\t\t\ttag = ALL\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_burma"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_burma decision";
	else if (countries.find("BUR") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = burmese\n";
		potential += "\t\t\t\tprimary_culture = shan\n";
		potential += "\t\t\t\tprimary_culture = kachin\n";
		potential += "\t\t\t\tprimary_culture = karen\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = BUR\n";
		if (countries.find("SHA") != countries.end())
			potential += "\t\t\t\ttag = SHA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tprestige = 5\n";
		allow += "\t\t\tBUR = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\towner = { in_sphere = THIS }\n";
		if (countries.find("SHA") != countries.end())
			allow += "\t\t\t\t\t\towned_by = SHA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tchange_tag = BUR\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = BUR\n";
		effect += "\t\t\t\t\tNOT = {\n";
		effect += "\t\t\t\t\t\ttag = BUR\n";
		if (countries.find("SHA") != countries.end())
			effect += "\t\t\t\t\t\ttag = SHA\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t\tOR = {\n";
		effect += "\t\t\t\t\t\tprimary_culture = burmese\n";
		effect += "\t\t\t\t\t\tprimary_culture = shan\n";
		effect += "\t\t\t\t\t\tprimary_culture = kachin\n";
		effect += "\t\t\t\t\t\tprimary_culture = karen\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tannex_to = BUR\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_owned = { add_core = BUR }\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_shan"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_shan decision";
	else if (countries.find("SHA") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = shan\n";
		potential += "\t\t\tNOT = {\n";
		if (countries.find("BUR") != countries.end())
			potential += "\t\t\t\ttag = BUR\n";
		potential += "\t\t\t\texists = SHA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tprestige = 5\n";
		allow += "\t\t\tSHA = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\towner = { in_sphere = THIS }\n";
		if (countries.find("BUR") != countries.end())
			allow += "\t\t\t\t\t\towned_by = BUR\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("form_libya"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_libya decision";
	else if (countries.find("LBY") == countries.end() ||
			 (countries.find("TRI") == countries.end() && countries.find("CYR") == countries.end()))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		if (countries.find("TRI") != countries.end())
			potential += "\t\t\t\ttag = TRI\n";
		if (countries.find("CYR") != countries.end())
			potential += "\t\t\t\ttag = CYR\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { exists = LBY }\n";
		potential += "\t\t\tcivilized = yes\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_kenya"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_kenya decision";
	else if (countries.find("KNY") == countries.end() || countries.find("SWA") == countries.end())
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("moldavia_moldova"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load moldavia_moldova decision";
	else if (countries.find("MDV") == countries.end() || countries.find("MOL") == countries.end())
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		if (countries.find("ROM") != countries.end())
			potential += "\t\t\texists = ROM\n";
		potential += "\t\t\ttag = MOL\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\texists = MOL\n";
		potential += "\t\t\t\tNOT = { exists = MOL }\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { has_global_flag = moldova }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		if (countries.find("ROM") != countries.end())
			allow += "\t\t\texists = ROM\n";
		else
			allow += "\t\t\talways = yes\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tany_core = {\n";
		if (countries.find("ROM") != countries.end())
		{
			effect += "\t\t\t\tlimit = {\n";
			effect += "\t\t\t\t\tNOT = {\n";
			effect += "\t\t\t\t\t\towned_by = ROM\n";
			effect += "\t\t\t\t\t}\n";
			effect += "\t\t\t\t}\n";
		}
		effect += "\t\t\t\tadd_core = MDV\n";
		effect += "\t\t\t\tremove_core = MOL\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tchange_tag = MDV\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("zapadoslavia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load zapadoslavia decision";
	else if (countries.find("WSF") == countries.end() ||
			 (countries.find("PLC") == countries.end() && countries.find("POL") == countries.end() &&
			  countries.find("CZH") == countries.end()))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tis_greater_power = yes\n";
		potential += "\t\t\tOR = {\n";
		if (countries.find("PLC") != countries.end())
			potential += "\t\t\t\ttag = PLC\n";
		if (countries.find("POL") != countries.end())
			potential += "\t\t\t\ttag = POL\n";
		if (countries.find("CZH") != countries.end())
			potential += "\t\t\t\ttag = CZH\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = WSF\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tnationalism_n_imperialism = 1\n";
		if (countries.find("PLC") != countries.end())
		{
			allow += "\t\t\tPLC = {\n";
			allow += "\t\t\t\tall_core = {\n";
			allow += "\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\t\towned_by = THIS\n";
			allow += "\t\t\t\t\t\t\towner = {\n";
			allow += "\t\t\t\t\t\t\t\tin_sphere = THIS\n";
			allow += "\t\t\t\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\t\t\t\tis_vassal = no\n";
			allow += "\t\t\t\t\t\t\t\t\tvassal_of = THIS\n";
			allow += "\t\t\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t\t\t\twar = no\n";
			allow += "\t\t\t\t\t\t\t\tis_greater_power = no\n";
			allow += "\t\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t}\n";
			allow += "\t\t\t\t}\n";
			allow += "\t\t\t}\n";
		}
		if (countries.find("CZH") != countries.end())
		{
			allow += "\t\t\tCZH = {\n";
			allow += "\t\t\t\tall_core = {\n";
			allow += "\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\t\towned_by = THIS\n";
			allow += "\t\t\t\t\t\t\towner = {\n";
			allow += "\t\t\t\t\t\t\t\tin_sphere = THIS\n";
			allow += "\t\t\t\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\t\t\t\tis_vassal = no\n";
			allow += "\t\t\t\t\t\t\t\t\tvassal_of = THIS\n";
			allow += "\t\t\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t\t\t\twar = no\n";
			allow += "\t\t\t\t\t\t\t\tis_greater_power = no\n";
			allow += "\t\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t}\n";
			allow += "\t\t\t\t}\n";
			allow += "\t\t\t}\n";
		}
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("oy_vey"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load oy_vey decision";
	else if (countries.find("ISR") == countries.end())
			decisions.erase(theDecision);
}