//===========================================================//
// This file is part of the BrainGenix-NES Validation System //
//===========================================================//

/*
    Description: This file provides the validation RPC interfaces.
    Additional Notes: None
    Date Created: 2024-04-22
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <assert.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/RPCManager.h>
#include <RPC/RPCHandlerHelper.h>

#include <BG/Common/Logger/Logger.h>


namespace BG {

/**
 * @brief This class provides the infrastructure to run simulations.
 */
class ValidationRPCInterface {

private:

    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to the instance of the logging system*/

    SafeClient& NESAPIClient_;

public:

    /**
     * @brief Construct a new Interface object
     * Give this a pointer to an initialized configuration object.
     * 
     * @param _Logger Pointer to the logging interface
     * @param _Config 
     * @param _RPCManager
     */
    ValidationRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, API::RPCManager* _RPCManager);

    ~ModelRPCInterface();

    /**
     * @brief Various routes for API
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string SCValidation(std::string _JSONRequest);

};

}; // Close Namespace BG
