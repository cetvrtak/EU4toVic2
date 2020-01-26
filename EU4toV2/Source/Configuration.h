#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "EU4World/EU4Version.h"
#include "Date.h"
#include "newParser.h"
#include <string>
#include <vector>

class Configuration: commonItems::parser
{
	public:
		Configuration() = default;
		Configuration(const Configuration&) = default;
		Configuration(Configuration&&) = default;
		Configuration& operator=(const Configuration&) = default;
		Configuration& operator=(Configuration&&) = default;
		~Configuration() = default;

		void instantiate(std::istream& theStream, bool (*doesFolderExist)(const std::string& path), bool (*doesFileExist)(const std::string& path));

		std::string getEU4Path() { return EU4Path; }
		std::string getEU4DocumentsPath() const { return EU4DocumentsPath; }
		int getEU4RandomSeed() const { return eu4Seed; }
		std::string getSteamWorkshopPath() const { return SteamWorkshopPath; }
		std::string getCK2ExportPath() const { return CK2ExportPath; }
		std::string getVic2Path() { return Vic2Path; }
		std::string getVic2DocumentsPath() { return Vic2DocumentsPath; }
		std::string getResetProvinces() { return resetProvinces; }
		double getMaxLiteracy() { return MaxLiteracy; }
		double getLibertyThreshold() { return libertyThreshold; }
		enum class DEADCORES { LeaveAll = 1, DeadCores = 2, AllCores = 3 };
		enum class POPSHAPES { Vanilla = 1, PopShaping = 2, Extreme = 3};
		enum class COREHANDLES { DropNone = 1, DropNational = 2, DropUnions = 3, DropAll = 4};
		enum class EUROCENTRISM { VanillaImport = 1, EuroCentric = 2 };
		POPSHAPES getPopShaping() { return popShaping; }
		COREHANDLES getCoreHandling() { return coreHandling; }
		DEADCORES getRemoveType() { return removeType; }
		EUROCENTRISM getEuroCentrism() { return euroCentric; }
		double getPopShapingFactor() { return popShapingFactor; }
		bool getDebug() const { return debug; }
		bool getRandomiseRgos() const { return randomiseRgos; }

		const EU4::Version& getEU4Version() const { return version; }
		date getFirstEU4Date() const { return firstEU4Date; }
		date getLastEU4Date() const { return lastEU4Date; }
		date getStartEU4Date() const { return startEU4Date; }
		std::string getOutputName() { return outputName; }
		std::vector<std::string> getEU4Mods() { return EU4Mods; }

		void setFirstEU4Date(date _firstDate) { firstEU4Date = _firstDate; }
		void setLastEU4Date(date _lastDate) { lastEU4Date = _lastDate; }
		void setStartEU4Date(date _startDate) { startEU4Date = _startDate; }
		void setOutputName(std::string name) { outputName = name; }
		void addEU4Mod(std::string mod) { EU4Mods.push_back(mod); }
		void setEU4Version(EU4::Version _version) { version = _version; }
		void setEU4RandomSeed(int s) { eu4Seed = s; }
		void setActiveDLCs(std::vector<std::string> _activeDLCs) { activeDLCs = _activeDLCs; }

		bool wasDLCActive(const std::string& DLC);

	private:
		void verifyEU4Path(const std::string& path, bool (*doesFolderExist)(const std::string& path), bool (*doesFileExist)(const std::string& path));
		void verifyVic2Path(const std::string& path, bool (*doesFolderExist)(const std::string& path), bool (*doesFileExist)(const std::string& path));
		void verifyVic2DocumentsPath(const std::string& path, bool (*doesFolderExist)(const std::string& path));

		// options from configuration.txt
		std::string EU4Path;
		std::string EU4DocumentsPath;
		std::string SteamWorkshopPath;
		std::string CK2ExportPath;
		std::string Vic2Path;
		std::string Vic2DocumentsPath;
		std::string resetProvinces = "no";
		double MaxLiteracy = 1.0;
		double libertyThreshold = 50.0;
		POPSHAPES popShaping = POPSHAPES::Vanilla;
		COREHANDLES coreHandling = COREHANDLES::DropNone;
		DEADCORES removeType = DEADCORES::DeadCores;
		EUROCENTRISM euroCentric = EUROCENTRISM::VanillaImport;
		double popShapingFactor = 50.0;
		bool debug = false;
		bool randomiseRgos = false;
	
		// items set during conversion
		EU4::Version version;
		date firstEU4Date;
		date lastEU4Date;
		date startEU4Date = date("1000.1.1");
		int eu4Seed = 0;
		std::string outputName;
		std::vector<std::string> activeDLCs;
		std::vector<std::string> EU4Mods;
};

extern Configuration theConfiguration;

class ConfigurationFile: commonItems::parser
{
	public:
		explicit ConfigurationFile(const std::string& filename);
		~ConfigurationFile() = default;

	private:
		ConfigurationFile() = delete;
		ConfigurationFile(const ConfigurationFile&) = delete;
		ConfigurationFile(ConfigurationFile&&) = delete;
		ConfigurationFile& operator=(const ConfigurationFile&) = delete;
		ConfigurationFile& operator=(ConfigurationFile&&) = delete;
};

#endif // CONFIGURATION_H
