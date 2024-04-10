#include <Main.h>


/**
 * @brief The main entry point for the BrainGenix-EVM Evaluation Manager.
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

    // Setup RenderPools
    // BG::EVM::Simulator::VSDA::RenderPool RenderPool(&Logger, false, 5);
    // BG::EVM::Simulator::VisualizerPool VisualizerPool(&Logger, false, 1);

    // Setup Simulator (Adding the routes here)
    // BG::EVM::Simulator::SimulationRPCInterface SimulationRPCInterface(&Logger, &SystemConfiguration, &RenderPool, &VisualizerPool, &APIManager);
    // BG::EVM::Simulator::GeometryRPCInterface   GeometryRPCInterface(&Logger, SimulationRPCInterface.GetSimulationVectorPtr(), &APIManager);
    // BG::EVM::Simulator::ModelRPCInterface      ModelRPCInterface(&Logger, SimulationRPCInterface.GetSimulationVectorPtr(), &APIManager);
    // BG::EVM::Simulator::VisualizerRPCInterface VisualizerRPCInterface(&Logger, SimulationRPCInterface.GetSimulationVectorPtr(), &APIManager);
    // BG::EVM::Simulator::VSDA::VSDARPCInterface VSDARPCInterface(&Logger, &APIManager, SimulationRPCInterface.GetSimulationVectorPtr());

    // Print ASCII BrainGenix Logo To Console
    BG::EVM::Util::LogLogo(&Logger);


    // // Check if we have profiling enabled, if so do that then quit
    // if (SystemConfiguration.ProfilingStatus_ != BG::EVM::Config::PROFILE_NONE) {
    //     BG::EVM::Profiling::Manager(&Logger, &SystemConfiguration, &SimulationRPCInterface, &RenderPool, &VisualizerPool, &APIManager);
    //     return 0;
    // }

    // Block forever while servers are running
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
