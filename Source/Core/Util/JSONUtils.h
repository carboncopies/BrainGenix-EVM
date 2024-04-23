/**
 * This set of JSON utility functions is very similar to the set in JSONHelpers
 * and derived from the HandlerData class in NES.
 * Added this set, to simplify access through fewer namespaces and to ensure
 * more tests to prevent serious errors or crashes of the server. (R.K.)
 */

#include <BGStatusCode.h>


namespace BG {

BGStatusCode FindPar(const nlohmann::json& _JSON, const std::string& _ParName, nlohmann::json::iterator& _Iterator);
BGStatusCode GetParBool(const nlohmann::json& _JSON, const std::string& ParName, bool& Value);
BGStatusCode GetParInt(const nlohmann::json& _JSON, const std::string& ParName, int& Value);
BGStatusCode GetParFloat(const nlohmann::json& _JSON, const std::string& ParName, float& Value);
BGStatusCode GetParString(const nlohmann::json& _JSON, const std::string& ParName, std::string& Value);

std::string Response(const nlohmann::json& ResponseJSON);
std::string ErrResponse(int _Status);
std::string ErrResponse(int _Status, const std::string & _ErrStr)
std::string ErrResponse(BGStatusCode _Status);
std::string ErrResponse(BGStatusCode _Status, const std::string & _ErrStr);
std::string SuccessResponse();

}
