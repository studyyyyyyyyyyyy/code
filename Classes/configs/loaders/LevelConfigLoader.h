#ifndef __LEVEL_CONFIG_LOADER_H__
#define __LEVEL_CONFIG_LOADER_H__

#include "cocos2d.h"
#include "../models/LevelConfig.h"

using namespace cocos2d;

/**
 * @brief 关卡配置加载器
 * 
 * 职责：
 * - 从文件系统加载关卡配置数据
 * - 提供关卡配置的缓存管理
 * - 支持配置文件的验证和错误处理
 * 
 * 使用场景：
 * - 游戏启动时预加载所有关卡配置
 * - 关卡选择界面动态加载配置
 * - 配置更新时重新加载
 */
class LevelConfigLoader
{
public:
    /**
     * @brief 构造函数
     */
    LevelConfigLoader();
    
    /**
     * @brief 析构函数
     */
    ~LevelConfigLoader();
    
    /**
     * @brief 加载指定关卡的配置
     * @param levelId 关卡ID
     * @return 关卡配置指针，失败返回nullptr
     */
    LevelConfig* loadLevelConfig(int levelId);
    
    /**
     * @brief 加载所有关卡配置
     * @return 关卡配置列表
     */
    std::vector<LevelConfig*> loadAllLevelConfigs();
    
    /**
     * @brief 预加载关卡配置
     * @param levelIds 要预加载的关卡ID列表
     * @return 是否成功
     */
    bool preloadLevelConfigs(const std::vector<int>& levelIds);
    
    /**
     * @brief 获取缓存的关卡配置
     * @param levelId 关卡ID
     * @return 关卡配置指针，未缓存返回nullptr
     */
    LevelConfig* getCachedLevelConfig(int levelId);
    
    /**
     * @brief 清除缓存
     */
    void clearCache();
    
    /**
     * @brief 设置配置文件路径
     * @param configPath 配置文件路径
     */
    void setConfigPath(const std::string& configPath);
    
    /**
     * @brief 获取配置文件路径
     * @return 配置文件路径
     */
    std::string getConfigPath() const;
    
    /**
     * @brief 验证配置文件是否存在
     * @param levelId 关卡ID
     * @return 是否存在
     */
    bool isConfigFileExists(int levelId);
    
private:
    std::string _configPath;                    ///< 配置文件路径
    std::map<int, LevelConfig*> _configCache;   ///< 配置缓存
    
    /**
     * @brief 从文件加载配置
     * @param levelId 关卡ID
     * @return 关卡配置指针
     */
    LevelConfig* loadFromFile(int levelId);
    
    /**
     * @brief 获取配置文件完整路径
     * @param levelId 关卡ID
     * @return 完整文件路径
     */
    std::string getConfigFilePath(int levelId);
    
    /**
     * @brief 创建默认关卡配置
     * @param levelId 关卡ID
     * @return 默认关卡配置
     */
    LevelConfig* createDefaultLevelConfig(int levelId);
};

#endif // __LEVEL_CONFIG_LOADER_H__
