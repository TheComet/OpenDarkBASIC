#include "odb-cli/SDK.hpp"
#include "odb-sdk/FileSystem.hpp"
#include "odb-sdk/Log.hpp"

using namespace odb;

static std::string programName_;
static std::filesystem::path sdkRootDir_;
static std::vector<std::filesystem::path> pluginDirs_;
static SDKType sdkType_ = SDKType::ODB;

// ----------------------------------------------------------------------------
bool setSDKRootDir(const std::vector<std::string>& args)
{
    if (sdkRootDir_ != "")
    {
        Log::sdk(Log::ERROR, "SDK root directory is already set to `%s'\n", sdkRootDir_.c_str());
        return false;
    }

    sdkRootDir_ = args[0];
    Log::sdk(Log::INFO, "Using SDK root directory: `%s'\n", sdkRootDir_.c_str());

    return true;
}

// ----------------------------------------------------------------------------
bool setSDKType(const std::vector<std::string>& args)
{
    if (args[0] == "dbpro") {
      sdkType_ = SDKType::DarkBASIC;
    } else if (args[0] == "odb-sdk") {
      sdkType_ = SDKType::ODB;
    } else {
      Log::sdk(Log::ERROR, "Unknown SDK type `%s'\n", args[0].c_str());
      return false;
    }

    Log::sdk(Log::INFO, "Setting SDK type to `%s'\n", args[0].c_str());
    return true;
}

// ----------------------------------------------------------------------------
bool setAdditionalPluginsDir(const std::vector<std::string>& args)
{
    for (const auto& dirOrFile : args)
        pluginDirs_.push_back(dirOrFile);

    return true;
}

// ----------------------------------------------------------------------------
bool printSDKRootDir(const std::vector<std::string>& args)
{
    Log::sdk(Log::NOTICE, "SDK root directory: `%s'\n", sdkRootDir_.c_str());
    return true;
}

// ----------------------------------------------------------------------------
bool initSDK(const std::vector<std::string>& args)
{
    // Default SDK root dir
    if (sdkRootDir_ == "")
    {
        switch (sdkType_)
        {
            case SDKType::ODB : {
                // Should in the same directory as the odbc executable
                sdkRootDir_ = FileSystem::getPathToSelf().replace_filename("odb-sdk");
            } break;

            case SDKType::DarkBASIC: {
                Log::sdk(Log::ERROR, "There is no default path configured for the DarkBASIC Pro SDK root directory. Please specify it with --sdkroot or set the SDK type to `odb` with --sdktype\n");
                return false;
            }
        }
    }

    Log::sdk(Log::INFO, "Initialized SDK\n");
    return true;
}

// ----------------------------------------------------------------------------
SDKType getSDKType()
{
    return sdkType_;
}

// ----------------------------------------------------------------------------
const std::filesystem::path& getSDKRootDir()
{
    return sdkRootDir_;
}

// ----------------------------------------------------------------------------
const std::vector<std::filesystem::path>& getAdditionalPluginDirs()
{
    return pluginDirs_;
}
