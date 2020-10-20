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

	if (const auto& theDecision = decisions.find("taiping_and_csa"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load taiping_and_csa decision";
	else
	{
		std::istringstream effectString(theDecision->second.getEffect());
		Taiping taiping(effectString);

		std::vector<std::string> heimaten = {"PER", "TUR", "RUS", "SLA"};
		std::string effect;
		effect += "= {\n";
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
	else
	{
		if (countries.find("CHI") == countries.end())
			decisions.erase(theDecision);
	}

	if (const auto& theDecision = decisions.find("form_netherlands"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_netherlands decision";
	else
	{
		if (countries.find("NET") == countries.end())
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
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
	}

	if (const auto& theDecision = decisions.find("form_belgium"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_belgium decision";
	else
	{
		if (countries.find("BEL") == countries.end()
			 || (countries.find("FLA") == countries.end() && countries.find("WLL") == countries.end()))
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
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
	}

	if (const auto& theDecision = decisions.find("form_spain"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_spain decision";
	else
	{
		if (countries.find("SPA") == countries.end())
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
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

			std::string allow;
			allow += "= {\n";
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

			std::string effect;
			effect += "= {\n";
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
	}

	if (const auto& theDecision = decisions.find("form_ukraine"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_ukraine decision";
	else
	{
		if (countries.find("UKR") == countries.end())
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
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
	}

	if (const auto& theDecision = decisions.find("form_philippines"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_philippines decision";
	else
	{
		if (countries.find("PHI") == countries.end())
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
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
	}

	if (const auto& theDecision = decisions.find("form_malaya"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_malaya decision";
	else
	{
		if (countries.find("MLY") == countries.end())
			decisions.erase(theDecision);
	}

	if (const auto& theDecision = decisions.find("form_aztec"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_aztec decision";
	else
	{
		if (countries.find("AZT") == countries.end())
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
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
	}

	if (const auto& theDecision = decisions.find("form_usa"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_usa decision";
	else
	{
		if (countries.find("USA") == countries.end())
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
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
	}

	if (const auto& theDecision = decisions.find("form_southafrica"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_southafrica decision";
	else
	{
		if (countries.find("SAF") == countries.end())
			decisions.erase(theDecision);
	}

	if (const auto& theDecision = decisions.find("form_madagascar"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_madagascar decision";
	else
	{
		if (countries.find("MAD") == countries.end())
			decisions.erase(theDecision);
	}

	if (const auto& theDecision = decisions.find("mandate_of_heaven"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load mandate_of_heaven decision";
	else
	{
		if (countries.find("CHI") == countries.end())
			decisions.erase(theDecision);
	}

	if (const auto& theDecision = decisions.find("become_hyderabad"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load become_hyderabad decision";
	else
	{
		if (countries.find("HYD") == countries.end() || countries.find("DEC") == countries.end())
			decisions.erase(theDecision);
	}

	if (const auto& theDecision = decisions.find("become_slovenia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load become_slovenia decision";
	else
	{
		if (countries.find("SLO") == countries.end()
			 || (countries.find("AQU") == countries.end() && countries.find("CRT") == countries.end()))
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
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
	}

	if (const auto& theDecision = decisions.find("latin_empire_upgrade"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load latin_empire_upgrade decision";
	else
	{
		if (countries.find("SPQ") == countries.end() || countries.find("LTN") == countries.end())
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
			potential += "\t\t\ttag = LTN\n";
			if (countries.find("BYZ") != countries.end())
				potential += "\t\t\tNOT = { exists = BYZ }\n";
			potential += "\t\t}\t\n";
			(theDecision->second).updateDecision("potential", potential);
		}
	}

	if (const auto& theDecision = decisions.find("become_longobardia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load become_longobardia decision";
	else
	{
		if (countries.find("LGB") == countries.end())
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
			potential += "\t\t\tprimary_culture = lombard\n";
			potential += "\t\t\tNOT = { exists = LGB }\n";
			if (countries.find("LOM") != countries.end())
				potential += "\t\t\tNOT = { exists = LOM }\n";
			potential += "\t\t}\n";
			(theDecision->second).updateDecision("potential", potential);
		}
	}

	if (const auto& theDecision = decisions.find("form_japan"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_japan decision";
	else
	{
		if (countries.find("JAP") == countries.end())
			decisions.erase(theDecision);
		else
		{
			std::string allow;
			allow += "= {\n";
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
	}

	if (const auto& theDecision = decisions.find("form_the_undeads"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_the_undeads decision";
	else
	{
		if (countries.find("UND") == countries.end() || countries.find("ZOM") == countries.end())
			decisions.erase(theDecision);
	}

	if (const auto& theDecision = decisions.find("form_ireland"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_ireland decision";
	else
	{
		if (countries.find("IRE") == countries.end())
			decisions.erase(theDecision);
		else
		{
			std::string potential;
			potential += "= {\n";
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

			std::string allow;
			allow += "= {\n";
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
	}
}