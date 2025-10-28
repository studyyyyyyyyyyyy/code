#include "LevelConfigLoader.h"
#include "json/document.h"
#include "json/filereadstream.h"
#include <fstream>

LevelConfigLoader::LevelConfigLoader()
: _configPath("configs/levels/")
{
}

LevelConfigLoader::~LevelConfigLoader()
{
    clearCache();
}

LevelConfig* LevelConfigLoader::loadLevelConfig(int levelId)
{
    // 先检查缓存
    auto it = _configCache.find(levelId);
    if (it != _configCache.end())
    {
        return it->second;
    }
    
    // 从文件加载
    LevelConfig* config = loadFromFile(levelId);
    if (config)
    {
        _configCache[levelId] = config;
    }
    
    return config;
}

std::vector<LevelConfig*> LevelConfigLoader::loadAllLevelConfigs()
{
    std::vector<LevelConfig*> configs;
    
    // 遍历配置文件目录
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(_configPath);
    if (fullPath.empty())
    {
        return configs;
    }
    
    // 这里简化实现，实际项目中需要遍历目录
    // 假设关卡ID从1开始，最多100关
    for (int i = 1; i <= 100; ++i)
    {
        LevelConfig* config = loadLevelConfig(i);
        if (config)
        {
            configs.push_back(config);
        }
        else
        {
            break;
        }
    }
    
    return configs;
}

bool LevelConfigLoader::preloadLevelConfigs(const std::vector<int>& levelIds)
{
    bool allSuccess = true;
    
    for (int levelId : levelIds)
    {
        LevelConfig* config = loadLevelConfig(levelId);
        if (!config)
        {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

LevelConfig* LevelConfigLoader::getCachedLevelConfig(int levelId)
{
    auto it = _configCache.find(levelId);
    return (it != _configCache.end()) ? it->second : nullptr;
}

void LevelConfigLoader::clearCache()
{
    for (auto& pair : _configCache)
    {
        delete pair.second;
    }
    _configCache.clear();
}

void LevelConfigLoader::setConfigPath(const std::string& configPath)
{
    _configPath = configPath;
}

std::string LevelConfigLoader::getConfigPath() const
{
    return _configPath;
}

bool LevelConfigLoader::isConfigFileExists(int levelId)
{
    std::string filePath = getConfigFilePath(levelId);
    return FileUtils::getInstance()->isFileExist(filePath);
}

LevelConfig* LevelConfigLoader::loadFromFile(int levelId)
{
    std::string filePath = getConfigFilePath(levelId);
    
    if (!FileUtils::getInstance()->isFileExist(filePath))
    {
        // 文件不存在，创建默认配置
        return createDefaultLevelConfig(levelId);
    }
    
    // 读取文件内容
    std::string content = FileUtils::getInstance()->getStringFromFile(filePath);
    if (content.empty()) return nullptr;
    
    // 解析JSON
    LevelConfig* config = new LevelConfig();
    if (config->deserialize(content))
    {
        return config;
    }
    else
    {
        delete config;
        return nullptr;
    }
}

std::string LevelConfigLoader::getConfigFilePath(int levelId)
{
    return _configPath + "level_" + std::to_string(levelId) + ".json";
}

LevelConfig* LevelConfigLoader::createDefaultLevelConfig(int levelId)
{
    LevelConfig* config = new LevelConfig();
    config->_levelId = levelId;
    config->_levelName = "关卡 " + std::to_string(levelId);
    config->_levelDescription = "这是一个默认关卡配置";
    config->_difficulty = 1;
    config->_targetScore = 1000 + levelId * 100;
    config->_timeLimit = 0;
    config->_initialCardCount = 52;
    config->_maxUndoCount = 10;
    config->_allowHint = true;
    config->_allowUndo = true;
    config->_star1Score = 500 + levelId * 50;
    config->_star2Score = 1000 + levelId * 100;
    config->_star3Score = 1500 + levelId * 150;
    config->_requiredLevelId = (levelId > 1) ? levelId - 1 : 0;
    config->_requiredStars = 0;
    
    return config;
}
