#include "Decisions.h"
#include "../Country/Country.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "Taiping.h"
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

void V2::Decisions::updateACW(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	if (const auto& theDecision = decisions.find("apply_for_USA_statehood"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load apply_for_USA_statehood decision";
	else if (!x(countries, "USA") && !(x(countries, "TEX") || x(countries, "DES") || x(countries, "CAL") || x(countries, "LSK") || x(countries, "LOU")))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\texists = USA\n";
		potential += "\t\t\tUSA = { government = democracy }\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "TEX"))
			potential += "\t\t\t\ttag = TEX\n";
		if (x(countries, "DES"))
			potential += "\t\t\t\ttag = DES\n";
		if (x(countries, "CAL"))
			potential += "\t\t\t\ttag = CAL\n";
		if (x(countries, "LSK"))
			potential += "\t\t\t\ttag = LSK\n";
		if (x(countries, "LOU"))
			potential += "\t\t\t\ttag = LOU\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tis_vassal = no\n";
		potential += "\t\t\t\tvassal_of = USA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tpart_of_sphere = no\n";
		potential += "\t\t\t\tin_sphere = USA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tai = yes\n";
		potential += "\t\t\tgovernment = democracy\n";
		potential += "\t\t\tNOT = { has_country_flag = usstatehood_we_have_applied }\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("king_cotton"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load king_cotton decision";
	else if (!x(countries, "CSA") || !x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("trail_of_tears"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load trail_of_tears decision";
	else if (!x(countries, "USA") && !x(countries, "CSA"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "USA"))
			potential += "\t\t\t\ttag = USA\n";
		if (x(countries, "CSA"))
			potential += "\t\t\t\ttag = CSA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\thas_global_flag = marching_on_the_trail_of_tears\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string effect = "= {\n";
		effect += "\t\t\tany_pop = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\thas_pop_culture = cherokee\n";
		effect += "\t\t\t\t\tNOT = {\n";
		effect += "\t\t\t\t\t\tlocation = {\n";
		effect += "\t\t\t\t\t\t\tOR = {\n";
		effect += "\t\t\t\t\t\t\t\tprovince_id = 130\n";
		effect += "\t\t\t\t\t\t\t\tprovince_id = 131\n";
		effect += "\t\t\t\t\t\t\t\tprovince_id = 129\n";
		effect += "\t\t\t\t\t\t\t}\n";
		effect += "\t\t\t\t\t\t}\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\treduce_pop = 0.80\n";
		effect += "\t\t\t\tconsciousness = 2\n";
		effect += "\t\t\t\tmilitancy = -1\n";
		effect += "\t\t\t\tmove_pop = 130\n";
		effect += "\t\t\t}\n";
		if (x(countries, "CHE"))
		{
			effect += "\t\t\t130 = {\t\t\n";
			effect += "\t\t\t\tadd_core = CHE\t\t\n";
			effect += "\t\t\t}\t\t\n";
			effect += "\t\t\t131 = {\t\t\n";
			effect += "\t\t\t\tadd_core = CHE\t\t\n";
			effect += "\t\t\t}\t\t\n";
			effect += "\t\t\t129 = {\t\t\n";
			effect += "\t\t\t\tadd_core = CHE\t\t\n";
			effect += "\t\t\t}\t\t\n";
			effect += "\t\t\t145 = {\t\t\n";
			effect += "\t\t\t\tremove_core = CHE\t\t\n";
			effect += "\t\t\t}\t\t\n";
			effect += "\t\t\t143 = {\t\t\n";
			effect += "\t\t\t\tremove_core = CHE\t\t\n";
			effect += "\t\t\t}\n";
		}
		effect += "\t\t\tadd_accepted_culture = cherokee\n";
		effect += "\t\t\tset_global_flag = marching_on_the_trail_of_tears\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("custers_expedition"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load custers_expedition decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("enact_the_homestead_act"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load enact_the_homestead_act decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("bixby_letter"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load bixby_letter decision";
	else if (!x(countries, "USA") || !x(countries, "CSA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("enact_anaconda_plan"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load enact_anaconda_plan decision";
	else if (!x(countries, "USA") && !x(countries, "CSA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("shermans_march_to_the_sea"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load shermans_march_to_the_sea decision";
	else if (!x(countries, "USA") && !x(countries, "CSA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("emancipation_proclamation"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load emancipation_proclamation decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\ttag = USA\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\thas_country_flag = emancipation\n";
		potential += "\t\t\t\thas_country_flag = the_slavery_debate\n";
		potential += "\t\t\t}\n";
		if (x(countries, "CSA"))
		{
			potential += "\t\t\tOR = {\n";
			potential += "\t\t\t\twar_with = CSA\n";
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\tNOT = { exists = CSA }\n";
			potential += "\t\t\t\t\thas_global_flag = american_civil_war_has_happened\n";
			potential += "\t\t\t\t}\n";
			potential += "\t\t\t}\n";
		}
		else
			potential += "\t\t\thas_global_flag = american_civil_war_has_happened\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		if (x(countries, "CSA"))
		{
			allow += "\t\t\tOR = {\n";
			allow += "\t\t\t\tCSA = { national_provinces_occupied = 0.40 }\n";
			allow += "\t\t\t\tCSA = { exists = no }\n";
			allow += "\t\t\t}\n";
		}
		allow += "\t\t\tprestige = 35\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("manifest_destiny"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load manifest_destiny decision";
	else if (!x(countries, "USA") || !x(countries, "MEX"))
		decisions.erase(theDecision);
	else
	{
		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tromanticism = 1\n";
		allow += "\t\t\tstate_n_government = 1\n";
		allow += "\t\t\tNOT = { truce_with = MEX }\n";
		allow += "\t\t\tOR = {\n";
		if (x(countries, "TEX"))
			allow += "\t\t\t\tTEX = { exists = no }\n";
		allow += "\t\t\t\tinvention = manifest_destiny\n";
		allow += "\t\t\t\tany_owned_province = { is_core = MEX }\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tset_country_flag = hasmanifestdestiny\n";
		effect += "\t\t\tMEX_85 = { add_core = USA } #California\n";
		effect += "\t\t\tTEX_132 = { add_core = USA } #Texas\n";
		effect += "\t\t\tMEX_103 = { add_core = USA } #New Mexico\n";
		effect += "\t\t\tMEX_94 = { add_core = USA } #Nevada\n";
		effect += "\t\t\t#MEX_97 = { add_core = USA } #Utah\n";
		effect += "\t\t\tMEX_100 = { add_core = USA } #Arizona\n";
		effect += "\t\t\tUSA_129 = { add_core = USA } #Oklahoma\n";
		effect += "\t\t\tUSA_106 = { add_core = USA } #Colorado\n";
		effect += "\t\t\tUSA_78 = { add_core = USA } #Washington\n";
		effect += "\t\t\tUSA_81 = { add_core = USA } #Portland\n";
		effect += "\t\t\tUSA_91 = { add_core = USA } #Idaho\n";
		effect += "\t\t\trelation = { who = MEX value = -200 }\n";
		effect += "\t\t\tdiplomatic_influence = { who = MEX value = -200 }\n";
		effect += "\t\t\tleave_alliance = MEX\n";
		effect += "\t\t\trandom_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\ttag = MEX\n";
		effect += "\t\t\t\t\tvassal_of = THIS\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tTHIS = { release_vassal = MEX }\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\trandom_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tis_our_vassal = MEX\n";
		effect += "\t\t\t\t\tNOT = { tag = THIS }\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\trelation = { who = THIS value = -200 }\n";
		effect += "\t\t\t\tleave_alliance = THIS\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\trandom_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\ttag = MEX\n";
		effect += "\t\t\t\t\tany_owned_province = {\n";
		effect += "\t\t\t\t\t\tOR = {\n";
		effect += "\t\t\t\t\t\t\tregion = MEX_85\n";
		effect += "\t\t\t\t\t\t\tregion = TEX_132\n";
		effect += "\t\t\t\t\t\t\tregion = MEX_103\n";
		effect += "\t\t\t\t\t\t\tregion = MEX_94\n";
		effect += "\t\t\t\t\t\t\tregion = MEX_97\n";
		effect += "\t\t\t\t\t\t\tregion = MEX_100\n";
		effect += "\t\t\t\t\t\t}\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tcountry_event = 44126\n";
		effect += "\t\t\t}\n";
		if (x(countries, "CAL") || x(countries, "TEX") || x(countries, "DES"))
		{
			effect += "\t\t\tany_country = {\n";
			effect += "\t\t\t\tlimit = {\n";
			effect += "\t\t\t\t\texists = yes\n";
			effect += "\t\t\t\t\tOR = {\n";
			if (x(countries, "CAL"))
				effect += "\t\t\t\t\t\ttag = CAL\n";
			if (x(countries, "TEX"))
				effect += "\t\t\t\t\t\ttag = TEX\n";
			if (x(countries, "DES"))
				effect += "\t\t\t\t\t\ttag = DES\n";
			effect += "\t\t\t\t\t}\n";
			effect += "\t\t\t\t\tOR = {\n";
			effect += "\t\t\t\t\t\tin_sphere = THIS\n";
			effect += "\t\t\t\t\t\tpart_of_sphere = no\n";
			effect += "\t\t\t\t\t}\n";
			effect += "\t\t\t\t\tOR = {\n";
			effect += "\t\t\t\t\t\tvassal_of = THIS\n";
			effect += "\t\t\t\t\t\tis_vassal = no\n";
			effect += "\t\t\t\t\t}\n";
			effect += "\t\t\t\t}\n";
			effect += "\t\t\t\tcountry_event = 98650\n";
			effect += "\t\t\t}\n";
		}
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tis_greater_power = yes\n";
		effect += "\t\t\t\t\tNOT = { tag = THIS }\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tdiplomatic_influence = { who = MEX value = -100 }\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tany_pop = {\n";
		effect += "\t\t\t\tdominant_issue = {\n";
		effect += "\t\t\t\t\tfactor = 0.05\n";
		effect += "\t\t\t\t\tvalue = jingoism\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tconsciousness = 2\n";
		effect += "\t\t\t}\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("gag_rule"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load gag_rule decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("clay_and_douglas_draft"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load clay_and_douglas_draft decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("corwin_amendment"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load corwin_amendment decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("fugitive_slave_act"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load fugitive_slave_act decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("nashville_convention"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load nashville_convention decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("ostend_manifesto"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load ostend_manifesto decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("kansas_nebraska_act"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load kansas_nebraska_act decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("wilmot_proviso"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load wilmot_proviso decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("form_kellys_irish_brigade"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_kellys_irish_brigade decision";
	else if (!x(countries, "CSA") || !x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("raise_the_bonnie_blue_flag"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load raise_the_bonnie_blue_flag decision";
	else if (!x(countries, "CSA") || !x(countries, "USA"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("enact_radical_reconstruction"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load enact_radical_reconstruction decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\ttag = USA\n";
		potential += "\t\t\twar = no\n";
		potential += "\t\t\thas_global_flag = american_civil_war_has_happened\n";
		potential += "\t\t\towns = 211\n";
		potential += "\t\t\towns = 195\n";
		potential += "\t\t\towns = 139\n";
		potential += "\t\t\tNOT = {\n";
		if (x(countries, "CSA"))
			potential += "\t\t\t\texists = CSA\n";
		potential += "\t\t\t\thas_global_flag = alt_american_civil_war_has_happened\n";
		potential += "\t\t\t\thas_country_flag = has_used_radical_reconstruction\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("annex_hawaii"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load annex_hawaii decision";
	else if (!x(countries, "HAW"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("alaskan_purchase"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load alaskan_purchase decision";
	else if (!x(countries, "USA") || !x(countries, "RUS"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("webster_ashburton_treaty"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load webster_ashburton_treaty decision";
	else if (!x(countries, "ENG") || !x(countries, "USA"))
		decisions.erase(theDecision);
	else
	{
		std::string effect = "= {\n";
		effect += "\t\t\trelation = {\t\t\n";
		effect += "\t\t\t\twho = USA\t\t\n";
		effect += "\t\t\t\tvalue = 100\t\t\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\t250 = {\n";
		effect += "\t\t\t\tremove_core = ENG\n";
		if (x(countries, "MRU"))
			effect += "\t\t\t\t#remove_core = MRU\n";
		if (x(countries, "CAN"))
			effect += "\t\t\t\tremove_core = CAN\n";
		effect += "\t\t\t\tsecede_province = USA\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tbadboy = -2\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tUSA = { country_event = 44122 }\n";
		effect += "\t\t\tset_country_flag = webster_ashburton_signed\n";
		effect += "\t\t\t250 = {\t\t\t\t#Caribou\t\t\n";
		if (x(countries, "CAN"))
			effect += "\t\t\t\tremove_core = CAN\t\t\n";
		effect += "\t\t\t}\t\t\n";
		if (x(countries, "CAN"))
			effect += "\t\t\t249 = {\t\t\t\t#Bangor\t\t\n";
		effect += "\t\t\t\tremove_core = CAN\t\t\n";
		effect += "\t\t\t}\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}
}

void V2::Decisions::updateArabianFormation(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	if (const auto& theDecision = decisions.find("form_arabia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_arabia decision";
	else if (!x(countries, "ARA"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tARA = { exists = no }\n";
		if (x(countries, "ALL"))
			potential += "\t\t\tNOT = { tag = ALL }\n";
		potential += "\t\t\tis_culture_group = arab\n";
		potential += "\t\t}\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\tOR = { \n";
		allow += "\t\t\t\tAND = { \n";
		allow += "\t\t\t\t\tis_greater_power = yes\n";
		allow += "\t\t\t\t\tany_neighbor_country = {\n";
		allow += "\t\t\t\t\t\tin_sphere = this\n";
		allow += "\t\t\t\t\t\tis_culture_group = arab\n";
		allow += "\t\t\t\t\t\trelation = { who = THIS value = 100 }\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		if (x(countries, "NEJ") || x(countries, "HDJ") || x(countries, "HAL"))
		{
			allow += "\t\t\t\tAND = {\n";
			allow += "\t\t\t\t\tOR = {\n";
			if (x(countries, "NEJ"))
				allow += "\t\t\t\t\t\ttag = NEJ\n";
			if (x(countries, "HDJ"))
				allow += "\t\t\t\t\t\ttag = HDJ\n";
			if (x(countries, "HAL"))
				allow += "\t\t\t\t\t\ttag = HAL\n";
			allow += "\t\t\t\t\t}\n";
			if (x(countries, "NEJ"))
			{
				allow += "\t\t\t\t\tNEJ = { #Nejd\n";
				allow += "\t\t\t\t\t\tall_core = { owned_by = THIS }\n";
				allow += "\t\t\t\t\t}\n";
			}
			if (x(countries, "HDJ"))
			{
				allow += "\t\t\t\t\tHDJ = { #Hejaz\n";
				allow += "\t\t\t\t\t\tall_core = { owned_by = THIS }\n";
				allow += "\t\t\t\t\t}\n";
			}
			if (x(countries, "HAL"))
			{
				allow += "\t\t\t\t\tHAL = { #Hail Shammar\n";
				allow += "\t\t\t\t\t\tall_core = { owned_by = THIS }\n";
				allow += "\t\t\t\t\t}\n";
			}
			allow += "\t\t\t\t}\n";
		}
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}
}

void V2::Decisions::updateConvAfricanPostColonial(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	if (const auto& theDecision = decisions.find("african_post_colonialism"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load african_post_colonialism decision";
	else
	{
		std::istringstream effectString(theDecision->second.getEffect());
		Taiping africa(effectString);

		std::string effect = "= {\n";
		for (const auto& nonCountrySpecificEffect: africa.getNonCountrySpecificEffects())
		{
			effect += "\t\t\t" + nonCountrySpecificEffect + "\n";
		}
		for (const auto& tag: africa.getTagEffectMap())
		{
			if (x(countries, tag.first))
			{
				effect += "\t\t\t" + tag.second + "\n";
			}
		}
		for (const auto& coreEffect: africa.getCountryCores())
		{
			effect += "\t\t\t" + coreEffect + "\n";
		}

		effect += "\t\t\tset_global_flag = der_postcolonialism_2\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}
}

void V2::Decisions::updateConverterPostCol(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	if (const auto& theDecision = decisions.find("the_post_colonialism"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load the_post_colonialism decision";
	else
	{
		std::istringstream effectString(theDecision->second.getEffect());
		Taiping newWorld(effectString);

		std::string effect = "= {\n";
		for (const auto& nonCountrySpecificEffect: newWorld.getNonCountrySpecificEffects())
		{
			effect += "\t\t\t" + nonCountrySpecificEffect + "\n";
		}
		for (const auto& tag: newWorld.getTagEffectMap())
		{
			if (x(countries, tag.first))
			{
				effect += "\t\t\t" + tag.second + "\n";
			}
		}
		for (const auto& coreEffect: newWorld.getCountryCores())
		{
			effect += "\t\t\t" + coreEffect + "\n";
		}

		effect += "\t\t\tset_global_flag = der_postcolonialism\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("postcol_LSK"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_LSK decision";
	else if (!x(countries, "LSK"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = LSK\n";
		if (x(countries, "CAN"))
			potential += "\t\t\t\ttag = CAN\n";
		if (x(countries, "COL"))
			potential += "\t\t\t\ttag = COL\n";
		if (x(countries, "USA"))
			potential += "\t\t\t\ttag = USA\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tcapital_scope = { is_core = LSK }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("postcol_QUE"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_QUE decision";
	else if (!x(countries, "QUE"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = french\n";
		potential += "\t\t\t\tprimary_culture = french_canadian\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = QUE\n";
		if (x(countries, "CAN"))
			potential += "\t\t\t\ttag = CAN\n";
		if (x(countries, "COL"))
			potential += "\t\t\t\ttag = COL\n";
		if (x(countries, "NEW"))
			potential += "\t\t\t\ttag = NEW\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tcapital_scope = { is_core = QUE }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tQUE = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "CAN"))
			allow += "\t\t\t\t\t\towned_by = CAN\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_COL"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_COL decision";
	else if (!x(countries, "COL"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = british\n";
		potential += "\t\t\t\tprimary_culture = english\n";
		potential += "\t\t\t\tprimary_culture = anglo_canadian\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = COL\n";
		if (x(countries, "CAN"))
			potential += "\t\t\t\ttag = CAN\n";
		if (x(countries, "NEW"))
			potential += "\t\t\t\ttag = NEW\n";
		if (x(countries, "QUE"))
			potential += "\t\t\t\ttag = QUE\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tcapital_scope = { is_core = COL }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tCOL = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "CAN"))
			allow += "\t\t\t\t\t\towned_by = CAN\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_NEW"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_NEW decision";
	else if (!x(countries, "NEW"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = british\n";
		potential += "\t\t\t\tprimary_culture = english\n";
		potential += "\t\t\t\tprimary_culture = anglo_canadian\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = NEW\n";
		if (x(countries, "CAN"))
			potential += "\t\t\t\ttag = CAN\n";
		if (x(countries, "COL"))
			potential += "\t\t\t\ttag = COL\n";
		if (x(countries, "QUE"))
			potential += "\t\t\t\ttag = QUE\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tcapital_scope = { is_core = NEW }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tNEW = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "CAN"))
			allow += "\t\t\t\t\t\towned_by = CAN\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_CAN"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_CAN decision";
	else if (!x(countries, "CAN"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("postcol_NEN"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_NEN decision";
	else if (!x(countries, "NEN"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = NEN\n";
		if (x(countries, "USA"))
			potential += "\t\t\t\ttag = USA\n";
		if (x(countries, "CAN"))
			potential += "\t\t\t\ttag = CAN\n";
		if (x(countries, "QUE"))
			potential += "\t\t\t\ttag = QUE\n";
		if (x(countries, "COL"))
			potential += "\t\t\t\ttag = COL\n";
		if (x(countries, "NEW"))
			potential += "\t\t\t\ttag = NEW\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = british\n";
		potential += "\t\t\t\tprimary_culture = english\n";
		potential += "\t\t\t\tprimary_culture = yankee\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tcapital_scope = { is_core = NEN }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tNEN = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "USA"))
			allow += "\t\t\t\t\t\towned_by = USA\n";
		if (x(countries, "CSA"))
			allow += "\t\t\t\t\t\towned_by = CSA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_MAN"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_MAN decision";
	else if (!x(countries, "MAN"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = MAN\n";
		if (x(countries, "USA"))
			potential += "\t\t\t\ttag = USA\n";
		if (x(countries, "NEN"))
			potential += "\t\t\t\ttag = NEN\n";
		if (x(countries, "CAN"))
			potential += "\t\t\t\ttag = CAN\n";
		if (x(countries, "QUE"))
			potential += "\t\t\t\ttag = QUE\n";
		if (x(countries, "COL"))
			potential += "\t\t\t\ttag = COL\n";
		if (x(countries, "NEW"))
			potential += "\t\t\t\ttag = NEW\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tcapital_scope = { is_core = MAN }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tMAN = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "NEN"))
			allow += "\t\t\t\t\t\towned_by = NEN\n";
		if (x(countries, "USA"))
			allow += "\t\t\t\t\t\towned_by = USA\n";
		if (x(countries, "CSA"))
			allow += "\t\t\t\t\t\towned_by = CSA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_LOU"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_LOU decision";
	else if (!x(countries, "LOU"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = LOU\n";
		if (x(countries, "USA"))
			potential += "\t\t\t\ttag = USA\n";
		if (x(countries, "CSA"))
			potential += "\t\t\t\ttag = CSA\n";
		if (x(countries, "NEN"))
			potential += "\t\t\t\ttag = NEN\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tcapital_scope = { is_core = LOU }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tLOU = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "USA"))
			allow += "\t\t\t\t\t\towned_by = USA\n";
		if (x(countries, "CSA"))
			allow += "\t\t\t\t\t\towned_by = CSA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = LOU\n";
		if (x(countries, "USA"))
			effect += "\t\t\t\tremove_core = USA\n";
		if (x(countries, "CSA"))
			effect += "\t\t\t\tremove_core = CSA\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tchange_tag = LOU\n";
		effect += "\t\t\tcapital = 139 #New Orleans\n";
		effect += "\t\t\tset_country_flag = postcol_1\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("postcol_TEX"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_TEX decision";
	else if (!x(countries, "TEX"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = TEX }\n";
		potential += "\t\t\tcapital_scope = { is_core = TEX }\n";
		potential += "\t\t\tNOT = {\n";
		if (x(countries, "USA"))
			potential += "\t\t\t\ttag = USA\n";
		if (x(countries, "MEX"))
			potential += "\t\t\t\ttag = MEX\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("postcol_USA"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_USA decision";
	else if (!x(countries, "USA"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = USA }\n";
		potential += "\t\t\tcapital_scope = { is_core = USA }\n";
		if (x(countries, "CAN"))
			potential += "\t\t\tNOT = { tag = CAN }\n";
		if (x(countries, "CSA"))
			potential += "\t\t\tNOT = { tag = CSA }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("postcol_CAL"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_CAL decision";
	else if (!x(countries, "CAL"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = CAL }\n";
		potential += "\t\t\tcapital_scope = { is_core = CAL }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = {\n";
		if (x(countries, "USA"))
			potential += "\t\t\t\ttag = USA\n";
		if (x(countries, "CAN"))
			potential += "\t\t\t\ttag = CAN\n";
		if (x(countries, "MEX"))
			potential += "\t\t\t\ttag = MEX\n";
		if (x(countries, "TEX"))
			potential += "\t\t\t\ttag = TEX\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("postcol_CUB"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_CUB decision";
	else if (!x(countries, "CUB"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("postcol_HAI"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_HAI decision";
	else if (!x(countries, "HAI"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = HAI }\n";
		if (x(countries, "DOM"))
			potential += "\t\t\tNOT = { tag = DOM }\n";
		potential += "\t\t\tcapital_scope = { is_core = HAI }\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tprimary_culture = french\n";
		potential += "\t\t\t\tprimary_culture = americaine\n";
		potential += "\t\t\t\tprimary_culture = afro_caribbean\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		if (x(countries, "DOM"))
			effect += "\t\t\t2213 = { remove_core = DOM } # To avoid Haiti disappearing\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = HAI\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tchange_tag = HAI\n";
		effect += "\t\t\tcapital = 2213\n";
		if (x(countries, "DOM"))
			effect += "\t\t\trelease_vassal = DOM\n";
		effect += "\t\t\tset_country_flag = postcol_1\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("postcol_DOM"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_DOM decision";
	else if (!x(countries, "DOM"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = DOM }\n";
		if (x(countries, "HAI"))
			potential += "\t\t\tNOT = { tag = HAI }\n";
		potential += "\t\t\tcapital_scope = { is_core = DOM }\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\tprimary_culture = french\n";
		potential += "\t\t\t\tprimary_culture = americaine\n";
		potential += "\t\t\t\tprimary_culture = afro_caribbean\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		if (x(countries, "HAI"))
			effect += "\t\t\t2214 = { remove_core = HAI } #Same as for Haiti\n";
		effect += "\t\t\tany_owned = {\n";
		effect += "\t\t\t\tadd_core = DOM\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\tchange_tag = DOM\n";
		effect += "\t\t\tcapital = 2214\n";
		if (x(countries, "HAI"))
			effect += "\t\t\trelease_vassal = HAI\n";
		effect += "\t\t\tset_country_flag = postcol_1\n";
		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("postcol_COS"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_COS decision";
	else if (!x(countries, "COS"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = COS }\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "UCA"))
			potential += "\t\t\t\tNOT = { tag = UCA }\n";
		potential += "\t\t\t\tAND = {\n";
		if (x(countries, "UCA"))
			potential += "\t\t\t\t\ttag = UCA\n";
		if (x(countries, "ELS"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = ELS }\n";
		if (x(countries, "GUA"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = GUA }\n";
		if (x(countries, "HON"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = HON }\n";
		if (x(countries, "NIC"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = NIC }\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		if (x(countries, "MEX"))
			potential += "\t\t\tNOT = { tag = MEX }\n";
		potential += "\t\t\tcapital_scope = { is_core = COS }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tCOS = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "MEX"))
			allow += "\t\t\t\t\t\towned_by = MEX\n";
		if (x(countries, "UCA"))
			allow += "\t\t\t\t\t\towned_by = UCA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_HON"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_HON decision";
	else if (!x(countries, "HON"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = HON }\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "UCA"))
			potential += "\t\t\t\tNOT = { tag = UCA }\n";
		potential += "\t\t\t\tAND = {\n";
		if (x(countries, "UCA"))
			potential += "\t\t\t\t\ttag = UCA\n";
		if (x(countries, "COS"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = COS }\n";
		if (x(countries, "ELS"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = ELS }\n";
		if (x(countries, "GUA"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = GUA }\n";
		if (x(countries, "NIC"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = NIC }\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		if (x(countries, "MEX"))
			potential += "\t\t\tNOT = { tag = MEX }\n";
		potential += "\t\t\tcapital_scope = { is_core = HON }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tHON = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "MEX"))
			allow += "\t\t\t\t\t\towned_by = MEX\n";
		if (x(countries, "UCA"))
			allow += "\t\t\t\t\t\towned_by = UCA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_NIC"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_NIC decision";
	else if (!x(countries, "NIC"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = NIC }\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "UCA"))
			potential += "\t\t\t\tNOT = { tag = UCA }\n";
		potential += "\t\t\t\tAND = {\n";
		if (x(countries, "UCA"))
			potential += "\t\t\t\t\ttag = UCA\n";
		if (x(countries, "COS"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = COS }\n";
		if (x(countries, "ELS"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = ELS }\n";
		if (x(countries, "GUA"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = GUA }\n";
		if (x(countries, "HON"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = HON }\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		if (x(countries, "MEX"))
			potential += "\t\t\tNOT = { tag = MEX }\n";
		potential += "\t\t\tcapital_scope = { is_core = NIC }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tNIC = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "MEX"))
			allow += "\t\t\t\t\t\towned_by = MEX\n";
		if (x(countries, "UCA"))
			allow += "\t\t\t\t\t\towned_by = UCA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_ELS"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_ELS decision";
	else if (!x(countries, "ELS"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = ELS }\n";
		if (x(countries, "HON"))
			potential += "\t\t\tNOT = { tag = HON }\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "UCA"))
			potential += "\t\t\t\tNOT = { tag = UCA }\n";
		potential += "\t\t\t\tAND = {\n";
		if (x(countries, "UCA"))
			potential += "\t\t\t\t\ttag = UCA\n";
		if (x(countries, "COS"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = COS }\n";
		if (x(countries, "GUA"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = GUA }\n";
		if (x(countries, "HON"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = HON }\n";
		if (x(countries, "NIC"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = NIC }\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		if (x(countries, "MEX"))
			potential += "\t\t\tNOT = { tag = MEX }\n";
		potential += "\t\t\tcapital_scope = { is_core = ELS }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tELS = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "HON"))
			allow += "\t\t\t\t\t\towned_by = HON\n";
		if (x(countries, "MEX"))
			allow += "\t\t\t\t\t\towned_by = MEX\n";
		if (x(countries, "UCA"))
			allow += "\t\t\t\t\t\towned_by = UCA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_PNM"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_PNM decision";
	else if (!x(countries, "PNM"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = PNM }\n";
		if (x(countries, "UCA"))
			potential += "\t\t\tNOT = { tag = UCA }\n";
		if (x(countries, "MEX"))
			potential += "\t\t\tNOT = { tag = MEX }\n";
		potential += "\t\t\tcapital_scope = { is_core = PNM }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tPNM = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "MEX"))
			allow += "\t\t\t\t\t\towned_by = MEX\n";
		if (x(countries, "UCA"))
			allow += "\t\t\t\t\t\towned_by = UCA\n";
		if (x(countries, "CLM"))
			allow += "\t\t\t\t\t\towned_by = CLM\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_GUA"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_GUA decision";
	else if (!x(countries, "GUA"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = GUA }\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "UCA"))
			potential += "\t\t\t\tNOT = { tag = UCA }\n";
		potential += "\t\t\t\tAND = {\n";
		if (x(countries, "UCA"))
			potential += "\t\t\t\t\ttag = UCA\n";
		if (x(countries, "COS"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = COS }\n";
		if (x(countries, "ELS"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = ELS }\n";
		if (x(countries, "HON"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = HON }\n";
		if (x(countries, "NIC"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = NIC }\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		if (x(countries, "MEX"))
			potential += "\t\t\tNOT = { tag = MEX }\n";
		potential += "\t\t\tcapital_scope = { is_core = GUA }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tGUA = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "MEX"))
			allow += "\t\t\t\t\t\towned_by = MEX\n";
		if (x(countries, "UCA"))
			allow += "\t\t\t\t\t\towned_by = UCA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_UCA"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_UCA decision";
	else if (!x(countries, "UCA"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = UCA\n";
		if (x(countries, "MEX"))
			potential += "\t\t\t\ttag = MEX\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tcapital_scope = { is_core = UCA }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tUCA = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "MEX"))
			allow += "\t\t\t\t\t\towned_by = MEX\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_ECU"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_ECU decision";
	else if (!x(countries, "ECU"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = ECU }\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "GCO"))
			potential += "\t\t\t\tNOT = { tag = GCO }\n";
		potential += "\t\t\t\tAND = {\n";
		if (x(countries, "GCO"))
			potential += "\t\t\t\t\ttag = GCO\n";
		if (x(countries, "CLM"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = CLM }\n";
		if (x(countries, "VNZ"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = VNZ }\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		if (x(countries, "CLM"))
			potential += "\t\t\tNOT = { tag = CLM }\n";
		if (x(countries, "PEU"))
			potential += "\t\t\tNOT = { tag = PEU }\n";
		if (x(countries, "VNZ"))
			potential += "\t\t\tNOT = { tag = VNZ }\n";
		potential += "\t\t\tcapital_scope = { is_core = ECU }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tECU = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "GCO"))
			allow += "\t\t\t\t\t\towned_by = GCO\n";
		if (x(countries, "CLM"))
			allow += "\t\t\t\t\t\towned_by = CLM\n";
		if (x(countries, "PEU"))
			allow += "\t\t\t\t\t\towned_by = PEU\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_MEX"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_MEX decision";
	else if (!x(countries, "MEX"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = MEX }\n";
		if (x(countries, "USA"))
			potential += "\t\t\tNOT = { tag = USA }\n";
		if (x(countries, "UCA"))
			potential += "\t\t\tNOT = { tag = UCA }\n";
		potential += "\t\t\tcapital_scope = { is_core = MEX }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tMEX = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "TEX"))
			allow += "\t\t\t\t\t\towned_by = TEX\n";
		if (x(countries, "USA"))
			allow += "\t\t\t\t\t\towned_by = USA\n";
		if (x(countries, "UCA"))
			allow += "\t\t\t\t\t\towned_by = UCA\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_BRZ"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_BRZ decision";
	else if (!x(countries, "BRZ"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = BRZ }\n";
		if (x(countries, "URU"))
			potential += "\t\t\tNOT = { tag = URU }\n";
		potential += "\t\t\tcapital_scope = { is_core = BRZ }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("postcol_PEU"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_PEU decision";
	else if (!x(countries, "PEU"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = PEU }\n";
		if (x(countries, "GCO"))
			potential += "\t\t\tNOT = { tag = GCO }\n";
		if (x(countries, "ECU"))
			potential += "\t\t\tNOT = { tag = ECU }\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "PBC"))
			potential += "\t\t\t\tNOT = { tag = PBC }\n";
		potential += "\t\t\t\tAND = {\n";
		if (x(countries, "PBC"))
			potential += "\t\t\t\t\ttag = PBC\n";
		if (x(countries, "BOL"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = BOL }\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		if (x(countries, "PBC"))
			potential += "\t\t\tNOT = { tag = PBC }\n";
		potential += "\t\t\tcapital_scope = { is_core = PEU }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tPEU = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "GCO"))
			allow += "\t\t\t\t\t\towned_by = GCO\n";
		if (x(countries, "CLM"))
			allow += "\t\t\t\t\t\towned_by = CLM\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_ARG"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_ARG decision";
	else if (!x(countries, "ARG"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = ARG }\n";
		if (x(countries, "URU"))
			potential += "\t\t\tNOT = { tag = URU }\n";
		if (x(countries, "CHL"))
			potential += "\t\t\tNOT = { tag = CHL }\n";
		potential += "\t\t\tcapital_scope = { is_core = ARG }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tARG = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "PRG"))
			allow += "\t\t\t\t\t\towned_by = PRG\n";
		if (x(countries, "CHL"))
			allow += "\t\t\t\t\t\towned_by = CHL\n";
		if (x(countries, "URU"))
			allow += "\t\t\t\t\t\towned_by = URU\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_PRG"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_PRG decision";
	else if (!x(countries, "PRG"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = PRG }\n";
		if (x(countries, "ARG"))
			potential += "\t\t\tNOT = { tag = ARG }\n";
		if (x(countries, "URU"))
			potential += "\t\t\tNOT = { tag = URU }\n";
		if (x(countries, "BOL"))
			potential += "\t\t\tNOT = { tag = BOL }\n";
		if (x(countries, "CHL"))
			potential += "\t\t\tNOT = { tag = CHL }\n";
		potential += "\t\t\tcapital_scope = { is_core = PRG }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tPRG = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "ARG"))
			allow += "\t\t\t\t\t\towned_by = ARG\n";
		if (x(countries, "CHL"))
			allow += "\t\t\t\t\t\towned_by = CHL\n";
		if (x(countries, "URU"))
			allow += "\t\t\t\t\t\towned_by = URU\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_URU"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_URU decision";
	else if (!x(countries, "URU"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = URU }\n";
		if (x(countries, "ARG"))
			potential += "\t\t\tNOT = { tag = ARG }\n";
		if (x(countries, "PRG"))
			potential += "\t\t\tNOT = { tag = PRG }\n";
		if (x(countries, "BOL"))
			potential += "\t\t\tNOT = { tag = BOL }\n";
		if (x(countries, "CHL"))
			potential += "\t\t\tNOT = { tag = CHL }\n";
		potential += "\t\t\tcapital_scope = { is_core = URU }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tURU = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "PRG"))
			allow += "\t\t\t\t\t\towned_by = PRG\n";
		if (x(countries, "CHL"))
			allow += "\t\t\t\t\t\towned_by = CHL\n";
		if (x(countries, "ARG"))
			allow += "\t\t\t\t\t\towned_by = ARG\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_CHL"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_CHL decision";
	else if (!x(countries, "CHL"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = CHL }\n";
		if (x(countries, "ARG"))
			potential += "\t\t\tNOT = { tag = ARG }\n";
		if (x(countries, "PEU"))
			potential += "\t\t\tNOT = { tag = PEU }\n";
		if (x(countries, "BOL"))
			potential += "\t\t\tNOT = { tag = BOL }\n";
		potential += "\t\t\tcapital_scope = { is_core = CHL }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tCHL_2322 = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "PRG"))
			allow += "\t\t\t\t\t\towned_by = PRG\n";
		if (x(countries, "BOL"))
			allow += "\t\t\t\t\t\towned_by = BOL\n";
		if (x(countries, "ARG"))
			allow += "\t\t\t\t\t\towned_by = ARG\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t\tCHL_2327 = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "PRG"))
			allow += "\t\t\t\t\t\towned_by = PRG\n";
		if (x(countries, "BOL"))
			allow += "\t\t\t\t\t\towned_by = BOL\n";
		if (x(countries, "ARG"))
			allow += "\t\t\t\t\t\towned_by = ARG\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t\tCHL_2332 = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "PRG"))
			allow += "\t\t\t\t\t\towned_by = PRG\n";
		if (x(countries, "BOL"))
			allow += "\t\t\t\t\t\towned_by = BOL\n";
		if (x(countries, "ARG"))
			allow += "\t\t\t\t\t\towned_by = ARG\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_BOL"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_BOL decision";
	else if (!x(countries, "BOL"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = BOL }\n";
		if (x(countries, "ARG"))
			potential += "\t\t\tNOT = { tag = ARG }\n";
		if (x(countries, "BRZ"))
			potential += "\t\t\tNOT = { tag = BRZ }\n";
		if (x(countries, "BOL"))
			potential += "\t\t\tNOT = { tag = BOL }\n";
		if (x(countries, "CHL"))
			potential += "\t\t\tNOT = { tag = CHL }\n";
		if (x(countries, "PBC") && x(countries, "PEU"))
		{
			potential += "\t\t\tOR = {\n";
			potential += "\t\t\t\tNOT = { tag = PBC }\n";
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = PBC\n";
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = PEU }\n";
			potential += "\t\t\t\t}\n";
			potential += "\t\t\t}\n";
		}
		else if (x(countries, "PBC"))
			potential += "\t\t\tNOT = { tag = PBC }\n";
		potential += "\t\t\tcapital_scope = { is_core = BOL }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tBOL = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "CHL"))
			allow += "\t\t\t\t\t\towned_by = CHL\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_CLM"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_CLM decision";
	else if (!x(countries, "CLM"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = CLM }\n";
		if (x(countries, "VNZ"))
			potential += "\t\t\tNOT = { tag = VNZ }\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "GCO"))
			potential += "\t\t\t\tNOT = { tag = GCO }\n";
		if (x(countries, "GCO"))
			potential += "\t\t\t\tAND = {\n";
		potential += "\t\t\t\t\ttag = GCO\n";
		if (x(countries, "ECU"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = ECU }\n";
		if (x(countries, "VNZ"))
			potential += "\t\t\t\t\tNOT = { is_possible_vassal = VNZ }\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		if (x(countries, "ECU"))
			potential += "\t\t\tNOT = { tag = ECU }\n";
		potential += "\t\t\tcapital_scope = { is_core = CLM }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tCLM = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "GCO"))
			allow += "\t\t\t\t\t\towned_by = GCO\n";
		if (x(countries, "VNZ"))
			allow += "\t\t\t\t\t\towned_by = VNZ\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_VNZ"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_VNZ decision";
	else if (!x(countries, "VNZ"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = VNZ }\n";
		if (x(countries, "GCO"))
		{
			potential += "\t\t\tOR = {\n";
			potential += "\t\t\t\tNOT = { tag = GCO }\n";
			potential += "\t\t\t\tAND = {\n";
			potential += "\t\t\t\t\ttag = GCO\n";
			if (x(countries, "ECU"))
				potential += "\t\t\t\t\tNOT = { is_possible_vassal = ECU }\n";
			if (x(countries, "CLM"))
				potential += "\t\t\t\t\tNOT = { is_possible_vassal = CLM }\n";
			potential += "\t\t\t\t}\n";
			potential += "\t\t\t}\n";
		}
		if (x(countries, "CLM"))
			potential += "\t\t\tNOT = { tag = CLM }\n";
		if (x(countries, "ECU"))
			potential += "\t\t\tNOT = { tag = ECU }\n";
		if (x(countries, "BRZ"))
			potential += "\t\t\tNOT = { tag = BRZ }\n";
		potential += "\t\t\tcapital_scope = { is_core = VNZ }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_independant = yes\n";
		allow += "\t\t\tVNZ = {\n";
		allow += "\t\t\t\tall_core = {\n";
		allow += "\t\t\t\t\tOR = {\n";
		allow += "\t\t\t\t\t\towned_by = THIS\n";
		allow += "\t\t\t\t\t\tempty = yes\n";
		allow += "\t\t\t\t\t\towner = {\n";
		allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
		allow += "\t\t\t\t\t\t}\n";
		if (x(countries, "GCO"))
			allow += "\t\t\t\t\t\towned_by = GCO\n";
		if (x(countries, "CLM"))
			allow += "\t\t\t\t\t\towned_by = CLM\n";
		allow += "\t\t\t\t\t}\n";
		allow += "\t\t\t\t}\n";
		allow += "\t\t\t}\n";
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);
	}

	if (const auto& theDecision = decisions.find("postcol_AST"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_AST decision";
	else if (!x(countries, "AST"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = { exists = AST }\n";
		potential += "\t\t\tcapital_scope = { is_core = AST }\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		if (x(countries, "FIJ"))
			potential += "\t\t\tNOT = { tag = FIJ }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("postcol_NZL"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_NZL decision";
	else if (!x(countries, "NZL"))
		decisions.erase(theDecision);

	if (const auto& theDecision = decisions.find("postcol_GUY"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_GUY decision";
	else if (!x(countries, "GUY"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = GUY\n";
		if (x(countries, "GCO"))
			potential += "\t\t\t\ttag = GCO\n";
		if (x(countries, "BRZ"))
			potential += "\t\t\t\ttag = BRZ\n";
		if (x(countries, "VNZ"))
			potential += "\t\t\t\ttag = VNZ\n";
		if (x(countries, "SUR"))
			potential += "\t\t\t\ttag = SUR\n";
		if (x(countries, "CAY"))
			potential += "\t\t\t\ttag = CAY\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tcapital_scope = {\n";
		potential += "\t\t\t\tOR = {\n";
		potential += "\t\t\t\t\tis_core = GUY\n";
		if (x(countries, "SUR"))
			potential += "\t\t\t\t\tis_core = SUR\n";
		if (x(countries, "CAY"))
			potential += "\t\t\t\t\tis_core = CAY\n";
		potential += "\t\t\t\t}\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_1 }\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("the_central_asia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load the_central_asia decision";
	else if (!x(countries, "TKS"))
		decisions.erase(theDecision);
	else
	{
		std::istringstream effectString(theDecision->second.getEffect());
		Taiping centralAsia(effectString);

		std::string effect = "= {\n";
		effect += "\t\t\tset_global_flag = the_central_asia\n";

		for (const auto& nonCountrySpecificEffect: centralAsia.getNonCountrySpecificEffects())
		{
			effect += "\t\t\t" + nonCountrySpecificEffect + "\n";
		}
		for (const auto& tag: centralAsia.getTagEffectMap())
		{
			if (x(countries, tag.first))
			{
				effect += "\t\t\t" + tag.second + "\n";
			}
		}
		for (const auto& coreEffect: centralAsia.getCountryCores())
		{
			effect += "\t\t\t" + coreEffect + "\n";
		}

		effect += "\t\t}\n";
		(theDecision->second).updateDecision("effect", effect);
	}

	if (const auto& theDecision = decisions.find("postcol_CRB"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load postcol_CRB decision";
	else if (!x(countries, "CRB"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = CRB\n";
		if (x(countries, "CUB"))
			potential += "\t\t\t\ttag = CUB\n";
		if (x(countries, "HAI"))
			potential += "\t\t\t\ttag = HAI\n";
		if (x(countries, "DOM"))
			potential += "\t\t\t\ttag = DOM\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\tcapital = 2236\n";
		potential += "\t\t\t\tcapital = 2224\n";
		potential += "\t\t\t\tcapital = 2229\n";
		potential += "\t\t\t\tcapital = 2219\n";
		potential += "\t\t\t\tcapital = 2220\n";
		potential += "\t\t\t\tcapital = 2234\n";
		potential += "\t\t\t\tcapital = 2235\n";
		potential += "\t\t\t\tcapital = 2232\n";
		potential += "\t\t\t\tcapital = 2230\n";
		potential += "\t\t\t\tcapital = 2233\n";
		potential += "\t\t\t\tcapital = 2225\n";
		potential += "\t\t\t\tcapital = 2228\n";
		potential += "\t\t\t\tcapital = 2227\n";
		potential += "\t\t\t\tcapital = 2231\n";
		potential += "\t\t\t\tcapital = 2409\n";
		potential += "\t\t\t\tcapital = 2217\n";
		potential += "\t\t\t\tcapital = 2218\n";
		potential += "\t\t\t\tcapital = 2222\n";
		potential += "\t\t\t\tcapital = 2223\n";
		potential += "\t\t\t\tcapital = 2237\n";
		potential += "\t\t\t\tcapital = 2238\n";
		potential += "\t\t\t\tcapital = 2226\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("vassal_CRB"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load vassal_CRB decision";
	else if (!x(countries, "CRB"))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = CRB\n";
		if (x(countries, "CUB"))
			potential += "\t\t\t\ttag = CUB\n";
		if (x(countries, "HAI"))
			potential += "\t\t\t\ttag = HAI\n";
		if (x(countries, "DOM"))
			potential += "\t\t\t\ttag = DOM\n";
		if (x(countries, "ALI"))
			potential += "\t\t\t\ttag = ALI #Yeah...\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { has_country_flag = precolonial }\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\tcapital = 2236\n";
		potential += "\t\t\t\tcapital = 2224\n";
		potential += "\t\t\t\tcapital = 2229\n";
		potential += "\t\t\t\tcapital = 2219\n";
		potential += "\t\t\t\tcapital = 2220\n";
		potential += "\t\t\t\tcapital = 2234\n";
		potential += "\t\t\t\tcapital = 2235\n";
		potential += "\t\t\t\tcapital = 2232\n";
		potential += "\t\t\t\tcapital = 2230\n";
		potential += "\t\t\t\tcapital = 2233\n";
		potential += "\t\t\t\tcapital = 2225\n";
		potential += "\t\t\t\tcapital = 2228\n";
		potential += "\t\t\t\tcapital = 2227\n";
		potential += "\t\t\t\tcapital = 2231\n";
		potential += "\t\t\t\tcapital = 2409\n";
		potential += "\t\t\t\tcapital = 2217\n";
		potential += "\t\t\t\tcapital = 2218\n";
		potential += "\t\t\t\tcapital = 2222\n";
		potential += "\t\t\t\tcapital = 2223\n";
		potential += "\t\t\t\tcapital = 2237\n";
		potential += "\t\t\t\tcapital = 2238\n";
		potential += "\t\t\t\tcapital = 2226\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tOR = {\n";
		potential += "\t\t\t\t2236 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2224 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2229 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2219 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2220 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2232 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2230 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2233 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2234 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2235 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2225 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2226 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2228 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2227 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2231 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2409 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2217 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2218 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2222 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2223 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2237 = { owned_by = THIS }\n";
		potential += "\t\t\t\t2238 = { owned_by = THIS }\n";
		potential += "\t\t\t}\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);
	}

	if (const auto& theDecision = decisions.find("form_peru_bolivia"); theDecision == decisions.end())
		Log(LogLevel::Warning) << "Could not load form_peru_bolivia decision";
	else if (!x(countries, "PBC") || (!x(countries, "PEU") && !x(countries, "BOL")))
		decisions.erase(theDecision);
	else
	{
		std::string potential = "= {\n";
		potential += "\t\t\thas_global_flag = der_postcolonialism\n";
		potential += "\t\t\tOR = {\n";
		if (x(countries, "PEU"))
			potential += "\t\t\t\ttag = PEU\n";
		if (x(countries, "BOL"))
			potential += "\t\t\t\ttag = BOL\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = {\n";
		potential += "\t\t\t\texists = PBC\n";
		potential += "\t\t\t}\n";
		potential += "\t\t\tNOT = { has_country_flag = postcol_2 }\n";
		potential += "\t\t}\t\n";
		(theDecision->second).updateDecision("potential", potential);

		std::string allow = "= {\n";
		allow += "\t\t\twar = no\n";
		allow += "\t\t\tis_greater_power = yes\n";
		if (x(countries, "PEU"))
		{
			allow += "\t\t\tPEU = {\n";
			allow += "\t\t\t\tall_core = {\n";
			allow += "\t\t\t\t\towner = {\n";
			allow += "\t\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\t\ttag = THIS\n";
			allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
			allow += "\t\t\t\t\t\t\tvassal_of = THIS\n";
			allow += "\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t}\n";
			allow += "\t\t\t\t}\n";
			allow += "\t\t\t}\n";
		}
		if (x(countries, "BOL"))
		{
			allow += "\t\t\tBOL = {\n";
			allow += "\t\t\t\tall_core = {\n";
			allow += "\t\t\t\t\towner = {\n";
			allow += "\t\t\t\t\t\tOR = {\n";
			allow += "\t\t\t\t\t\t\ttag = THIS\n";
			allow += "\t\t\t\t\t\t\tin_sphere = THIS\n";
			allow += "\t\t\t\t\t\t\tvassal_of = THIS\n";
			allow += "\t\t\t\t\t\t}\n";
			allow += "\t\t\t\t\t}\n";
			allow += "\t\t\t\t}\n";
			allow += "\t\t\t}\n";
		}
		allow += "\t\t}\n";
		(theDecision->second).updateDecision("allow", allow);

		std::string effect = "= {\n";
		effect += "\t\t\tprestige = 10\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tvassal_of = THIS\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\tannex_to = THIS\n";
		effect += "\t\t\t}\n";
		effect += "\t\t\t\n";
		if (x(countries, "PEU"))
			effect += "\t\t\tPEU = { all_core = { add_core = PBC } }\n";
		if (x(countries, "BOL"))
			effect += "\t\t\tBOL = { all_core = { add_core = PBC } }\n";
		effect += "\t\t\tany_country = {\n";
		effect += "\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\tin_sphere = THIS\n";
		effect += "\t\t\t\t}\n";
		effect += "\t\t\t\t\n";
		effect += "\t\t\t\tany_owned = {\n";
		effect += "\t\t\t\t\tlimit = {\n";
		effect += "\t\t\t\t\t\tis_core = PBC\n";
		effect += "\t\t\t\t\t}\n";
		effect += "\t\t\t\t\tsecede_province = THIS\n";
		effect += "\t\t\t\t}\n";
		effect += "\n";
		effect += "\t\t\t}\n";
		effect += "\n";
		effect += "\t\t\tchange_tag = PBC\n";
		effect += "\t\t\tset_country_flag = postcol_2\n";
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
			if (x(countries, tag.first) && std::find(heimaten.begin(), heimaten.end(), tag.first) == heimaten.end() && tag.first != "CHI")
			{
				effect += "\t\t\t" + tag.second + "\n";
			}
		}
		for (const auto& coreEffect: taiping.getCountryCores())
		{
			effect += "\t\t\t" + coreEffect + "\n";
		}

		// Heimaten
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
		allow += "\t\t}\n";
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