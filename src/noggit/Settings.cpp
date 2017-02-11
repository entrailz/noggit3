// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ConfigFile.h>
#include <noggit/Log.h>
#include <noggit/Settings.h>
#include <noggit/Native.hpp>

#include <boost/filesystem.hpp>
#include <fstream>

Settings::Settings()
{
  // set hardcoded till settings get serialized
  this->random_rotation = false;
  this->random_size = false;
  this->random_tilt = false;
  this->AutoSelectingMode = true;
  this->FarZ = 1024;
  this->_noAntiAliasing = false;
  this->copyModelStats = true;
  this->tabletMode = false;
  this->renderModelsWithBox = false;
  this->importFile = "Import.txt";

  std::string configPath = Native::getConfigPath();
  bool configFileExists = boost::filesystem::exists(configPath);
  if (!configFileExists) {
      if (createConfigFile()) {
          configFileExists = saveToDisk();
      };
  }
                           
  if (configFileExists)
  {
    ConfigFile config(configPath);
    config.readInto(this->gamePath, "Path");
    config.readInto(this->projectPath, "ProjectPath");
    config.readInto(this->FarZ, "FarZ");
    config.readInto(_noAntiAliasing, "noAntiAliasing");
    config.readInto(this->wodSavePath, "wodSavePath");
	config.readInto(this->tabletMode, "TabletMode");
    config.readInto(this->importFile, "ImportFile");
    config.readInto(this->wmvLogFile, "wmvLogFile");

    {
      bool use (false);
      config.readInto(use, "MySQL");
      if (use)
      {
        mysql_connection_info info;
        config.readInto(info.Server, "Server");
        config.readInto(info.User, "User");
        config.readInto(info.Pass, "Pass");
        config.readInto(info.Database, "Database");
        mysql = info;
      }
    }
  }
}

bool Settings::createConfigFile()
{
    std::string configPath = Native::getConfigPath();
    bool configFileExists = boost::filesystem::exists(configPath);
    if (configFileExists) { return false; }
    
    std::ofstream file(configPath);
    
    if (file << "" << std::endl) {
        return true;
    }
    
    return false;
}

bool Settings::saveToDisk()
{
    std::string configPath = Native::getConfigPath();
    
    std::string gamePath(this->gamePath);
    std::string projectPath(this->projectPath);
    
    if (gamePath.length() == 0) {
        gamePath = Native::getGamePath();
    }
    
    if (projectPath.length() == 0) {
        projectPath = gamePath;
    }
    
    ConfigFile config(configPath);
    config.add("Path", gamePath);
    config.add("ProjectPath", projectPath);
    config.add("wodSavePath", this->wodSavePath);
    config.add("ImportFile", this->importFile);
    config.add("wmvLogFile", this->wmvLogFile);
    config.add("FarZ", this->FarZ);
    config.add("randomRotation", this->random_rotation);
    config.add("randomTilt", this->random_tilt);
    config.add("randomSize", this->random_size);
    config.add("autoSelectingMode", this->AutoSelectingMode);
    config.add("copyModelStats", this->copyModelStats);
    config.add("TabletMode", this->tabletMode);
    
    std::ofstream file(configPath);
    
    if (file << config) {
        return true;
    }
    
    return false;
}

const bool& Settings::noAntiAliasing() const
{
  return _noAntiAliasing;
}

Settings* Settings::instance = 0;

Settings* Settings::getInstance()
{
  if (!instance)
    instance = new Settings();
  return instance;
}
