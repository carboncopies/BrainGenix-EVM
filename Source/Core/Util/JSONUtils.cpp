/**
 * This set of JSON utility functions is very similar to the set in JSONHelpers
 * and derived from the HandlerData class in NES.
 * Added this set, to simplify access through fewer namespaces and to ensure
 * more tests to prevent serious errors or crashes of the server. (R.K.)
 */

#include <Vector/Vec3D.h>
#include <Util/JSONUtils.h>

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
BGStatusCode FindPar(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& _ParName, nlohmann::json::iterator& _Iterator) {
    auto Iterator = _JSON.find(_ParName);
    if (Iterator == _JSON.end()) {
        Logger_.Log("Error Finding Parameter '" + _ParName + "', Request Is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParBool(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, bool& Value) {
    nlohmann::json::iterator it;
    if (FindPar(Logger_, _JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (!it.value().is_boolean()) {
        Logger_.Log("Error Parameter '" + ParName + "', Wrong Type (expected bool) Request Is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    Value = it.value().template get<bool>();
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParInt(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, long& Value) {
    nlohmann::json::iterator it;
    if (FindPar(Logger_, _JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (!it.value().is_number()) {
        Logger_.Log("Error Parameter '" + ParName + "', Wrong Type (expected number) Request Is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    Value = it.value().template get<long>();
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParFloat(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, float& Value) {
    nlohmann::json::iterator it;
    if (FindPar(Logger_, _JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (!it.value().is_number()) {
        Logger_.Log("Error Parameter '" + ParName + "', Wrong Type (expected number) Request Is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    Value = it.value().template get<float>();
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParString(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, std::string& Value) {
    nlohmann::json::iterator it;
    if (FindPar(Logger_, _JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (!it.value().is_string()) {
        Logger_.Log("Error Parameter '" + ParName + "', Wrong Type (expected string) Request Is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    Value = it.value().template get<std::string>();
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParIntVec(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, std::vector<long>& Values) {
    nlohmann::json::iterator it;
    if (FindPar(Logger_, _JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (!it.value().is_array()) {
        Logger_.Log("Error Parameter '" + ParName + "', Wrong Type (expected array) Request Is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    nlohmann::json& list(it.value());
    size_t listsize = list.size();
    Values.resize(listsize);
    for (size_t i = 0; i < listsize; i++) {
        if (!list[i].is_number()) {
            Logger_.Log("Error Parameter '" + ParName + "', Wrong Type (expected array of integers) Request Is: " + _JSON.dump(), 7);
            return BGStatusCode::BGStatusInvalidParametersPassed;
        }
        Values[i] = list[i].template get<long>();
    }
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode ExtractVec3D(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, Vec3D& Value) {
    if (!_JSON.is_array()) {
        Logger_.Log("Error, Wrong Type (expected array) Request Is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (_JSON.size() != 3) {
        Logger_.Log("Error, Wrong array size (expected 3D vector). Request excerpt is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    for (size_t j = 0; j < 3; j++) {
        if (!_JSON[j].is_number()) {
            Logger_.Log("Error, Wrong Type (expected 3D float vector). Request excerpt is: " + _JSON.dump(), 7);
            return BGStatusCode::BGStatusInvalidParametersPassed;
        }
        Value[j] = _JSON[j].template get<float>();
    }
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParVec3D(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, Vec3D& Value) {
    nlohmann::json::iterator it;
    if (FindPar(Logger_, _JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (ExtractVec3D(Logger_, it.value(), Value) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    return BGStatusCode::BGStatusSuccess;
}

BGStatusCode GetParVecVec3D(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, std::vector<Vec3D>& Values) {
    nlohmann::json::iterator it;
    if (FindPar(Logger_, _JSON, ParName, it) != BGStatusCode::BGStatusSuccess) {
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    if (!it.value().is_array()) {
        Logger_.Log("Error Parameter '" + ParName + "', Wrong Type (expected array) Request Is: " + _JSON.dump(), 7);
        return BGStatusCode::BGStatusInvalidParametersPassed;
    }
    nlohmann::json& list(it.value());
    size_t listsize = list.size();
    Values.resize(listsize);
    for (size_t i = 0; i < listsize; i++) {
        if (ExtractVec3D(Logger_, list[i], Values[i]) != BGStatusCode::BGStatusSuccess) {
            return BGStatusCode::BGStatusInvalidParametersPassed;
        }
    }
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
