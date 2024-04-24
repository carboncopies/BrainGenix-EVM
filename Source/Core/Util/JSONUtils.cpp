/**
 * This set of JSON utility functions is very similar to the set in JSONHelpers
 * and derived from the HandlerData class in NES.
 * Added this set, to simplify access through fewer namespaces and to ensure
 * more tests to prevent serious errors or crashes of the server. (R.K.)
 */

#include <Utils/JSONUtils.h>

namespace BG {

/**
 * Find a parameter at the first level of a JSON object.
 * 
 * To find parameters at another level, extract the parameter that leads
 * to that level and call this function with that parameter as the JSON object.
 * 
 * @param _JSON The JSON object to parse.
 * @param ParName Name of the parameter.
 * @param _Iterator Reference that returns the parameter if found.
 * @return True if found.
 */
BGStatusCode FindPar(const nlohmann::json& _JSON, const std::string& _ParName, nlohmann::json::iterator& _Iterator) {
    Iterator = _JSON.find(ParName);
    if (Iterator == _JSON.end()) {
        Logger_->Log("Error Finding Parameter '" + ParName + "', Request Is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParBool(const nlohmann::json& _JSON, const std::string& ParName, bool& Value) {
    nlohmann::json::iterator it;
    if (FindPar(_JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (!it.value().is_boolean()) {
        Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected bool) Request Is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    Value = it.value().template get<bool>();
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParInt(const nlohmann::json& _JSON, const std::string& ParName, int& Value) {
    nlohmann::json::iterator it;
    if (FindPar(_JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (!it.value().is_number()) {
        Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected number) Request Is: " + _JSON.dump(), 7);
        Status = BGStatusCode::BGStatusInvalidParametersPassed;
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    Value = it.value().template get<int>();
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParFloat(const nlohmann::json& _JSON, const std::string& ParName, float& Value) {
    nlohmann::json::iterator it;
    if (FindPar(_JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return false;
    }
    if (!it.value().is_number()) {
        Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected number) Request Is: " + _JSON.dump(), 7);
        Status = BGStatusCode::BGStatusInvalidParametersPassed;
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    Value = it.value().template get<float>();
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParString(const nlohmann::json& _JSON, const std::string& ParName, std::string& Value) {
    nlohmann::json::iterator it;
    if (FindPar(_JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (!it.value().is_string()) {
        Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected string) Request Is: " + _JSON.dump(), 7);
        Status = BGStatusCode::BGStatusInvalidParametersPassed;
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    Value = it.value().template get<std::string>();
    return BGStatusCode::BGStatusSuccess;
}

std::string Response(const nlohmann::json& ResponseJSON) {
    return ResponseJSON.dump();
}
std::string ErrResponse(int _Status) {
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = _Status;
    return Response(ResponseJSON);
}
std::string ErrResponse(int _Status, const std::string & _ErrStr) {
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = _Status;
    ResponseJSON["ErrorMessage"] = _ErrStr;
    return Response(ResponseJSON);
}
std::string ErrResponse(BGStatusCode _Status) {
    return ErrResponse(int(_Status));
}
std::string ErrResponse(BGStatusCode _Status, const std::string & _ErrStr) {
    return ErrResponse(int(_Status), _ErrStr);
}
std::string SuccessResponse() {
    return ErrResponse(BGStatusCode::BGStatusSuccess);
}

} // BG
