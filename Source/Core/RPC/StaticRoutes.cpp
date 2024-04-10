#include <RPC/StaticRoutes.h>



namespace BG {
namespace EVM {
namespace API {

std::string GetAPIVersion() {
    return VERSION; // VERSION is a pre-defined constant or variable.
}

std::string Echo(std::string _Data) {
    return _Data;
}

} // Close Namespace API
} // Close Namespace EVM
} // Close Namespace BG
