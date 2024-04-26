/**
 * This set of JSON utility functions is very similar to the set in JSONHelpers
 * and derived from the HandlerData class in NES.
 * Added this set, to simplify access through fewer namespaces and to ensure
 * more tests to prevent serious errors or crashes of the server. (R.K.)
 */

#include <string>

#include <nlohmann/json.hpp>

#include <BG/Common/Logger/Logger.h>
#include <BGStatusCode.h>
#include <Vector/Vec3D.h>


namespace BG {

BGStatusCode FindPar(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& _ParName, nlohmann::json::iterator& _Iterator);
BGStatusCode GetParBool(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, bool& Value);
BGStatusCode GetParInt(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, long& Value);
BGStatusCode GetParFloat(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, float& Value);
BGStatusCode GetParString(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, std::string& Value);
BGStatusCode GetParIntVec(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, std::vector<long>& Values);
BGStatusCode GetParVec3D(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, Vec3D& Value);
BGStatusCode GetParVecVec3D(BG::Common::Logger::LoggingSystem& Logger_, const nlohmann::json& _JSON, const std::string& ParName, std::vector<Vec3D>& Values);

std::string Response(const nlohmann::json& ResponseJSON);
std::string ErrResponse(int _Status);
std::string ErrResponse(int _Status, const std::string & _ErrStr);
std::string ErrResponse(BGStatusCode _Status);
std::string ErrResponse(BGStatusCode _Status, const std::string & _ErrStr);
std::string SuccessResponse();

}
