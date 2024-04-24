//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <NESInteraction/NESRequest.h>

namespace BG {

const std::vector<std::string> BgErrorStr = {
	"BGStatusSuccess",
    "BGStatusGeneralFailure",
    "BGStatusInvalidParametersPassed",
    "BGStatusUpstreamGatewayUnavailable",
    "BGStatusUnauthorizedInvalidNoToken",
    "BGStatusSimulationBusy",
};

bool MakeNESRequest(SafeClient& _Client, const std::string& _Route, const nlohmann::json& _Data, nlohmann::json& _Result) {
	if (_Route.empty()) {
		_Client.Logger_->Log("Route must be non-empty string.", 7);
		return false
	}
	nlohmann::json RequestJSON;
	RequestJSON["ReqID"] = _Client.GetRequestID();
	RequestJSON[_Route] = _Data;
    std::string ResultStr;
	bool Status = _Client.MakeJSONQuery(_Route, '['+RequestJSON.dump()+']', &ResultStr);
	if (!Status) {
        _Client.Logger_->Log("Error During Simulation Load Request To NES", 7);
        return false;
    }
    _Result = nlohmann::json::parse(ResultStr);

    if (!_Result.is_array()) {
    	_Client.Logger_->Log("Bad response format. Expected list of NESRequest responses.", 7);
    	return false;
    }

    nlohmann::json& FirstResponse = _Result[0];
    if (!FirstResponse.is_object()) {
    	_Client.Logger_->Log("Bad response format. Response items must be JSON objects.", 7);
    	return false;
    }

    if (FirstResponse.find("StatusCode") == FirstResponse.end()) {
    	_Client.Logger_->Log("Bad response format. Missing StatusCode.", 7);
    	return false;
    }

    if (!FirstResponse["StatusCode"].is_number()) {
    	_Client.Logger_->Log("Bad response format. StatusCode must be a number.", 7);
    	return false;
    }

    unsigned int StatusCode = FirstResponse["StatusCode"];
    if (StatusCode != 0) {
    	if (StatusCode >= BgErrorStr.size()) {
    		_Client.Logger_->Log(_Route+" failed. Unknown StatusCode: "+std::to_string(StatusCode), 7);
    	} else {
    		_Client.Logger_->Log(_Route+" failed. StatusCode: "+BgErrorStr[StatusCode], 7);
    	}
    	return false;
    }

    return true;
}

} // BG
