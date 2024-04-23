//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <NESInteraction/NESSimLoad.h>



namespace BG {

bool GetNESStatus(BG::Common::Logger::LoggingSystem* _Logger, SafeClient & _Client, BGStatusCode & _StatusCode) {
	std::string CheckStatusRequest("");
	std::string Response;
	bool Status = _Client.MakeJSONQuery("", CheckStatusRequest, &Response);

	if (!Status) {
		_Logger->Log("Status request to NES failed", 7);
    	return false;
	}

	nlohmann::json ResponseJSON(Response);
	auto Iterator = Response.find("StatusCode");
	if (Iterator == Response.end()) {
		_Logger->Log("No 'StatusCode' in loading status response", 7);
		return false;
	}

	if (!Iterator.value().is_number()) {
        _Logger->Log("Error StatusCode is not a number", 7);
        return false;
	}

	_StatusCode = static_cast<BGStatusCode>(Iterator.value().template get<int>());
	return true;
}

bool AwaitNESOutcome(BG::Common::Logger::LoggingSystem* _Logger, SafeClient& _Client, unsigned long _Timeout_ms) {
	unsigned long Timeout_ms = _Timeout_ms;
	while (true) {
		BGStatusCode StatusCode;
		if (!GetNESStatus(_Logger, _Client, StatusCode)) {
			_Logger->Log("NES Status request failed while waiting for process to complete", 7);
			return false;
		}

    	if (StatusCode == BGStatusSuccess) {
    		return true; // Process appears to be done.
    	}

    	if (StatusCode != BGStatusSimulationBusy) {
    		_Logger->Log("NES Process status code returned error: "+std::to_string(static_cast<int>(StatusCode)), 7);
	        return false;
    	}

		Timeout_ms--;
		if (Timeout_ms==0) {
			_Logger->Log("Awaiting NES Process request timed out after "+std::to_string(_Timeout_ms)+" ms", 7);
        	return false;
		}
		std::this_thread::sleep_for (std::chrono::milliseconds(1));
	}
}

/**
 * Ask NES to load a previously saved simulation and wait for loading
 * to complete or fail.
 * 
 * @param _Client Reference to client connection with NES.
 * @param _SimSaveName Name of the previously saved simulation.
 * @param _SimID Reference through which to return the simulation ID.
 * @param _Timeout_ms Timeout that ensures this function cannot become stuck forever (e.g. due to broken connection),
 * @return True if loading was successful.
 */
bool AwaitNESSimLoad(BG::Common::Logger::LoggingSystem* _Logger, SafeClient & _Client, const std::string & _SimSaveName, int & _SimID, unsigned long _Timeout_ms) {

	_Logger->Log("Await NES Sim Load '" + _SimSaveName + "'", 1);

	// Start a simulation load request.

	std::string SimLoadRequest("[{\"ReqID\":0,\"Simulation/Load\": { \"SavedSimName\": "+_SimSaveName+" } }]");
	std::string Response;
	bool Status = _Client.MakeJSONQuery("Simulation/Load", SimLoadRequest, &Response);

	if (!Status) {
        _Logger->Log("Error During Simulation Load Request To NES", 7);
        return false;
    }

	// Wait for status to indicate that loading completed or failed.

	if (!AwaitNESOutcome(_Client, _Timeout_ms)) {
		_Logger->Log("Awaiting completion of NES load request failed", 7);
		return false;
	}

	// If successful then return the simulation ID.

	// *** Right... this needs some special support, because loading right now does not
	//     work as a process that can be awaited...

	return true;
}

} // BG
