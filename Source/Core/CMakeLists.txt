# Configure Sources
set(MAIN_SOURCES 


  ${SRC_DIR}/Core/BGStatusCode.h
  ${SRC_DIR}/Core/Main.cpp
  ${SRC_DIR}/Core/Main.h
  

  ${SRC_DIR}/Core/Config/ConfigurationManager.cpp
  ${SRC_DIR}/Core/Config/ConfigurationManager.h
  ${SRC_DIR}/Core/Config/ConfigFileParser.cpp
  ${SRC_DIR}/Core/Config/ConfigFileParser.h
  ${SRC_DIR}/Core/Config/Config.cpp
  ${SRC_DIR}/Core/Config/Config.h
  ${SRC_DIR}/Core/Config/ConfigDefaults.cpp
  ${SRC_DIR}/Core/Config/ConfigDefaults.h

  ${SRC_DIR}/Core/Metrics/N1Metrics.cpp
  ${SRC_DIR}/Core/Metrics/N1Metrics.h

  ${SRC_DIR}/Core/NESInteraction/NESRequest.cpp
  ${SRC_DIR}/Core/NESInteraction/NESRequest.h
  ${SRC_DIR}/Core/NESInteraction/NESSimLoad.cpp
  ${SRC_DIR}/Core/NESInteraction/NESSimLoad.h

  ${SRC_DIR}/Core/PCRegistration/SimpleRegistration.cpp
  ${SRC_DIR}/Core/PCRegistration/SimpleRegistration.h

  ${SRC_DIR}/Core/RPC/RPCManager.cpp
  ${SRC_DIR}/Core/RPC/RPCManager.h
  ${SRC_DIR}/Core/RPC/APIStatusCode.cpp
  ${SRC_DIR}/Core/RPC/APIStatusCode.h
  ${SRC_DIR}/Core/RPC/RPCHandlerHelper.cpp
  ${SRC_DIR}/Core/RPC/RPCHandlerHelper.h
  ${SRC_DIR}/Core/RPC/StaticRoutes.cpp
  ${SRC_DIR}/Core/RPC/StaticRoutes.h
  ${SRC_DIR}/Core/RPC/ManagerTaskData.cpp
  ${SRC_DIR}/Core/RPC/ManagerTaskData.h
  ${SRC_DIR}/Core/RPC/SafeClient.cpp
  ${SRC_DIR}/Core/RPC/SafeClient.h


  ${SRC_DIR}/Core/Util/JSONHelpers.cpp
  ${SRC_DIR}/Core/Util/JSONHelpers.h
  ${SRC_DIR}/Core/Util/JSONUtils.cpp
  ${SRC_DIR}/Core/Util/JSONUtils.h
  ${SRC_DIR}/Core/Util/LogLogo.cpp
  ${SRC_DIR}/Core/Util/LogLogo.h

  ${SRC_DIR}/Core/Validation/SCValidation.cpp
  ${SRC_DIR}/Core/Validation/SCValidation.h
  ${SRC_DIR}/Core/Validation/ValidationRPCInterface.cpp
  ${SRC_DIR}/Core/Validation/ValidationRPCInterface.h

  ${SRC_DIR}/Core/Vector/Vec3D.cpp
  ${SRC_DIR}/Core/Vector/Vec3D.h

)

# Create Main Executable
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})
set(EXECUTABLE_TYPE "Binary")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    # Boost::program_options
    bg-common-logger
    rpclib::rpc
    ${CMAKE_THREAD_LIBS_INIT}

    VersioningSystem
    CompileTimeStamp
    CompilePlatformInfo
)
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR}/Core)
target_include_directories(${PROJECT_NAME} PRIVATE ${CPP_BASE64_INCLUDE_DIRS})

