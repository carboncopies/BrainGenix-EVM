#include <Validation/ValidationRPCInterface.h>

#include <Main.h>


/**
 * @brief The main entry point for the BrainGenix-EVM Evaluation Metrics.
 * 
 * This function initializes the system components, including configuration, logging, API server,
 * rendering pool, and simulator. It also prints the BrainGenix logo to the console and runs
 * the main loop to keep the servers running.
 * 
 * @param NumArguments The number of command-line arguments.
 * @param ArgumentValues An array containing the command-line arguments.
 * @return int The exit code of the program.
 */
int main(int NumArguments, char** ArgumentValues) {

    // Startup With Config Manager, Will Read Args And Config File, Then Parse Into Config Struct
    BG::EVM::Config::Manager ConfigManager(NumArguments, ArgumentValues);
    BG::EVM::Config::Config& SystemConfiguration = ConfigManager.GetConfig();

    // Setup Logging System
    BG::Common::Logger::LoggingSystem Logger;
    Logger.SetKeepVectorLogs(false);

    // Setup API Server
    BG::EVM::API::RPCManager APIManager(&SystemConfiguration, &Logger);

    // Setup Validation RPC Interface
    BG::ValidationRPCInterface ValidationRPCInterface_(Logger, APIManager);

    // Print ASCII BrainGenix Logo To Console
    BG::EVM::Util::LogLogo(&Logger);

    // Block forever while servers are running
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
