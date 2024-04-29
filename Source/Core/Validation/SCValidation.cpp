//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>
#include <unordered_map>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <PCRegistration/SimpleRegistration.h>
#include <Metrics/N1Metrics.h>
#include <Validation/SCValidation.h>

namespace BG {

bool GenerateReportJSON(const DataCollector& _CollectedData, nlohmann::json& _ReportJSON) {

	_ReportJSON["Connectomes"] = _CollectedData.GetConnectomesJSON();

	_ReportJSON["KGT2Emu"] = _CollectedData.GetKGT2EmuMapJSON();

	_ReportJSON["GraphEdits"] = _CollectedData.GetGraphEditsJSON();

	_ReportJSON["Scores"] = _CollectedData.GetScoresJSON();

	return true;
}

bool GenerateReportMD(const DataCollector& _CollectedData, nlohmann::json& _ReportJSON) {
	std::string ReportMD;

	// *** Not yet implemented.

	_ReportJSON["ReportMD"] = ReportMD;
	return true;
}

// Extensible enumerated collection of report styles
typedef bool (*ReportFunction)(const DataCollector&, nlohmann::json&); // function pointer type
typedef std::unordered_map<ReportType, ReportFunction> ReportMap;
const ReportMap ReportMap_ = {
	{ JSONReport, GenerateReportJSON },
	{ MDReport, GenerateReportMD },
};

Validation::Validation(SafeClient & _Client, const std::string & _KGTSaveName, const std::string & _EmuSaveName, const ValidationConfig & _Config):
    Client_(_Client), KGTSaveName_(_KGTSaveName), EmuSaveName_(_EmuSaveName), Config_(_Config), CollectedData_(_KGTSaveName, _EmuSaveName) {

}

// Configuration driven entry point for report generation.
bool Validation::GenerateReport() {
	auto it = ReportMap_.find(Config_.ReportType_);
    if (it == ReportMap_.end()) {
        Client_.Logger_->Log("Report type unrecognized: "+std::to_string(int(Config_.ReportType_)), 7);
        return false;
    }
	return (*it->second)(CollectedData_, ReportJSON_);
}

/**
 * This is a simple entry point through which to carry out validation
 * using metrics that area suitable for a pair of emulation and
 * ground-truth systems expressed using Simple Compartmental neurons.
 */
bool Validation::SCValidate() {

	Client_.Logger_->Log("Commencing validation of Simple Compartmental ground-truth and emulation systems.",1);

	// Apply the N1 success-criteria metrics
	N1Metrics N1Metrics_(Client_, Config_, CollectedData_);
	if (!N1Metrics_.Validate()) {
		return false;
	}

	// *** There need to be more metrics applied here!

	return GenerateReport();
}

} // BG
