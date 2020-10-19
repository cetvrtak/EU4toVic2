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
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}