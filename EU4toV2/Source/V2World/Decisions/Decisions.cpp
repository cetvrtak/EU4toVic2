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

void V2::Decisions::update128Decisions(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	if (const auto& theDecision = decisions.find("the_concession_to_the_travelers"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load the_concession_to_the_travelers decision";
	else if (!x(countries, "GYP"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("create_rumelia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load create_rumelia decision";
	else if (!x(countries, "RML") || !x(countries, "BUL"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		if (x(countries, "TUR"))
		{
			potential += "\t\t\tOR = {\n";
			potential += "\t\t\t\ttag = TUR\n";
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\tNOT = { exists = TUR }\n";
			potential += "\t\t\t\t\tis_greater_power = yes\n";
			potential += "\t\t\t\t}\n";
			potential += "\t\t\t}\n";
		}
		else
			potential += "\t\t\tis_greater_power = yes\n";
		potential += "\t\t\tNOT = { is_culture_group = south_slavic }\n";
		potential += "\t\t\tNOT = { is_culture_group = greek }\n";
		if (x(countries, "ALI"))
			potential += "\t\t\tNOT = { tag = ALI } #Reasoning\n";
		if (x(countries, "RME"))
			potential += "\t\t\tNOT = { tag = RME }\n";
		if (x(countries, "BYZ"))
			potential += "\t\t\tNOT = { tag = BYZ }\n";
		potential += "\t\t\texists = BUL\n";
		if (x(countries, "YUG"))
			potential += "\t\t\tNOT = { exists = YUG }\n";
		potential += "\t\t\tNOT = { exists = RML }\n";
		potential += "\t\t\tNOT = { has_global_flag = rumelia }\n";
		potential += "\t\t\tTUR_809 = { owned_by = THIS }\n";
		potential += "\t\t\twar = no\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tNOT = { accepted_culture = bulgarian }\n";
		potential += "\t\t\t\thas_recently_lost_war = yes\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("independent_rumelia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load independent_rumelia decision";
	else if (!x(countries, "RML"))
			decisions.erase(theDecision);
	else
	{
		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		if (x(countries, "BUL"))
			allow += "\t\t\tBUL = { war = no }\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tclr_global_flag = rumelia\n";
		if (x(countries, "BUL"))
			effect += "\t\t\tany_owned = { limit = { is_core = RML } remove_core = RML add_core = BUL }\n";
		effect += "\t\t\tBUL = { inherit = THIS }\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}
}

void V2::Decisions::updateConveterUnions(const std::map<std::string, std::shared_ptr<Country>>& countries)
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
		if (x(countries, "FSA"))
			potential += "\t\t\t\t\t\t#tag = FSA\n";
		if (x(countries, "CSA"))
			potential += "\t\t\t\t\t\ttag = CSA\n";
		if (x(countries, "MGL"))
			potential += "\t\t\t\t\t\ttag = MGL\n";
		if (x(countries, "SPQ"))
			potential += "\t\t\t\t\t\ttag = SPQ\n";
		if (x(countries, "SLA"))
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
		if (x(countries, "SCY"))
			potential += "\t\t\t\t\tNOT = { exists = SCY }\n";
		if (x(countries, "NAP"))
			potential += "\t\t\t\t\tNOT = { exists = NAP }\n";
		potential += "\t\t\t\t}\n";
		if (x(countries, "NAP"))
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = NAP\n";
			potential += "\t\t\t\t\tNOT = { exists = SCY }\n";
			potential += "\t\t\t\t}\n";
		}
		if (x(countries, "SCY"))
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = SCY\n";
			potential += "\t\t\t\t\tNOT = { exists = NAP }\n";
			potential += "\t\t\t\t}\n";
		}
		potential += "\t\t\t}\n";
		if (x(countries, "SIC"))
			potential += "\t\t\tNOT = { exists = SIC }\n";
		if (x(countries, "ITA"))
			potential += "\t\t\tNOT = { exists = ITA }\n";
		if (x(countries, "SPQ"))
			potential += "\t\t\tNOT = { tag = SPQ }\n";
		if (x(countries, "BYZ"))
			potential += "\t\t\tNOT = { tag = BYZ }\n";
		if (x(countries, "PAP"))
			potential += "\t\t\tNOT = { tag = PAP }\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("mongol_khagan"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load mongol_khagan decision";
	else if (!x(countries, "KHA"))
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
			if (!x(countries, tag.first)
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
			if (x(countries, tag))
			{
				anythingToRemove = true;
				break;
			}
		}
		if (anythingToRemove)
		{
			for (const auto& tag: heimaten)
			{
				if (!x(countries, tag))
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
				if (x(countries, "GEO"))
					effect += "\t\t\t\t\tremove_core = GEO\n";
				if (x(countries, "ARM"))
					effect += "\t\t\t\t\tremove_core = ARM\n";
				if (x(countries, "AZB"))
					effect += "\t\t\t\t\tremove_core = AZB\n";
				effect += "\t\t\t\t}\n";
				effect += "\t\t\t}\n";
			}
		}

		if (x(countries, "CHI"))
		{
			effect += "\t\t\tCHI = {\n";
			effect += "\t\t\t\tany_owned_province = {\n";
			effect += "\t\t\t\t\tlimit = {\n";
			effect += "\t\t\t\t\t\tNOT = {\n";
			if (x(countries, "MGL"))
				effect += "\t\t\t\t\t\t\tis_core = MGL\n";
			effect += "\t\t\t\t\t\t\tregion = CHI_2608 #Mongolia proper\n";
			effect += "\t\t\t\t\t\t}\n";
			effect += "\t\t\t\t\t}\n";
			effect += "\t\t\t\t}\n";
			if (x(countries, "KHA"))
				effect += "\t\t\t\tremove_core = KHA\n";
			if (x(countries, "MGL"))
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
	else if (!x(countries, "CHI"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_netherlands"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_netherlands decision";
	else if (!x(countries, "NET"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = dutch\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = NET\n";
		if (x(countries, "FRA"))
			potential += "\t\t\t\ttag = FRA\n";
		if (x(countries, "SPQ"))
			potential += "\t\t\t\ttag = SPQ\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_belgium"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_belgium decision";
	else if (!x(countries, "BEL") || (!x(countries, "FLA") && !x(countries, "WLL")))
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
		if (x(countries, "LUX"))
			potential += "\t\t\t\ttag = LUX\n";
		if (x(countries, "SPQ"))
			potential += "\t\t\t\ttag = SPQ\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "FLA"))
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = FLA\n";
			if (x(countries, "WLL"))
				potential += "\t\t\t\t\tWLL = { exists = no }\n";
			potential += "\t\t\t\t}\n";
		}
		if (x(countries, "WLL"))
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = WLL\n";
			if (x(countries, "FLA"))
				potential += "\t\t\t\t\tFLA = { exists = no }\n";
			potential += "\t\t\t\t}\n";
		}
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_spain"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_spain decision";
	else if (!x(countries, "SPA"))
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
		if (x(countries, "SPC"))
			potential += "\t\t\t\texists = SPC\n";
		if (x(countries, "CAT"))
			potential += "\t\t\t\ttag = CAT\n";
		if (x(countries, "SPQ"))
			potential += "\t\t\t\ttag = SPQ\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tprestige = 5\n";
		allow += "\t\t\tOR = {\n";
		if (x(countries, "SPC"))
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
		if (x(countries, "SPC"))
			effect += "\t\t\t\tremove_core = SPC #Carlist Spain\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = SPA\n";
		effect += "\t\t}\t\t\t\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_ukraine"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_ukraine decision";
	else if (!x(countries, "UKR"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = ukrainian\n";
		potential += "\t\t\tNOT = {\n";
		if (x(countries, "RUT"))
			potential += "\t\t\t\texists = RUT\n";
		potential += "\t\t\t\texists = UKR\n";
		if (x(countries, "RUS"))
			potential += "\t\t\t\ttag = RUS\n";
		if (x(countries, "SLA"))
			potential += "\t\t\t\ttag = SLA\n";
		if (x(countries, "CRI"))
			potential += "\t\t\t\ttag = CRI\n";
		if (x(countries, "SPQ"))
			potential += "\t\t\t\ttag = SPQ\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_philippines"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_philippines decision";
	else if (!x(countries, "PHI"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = PHI\n";
		if (x(countries, "SPA"))
			potential += "\t\t\t\ttag = SPA\n";
		if (x(countries, "USA"))
			potential += "\t\t\t\ttag = USA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tprimary_culture = filipino\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_malaya"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_malaya decision";
	else if (!x(countries, "MLY"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_aztec"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_aztec decision";
	else if (!x(countries, "AZT"))
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
		if (x(countries, "MEX"))
			potential += "\t\t\t\texists = MEX\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_usa"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_usa decision";
	else if (!x(countries, "USA"))
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
		if (x(countries, "CSA"))
			potential += "\t\t\t\texists = CSA\n";
		if (x(countries, "TEX"))
			potential += "\t\t\t\ttag = TEX\n";
		if (x(countries, "CAL"))
			potential += "\t\t\t\ttag = CAL\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_southafrica"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_southafrica decision";
	else if (!x(countries, "SAF"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_madagascar"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_madagascar decision";
	else if (!x(countries, "MAD"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("mandate_of_heaven"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load mandate_of_heaven decision";
	else if (!x(countries, "CHI"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("become_hyderabad"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load become_hyderabad decision";
	else if (!x(countries, "HYD") || !x(countries, "DEC"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("become_slovenia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load become_slovenia decision";
	else if (!x(countries, "SLO") || (!x(countries, "AQU") && !x(countries, "CRT")))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "AQU"))
			potential += "\t\t\t\ttag = AQU\n";
		if (x(countries, "CRT"))
			potential += "\t\t\t\ttag = CRT\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tis_culture_group = south_slavic\n";
		potential += "\t\t\tNOT = { exists = SLO }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("latin_empire_upgrade"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load latin_empire_upgrade decision";
	else if (!x(countries, "SPQ") || !x(countries, "LTN"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\ttag = LTN\n";
		if (x(countries, "BYZ"))
			potential += "\t\t\tNOT = { exists = BYZ }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("become_longobardia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load become_longobardia decision";
	else if (!x(countries, "LGB"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = lombard\n";
		potential += "\t\t\tNOT = { exists = LGB }\n";
		if (x(countries, "LOM"))
			potential += "\t\t\tNOT = { exists = LOM }\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_japan"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_japan decision";
	else if (!x(countries, "JAP"))
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
		if (x(countries, "RYU"))
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
	else if (!x(countries, "UND") || !x(countries, "ZOM"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_ireland"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_ireland decision";
	else if (!x(countries, "IRE"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = irish\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = IRE\n";
		if (x(countries, "ENG"))
			potential += "\t\t\t\ttag = ENG\n";
		if (x(countries, "ENL"))
			potential += "\t\t\t\ttag = ENL\n";
		if (x(countries, "SCO"))
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
		if (x(countries, "ENG"))
			allow += "\t\t\t\t\t\towned_by = ENG\n";
		if (x(countries, "ENL"))
			allow += "\t\t\t\t\t\towned_by = ENL\n";
		if (x(countries, "SCO"))
			allow += "\t\t\t\t\t\towned_by = SCO\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("form_greece"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_greece decision";
	else if (!x(countries, "GRE"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = greek\n";
		potential += "\t\t\tNOT = {\n";
		if (x(countries, "BYZ"))
			potential += "\t\t\t\texists = BYZ\n";
		if (x(countries, "SPQ"))
			potential += "\t\t\t\texists = SPQ\n";
		if (x(countries, "GRE"))
			potential += "\t\t\t\texists = GRE\n";
		if (x(countries, "TUR"))
			potential += "\t\t\t\ttag = TUR\n";
		if (x(countries, "ITA"))
			potential += "\t\t\t\ttag = ITA\n";
		if (x(countries, "MCD"))
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
		if (x(countries, "TUR"))
			allow += "\t\t\t\t\t\towned_by = TUR\n";
		if (x(countries, "MCD"))
			allow += "\t\t\t\t\t\towned_by = MCD\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("form_turkestan"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_turkestan decision";
	else if (!x(countries, "TKS"))
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
		if (x(countries, "KAZ"))
			potential += "\t\t\t\ttag = KAZ\n";
		if (x(countries, "XIN"))
			potential += "\t\t\t\ttag = XIN\n";
		if (x(countries, "PER"))
			potential += "\t\t\t\ttag = PER\n";
		if (x(countries, "RUS"))
			potential += "\t\t\t\ttag = RUS\n";
		if (x(countries, "SLA"))
			potential += "\t\t\t\ttag = SLA\n";
		if (x(countries, "CHI"))
			potential += "\t\t\t\ttag = CHI\n";
		if (x(countries, "YUA"))
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
		if (x(countries, "KAZ"))
			allow += "\t\t\t\t\t\towned_by = KAZ\n";
		if (x(countries, "XIN"))
			allow += "\t\t\t\t\t\towned_by = XIN\n";
		if (x(countries, "PER"))
			allow += "\t\t\t\t\t\towned_by = PER\n";
		if (x(countries, "RUS"))
			allow += "\t\t\t\t\t\towned_by = RUS\n";
		if (x(countries, "SLA"))
			allow += "\t\t\t\t\t\towned_by = SLA\n";
		if (x(countries, "CHI"))
			allow += "\t\t\t\t\t\towned_by = CHI\n";
		if (x(countries, "YUA"))
			allow += "\t\t\t\t\t\towned_by = YUA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("form_occitania"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_occitania decision";
	else if (!x(countries, "OCC"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = occitan\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = OCC\n";
		if (x(countries, "FRA"))
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
		if (x(countries, "FRA"))
			allow += "\t\t\t\t\t\towned_by = FRA\n";
		if (x(countries, "SPA"))
			allow += "\t\t\t\t\t\towned_by = SPA\n";
		if (x(countries, "CAT"))
			allow += "\t\t\t\t\t\towned_by = CAT\n";
		if (x(countries, "NAV"))
			allow += "\t\t\t\t\t\towned_by = NAV\n";
		if (x(countries, "PAP"))
			allow += "\t\t\t\t\t\towned_by = PAP\n";
		if (x(countries, "ITA"))
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
		if (x(countries, "FRA"))
			effect += "\t\t\t\tremove_core = FRA\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = OCC\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_the_suebi"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_the_suebi decision";
	else if (!x(countries, "SUE"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = suebi\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = SUE\n";
		if (x(countries, "GLC"))
			potential += "\t\t\t\ttag = GLC\n";
		if (x(countries, "POR"))
			potential += "\t\t\t\ttag = POR\n";
		if (x(countries, "SPA"))
			potential += "\t\t\t\ttag = SPA\n";
		if (x(countries, "SPC"))
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
		if (x(countries, "SPA"))
			effect += "\t\t\t\tremove_core = SPA\n";
		if (x(countries, "POR"))
			effect += "\t\t\t\tremove_core = POR\n";
		if (x(countries, "GLC"))
			effect += "\t\t\t\tremove_core = GLC\n";
		if (x(countries, "SPC"))
			effect += "\t\t\t\tremove_core = SPC\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = SUE\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_visigothia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_visigothia decision";
	else if (!x(countries, "VGO"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = visigothic\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = VGO\n";
		if (x(countries, "POR"))
			potential += "\t\t\t\ttag = POR\n";
		if (x(countries, "SPA"))
			potential += "\t\t\t\ttag = SPA\n";
		if (x(countries, "SPC"))
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
		if (x(countries, "SPA"))
			effect += "\t\t\t\tremove_core = SPA\n";
		if (x(countries, "POR"))
			effect += "\t\t\t\tremove_core = POR\n";
		if (x(countries, "SPC"))
			effect += "\t\t\t\tremove_core = SPC\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = VGO\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_siberia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_siberia decision";
	else if (!x(countries, "SIB"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = siberian\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = SIB\n";
		if (x(countries, "RUS"))
			potential += "\t\t\t\ttag = RUS\n";
		if (x(countries, "SLA"))
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
		if (x(countries, "RUS"))
			effect += "\t\t\t\tremove_core = RUS\n";
		if (x(countries, "SLA"))
			effect += "\t\t\t\tremove_core = SLA\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = SIB\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_indochina"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_indochina decision";
	else if (!x(countries, "IDO"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_anglo_saxony"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_anglo_saxony decision";
	else if (!x(countries, "AES"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = anglo_saxon\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = AES\n";
		if (x(countries, "ENG"))
			potential += "\t\t\t\ttag = ENG\n";
		if (x(countries, "ENL"))
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
		if (x(countries, "ENG"))
			effect += "\t\t\t\tremove_core = ENG\n";
		if (x(countries, "ENL"))
			effect += "\t\t\t\tremove_core = ENL\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = AES\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_frankia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_frankia decision";
	else if (!x(countries, "FRK"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = frankish\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = FRK\n";
		if (x(countries, "FRA"))
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
		if (x(countries, "FRA"))
			effect += "\t\t\t\tremove_core = FRA\n";
		effect += "\t\t\t}\t\t\t\n";
		effect += "\t\t\t\n";
		effect += "\t\t\tchange_tag = FRK\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_britannia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_britannia decision";
	else if (!x(countries, "ENR"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_gallia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_gallia decision";
	else if (!x(countries, "FRR"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_laessinia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_laessinia decision";
	else if (!x(countries, "AUR"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_euskadi"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_euskadi decision";
	else if (!x(countries, "BSQ") || (!x(countries, "NAV") && !x(countries, "GYN")))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "NAV"))
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = NAV\n";
			potential += "\t\t\t\t\tgovernment = democracy\n";
			potential += "\t\t\t\t}\n";
		}
		if (x(countries, "GYN"))
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
		if (x(countries, "GYN"))
			allow += "\t\t\t\t\t\towned_by = GYN\n";
		if (x(countries, "NAV"))
			allow += "\t\t\t\t\t\towned_by = NAV\n";
		if (x(countries, "SPA"))
			allow += "\t\t\t\t\t\towned_by = SPA\n";
		if (x(countries, "FRA"))
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
		if (x(countries, "SPA"))
			effect += "\t\t\t\tremove_core = SPA\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tchange_tag = BSQ\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("form_nepal"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_nepal decision";
	else if (!x(countries, "NEP"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_persia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_persia decision";
	else if (!x(countries, "PER"))
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
		if (x(countries, "ARA"))
			potential += "\t\t\t\ttag = ARA\n";
		if (x(countries, "MUG"))
			potential += "\t\t\t\ttag = MUG\n";
		if (x(countries, "TIM"))
			potential += "\t\t\t\ttag = TIM\n";
		if (x(countries, "ALL"))
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
		if (x(countries, "TAJ"))
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
	else if (!x(countries, "KDS"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = kurdish\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = KDS\n";
		if (x(countries, "PER"))
			potential += "\t\t\t\ttag = PER\n";
		if (x(countries, "ARA"))
			potential += "\t\t\t\ttag = ARA\n";
		if (x(countries, "TUR"))
			potential += "\t\t\t\ttag = TUR\n";
		if (x(countries, "IRQ"))
			potential += "\t\t\t\ttag = IRQ\n";
		if (x(countries, "SYR"))
			potential += "\t\t\t\ttag = SYR\n";
		if (x(countries, "ALL"))
			potential += "\t\t\t\ttag = ALL\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("provincia_britannia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load provincia_britannia decision";
	else if (!x(countries, "ENR") && !x(countries, "SPQ"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("provincia_gallia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load provincia_gallia decision";
	else if (!x(countries, "FRR") && !x(countries, "SPQ"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("provincia_noricum"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load provincia_noricum decision";
	else if (!x(countries, "AUR") && !x(countries, "SPQ"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_LVN"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_LVN decision";
	else if (!x(countries, "LVN"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = LVN\n";
		if (x(countries, "ITA"))
			potential += "\t\t\t\ttag = ITA\n";
		if (x(countries, "ITP"))
			potential += "\t\t\t\ttag = ITP\n";
		if (x(countries, "SPQ"))
			potential += "\t\t\t\ttag = SPQ\n";
		if (x(countries, "BYZ"))
			potential += "\t\t\t\ttag = BYZ\n";
		if (x(countries, "LGB"))
			potential += "\t\t\t\ttag = LGB\n";
		if (x(countries, "PAP"))
			potential += "\t\t\t\ttag = PAP\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "LOM"))
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\tNOT = { exists = VEN }\n";
			potential += "\t\t\t\t\ttag = LOM\n";
			potential += "\t\t\t\t}\n";
		}
		if (x(countries, "VEN"))
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\tNOT = { exists = LOM }\n";
			potential += "\t\t\t\t\ttag = VEN\n";
			potential += "\t\t\t\t}\n";
		}
		potential += "\t\t\t\tAND = {\n";
		if (x(countries, "VEN"))
			potential += "\t\t\t\t\tNOT = { exists = VEN }\n";
		if (x(countries, "LOM"))
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
		if (x(countries, "LOM"))
		{
			allow += "\t\t\tLOM = {\n";
			allow += "\t\t\t\tall_core = { owned_by = THIS }\n";
			allow += "\t\t\t}\n";
		}
		if (x(countries, "VEN"))
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
		if (x(countries, "LOM"))
		{
			effect += "\t\t\tLOM = {\n";
			effect += "\t\t\t\tall_core = { add_core = LVN }\n";
			effect += "\t\t\t}\n";
		}
		if (x(countries, "VEN"))
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
	else if (!x(countries, "LVN"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = LVN\n";
		if (x(countries, "ITA"))
			potential += "\t\t\t\ttag = ITA\n";
		if (x(countries, "ITP"))
			potential += "\t\t\t\ttag = ITP\n";
		if (x(countries, "SPQ"))
			potential += "\t\t\t\ttag = SPQ\n";
		if (x(countries, "BYZ"))
			potential += "\t\t\t\ttag = BYZ\n";
		if (x(countries, "LGB"))
			potential += "\t\t\t\ttag = LGB\n";
		if (x(countries, "PAP"))
			potential += "\t\t\t\ttag = PAP\n";
		if (x(countries, "ALI"))
			potential += "\t\t\t\ttag = ALI #Yeah...\n";
		if (x(countries, "VEN"))
			potential += "\t\t\t\texists = VEN\n";
		if (x(countries, "LOM"))
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
		if (x(countries, "LOM"))
		{
			potential += "\t\t\t\tLOM = {\n";
			potential += "\t\t\t\t\tany_core = { owned_by = THIS }\n";
			potential += "\t\t\t\t}\n";
		}
		if (x(countries, "VEN"))
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
		if (x(countries, "LOM"))
		{
			allow += "\t\t\tLOM = {\n";
			allow += "\t\t\t\tall_core = { owned_by = THIS }\n";
			allow += "\t\t\t}\n";
		}
		if (x(countries, "VEN"))
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
		if (x(countries, "LOM"))
		{
			effect += "\t\t\tLOM = {\n";
			effect += "\t\t\t\tall_core = { add_core = LVN secede_province = LVN }\n";
			effect += "\t\t\t}\n";
		}
		if (x(countries, "VEN"))
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
	else if (!x(countries, "CEG") || !x(countries, "EGY"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("EGY_not_copt"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load EGY_not_copt decision";
	else if (!x(countries, "CEG") || !x(countries, "EGY"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_ITP"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_ITP decision";
	else if (!x(countries, "ITP") || !x(countries, "PAP"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\ttag = PAP\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\thas_country_flag = post_colonial_country\n";
		if (x(countries, "ITA"))
			potential += "\t\t\t\texists = ITA\n";
		potential += "\t\t\t}\n";
		if (x(countries, "SPQ"))
			potential += "\t\t\tNOT = { tag = SPQ }\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\tprestige = 45\n";
		allow += "\t\t\tnationalism_n_imperialism = 1\n";
		allow += "\t\t\tis_greater_power = yes\n";
		allow += "\t\t\twar = no\n";
		if (x(countries, "ITA"))
		{
			allow += "\t\t\tITA = {\n";
			allow += "\t\t\t\tall_core = {\n";
			allow += "\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\towned_by = THIS\n";
			if (x(countries, "VEN"))
			{
				allow += "\t\t\t\t\t\tAND = {\n";
				allow += "\t\t\t\t\t\t\tis_core = VEN\n";
				allow += "\t\t\t\t\t\t\towner = {\n";
				allow += "\t\t\t\t\t\t\t\tNOT = { is_culture_group = italian }\n";
				allow += "\t\t\t\t\t\t\t}\n";
				allow += "\t\t\t\t\t\t}\n";
			}
			if (x(countries, "LOM"))
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
		if (x(countries, "ITA"))
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
		if (x(countries, "FRA"))
			effect += "\t\t\t\t\t\ttag = FRA\n";
		if (x(countries, "AUS"))
			effect += "\t\t\t\t\t\ttag = AUS\n";
		if (x(countries, "KUK"))
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
	else if (!x(countries, "SPQ"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "BYZ"))
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = BYZ\n";
			if (x(countries, "LTN"))
				potential += "\t\t\t\t\tNOT = { exists = LTN }\n";
			if (x(countries, "HRE"))
				potential += "\t\t\t\t\tNOT = { exists = HRE }\n";
			potential += "\t\t\t\t}\n";
		}
		if (x(countries, "LTN"))
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = LTN\n";
			if (x(countries, "BYZ"))
				potential += "\t\t\t\t\tNOT = { exists = BYZ }\n";
			if (x(countries, "HRE"))
				potential += "\t\t\t\t\tNOT = { exists = HRE }\n";
			potential += "\t\t\t\t}\n";
		}
		if (x(countries, "HRE"))
		{
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = HRE\n";
			if (x(countries, "LTN"))
				potential += "\t\t\t\t\tNOT = { exists = LTN }\n";
			if (x(countries, "BYZ"))
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
		if (x(countries, "BYZ"))
			effect += "\t\t\t\tremove_core = BYZ\n";
		if (x(countries, "LTN"))
			effect += "\t\t\t\tremove_core = LTN\n";
		if (x(countries, "HRE"))
			effect += "\t\t\t\tremove_core = HRE\n";
		if (x(countries, "ITA"))
			effect += "\t\t\t\tremove_core = ITA\n";
		if (x(countries, "ITP"))
			effect += "\t\t\t\tremove_core = ITP\n";
		if (x(countries, "JER"))
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
	else if (!x(countries, "YEM"))
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
		if (x(countries, "ARA"))
			potential += "\t\t\t\ttag = ARA\n";
		if (x(countries, "ALL"))
			potential += "\t\t\t\ttag = ALL\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_burma"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_burma decision";
	else if (!x(countries, "BUR"))
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
		if (x(countries, "SHA"))
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
		if (x(countries, "SHA"))
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
		if (x(countries, "SHA"))
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
	else if (!x(countries, "SHA"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tprimary_culture = shan\n";
		potential += "\t\t\tNOT = {\n";
		if (x(countries, "BUR"))
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
		if (x(countries, "BUR"))
			allow += "\t\t\t\t\t\towned_by = BUR\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("form_libya"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_libya decision";
	else if (!x(countries, "LBY") || (!x(countries, "TRI") && !x(countries, "CYR")))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "TRI"))
			potential += "\t\t\t\ttag = TRI\n";
		if (x(countries, "CYR"))
			potential += "\t\t\t\ttag = CYR\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { exists = LBY }\n";
		potential += "\t\t\tcivilized = yes\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_kenya"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_kenya decision";
	else if (!x(countries, "KNY") || !x(countries, "SWA"))
			decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("moldavia_moldova"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load moldavia_moldova decision";
	else if (!x(countries, "MDV") || !x(countries, "MOL"))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		if (x(countries, "ROM"))
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
		if (x(countries, "ROM"))
			allow += "\t\t\texists = ROM\n";
		else
			allow += "\t\t\talways = yes\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tany_core = {\n";
		if (x(countries, "ROM"))
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
	else if (!x(countries, "WSF") || (!x(countries, "PLC") && !x(countries, "POL") && !x(countries, "CZH")))
			decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tis_greater_power = yes\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "PLC"))
			potential += "\t\t\t\ttag = PLC\n";
		if (x(countries, "POL"))
			potential += "\t\t\t\ttag = POL\n";
		if (x(countries, "CZH"))
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
		if (x(countries, "PLC"))
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
		if (x(countries, "CZH"))
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
	else if (!x(countries, "ISR"))
			decisions.erase(theDecision);
}

bool V2::Decisions::x(const std::map<std::string, std::shared_ptr<Country>>& countries, const std::string& tag)
{
	bool isTagEssential = false;
	if (countries.find(tag) != countries.end())
		isTagEssential = true;

	return isTagEssential;
}