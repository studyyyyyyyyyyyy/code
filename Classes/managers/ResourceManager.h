#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "cocos2d.h"
#include "../configs/models/CardResConfig.h"

using namespace cocos2d;

/**
 * @brief 资源管理器类
 * 
 * 职责：
 * - 管理游戏中的所有资源文件
 * - 提供资源加载和缓存功能
 * - 处理资源路径和配置
 * - 支持资源预加载和释放
 * 
 * 使用场景：
 * - 游戏启动时预加载资源
 * - 运行时动态加载资源
 * - 资源缓存管理
 * - 资源路径解析
 */
class ResourceManager : public Ref
{
public:
    /**
     * @brief 创建资源管理器实例
     * @return 资源管理器实例
     */
    static ResourceManager* create();
    
    /**
     * @brief 初始化资源管理器
     * @return 是否成功
     */
    bool init();
    
    /**
     * @brief 预加载所有资源
     * @return 是否成功
     */
    bool preloadAllResources();
    
    /**
     * @brief 预加载卡牌资源
     * @return 是否成功
     */
    bool preloadCardResources();
    
    /**
     * @brief 预加载字体资源
     * @return 是否成功
     */
    bool preloadFontResources();
    
    /**
     * @brief 预加载音效资源
     * @return 是否成功
     */
    bool preloadAudioResources();
    
    /**
     * @brief 获取卡牌资源配置
     * @return 卡牌资源配置
     */
    CardResConfig* getCardResConfig();
    
    /**
     * @brief 加载卡牌资源配置
     * @param configPath 配置文件路径
     * @return 是否成功
     */
    bool loadCardResConfig(const std::string& configPath);
    
    /**
     * @brief 获取卡牌背面纹理
     * @return 卡牌背面纹理
     */
    Texture2D* getCardBackTexture();
    
    /**
     * @brief 获取卡牌正面纹理
     * @param suit 花色
     * @param rank 点数
     * @return 卡牌正面纹理
     */
    Texture2D* getCardFrontTexture(int suit, int rank);
    
    /**
     * @brief 获取花色纹理
     * @param suit 花色
     * @return 花色纹理
     */
    Texture2D* getSuitTexture(int suit);
    
    /**
     * @brief 获取点数纹理
     * @param rank 点数
     * @return 点数纹理
     */
    Texture2D* getRankTexture(int rank);
    
    /**
     * @brief 获取字体
     * @param fontPath 字体路径
     * @return 字体
     */
    Font* getFont(const std::string& fontPath);
    
    /**
     * @brief 检查资源是否存在
     * @param resourcePath 资源路径
     * @return 是否存在
     */
    bool isResourceExists(const std::string& resourcePath);
    
    /**
     * @brief 获取资源完整路径
     * @param resourcePath 资源路径
     * @return 完整路径
     */
    std::string getResourcePath(const std::string& resourcePath);
    
    /**
     * @brief 清理资源缓存
     */
    void clearResourceCache();
    
    /**
     * @brief 获取资源使用统计
     * @return 资源统计信息
     */
    std::string getResourceStats();
    
protected:
    /**
     * @brief 构造函数
     */
    ResourceManager();
    
    /**
     * @brief 析构函数
     */
    ~ResourceManager();
    
    /**
     * @brief 加载纹理资源
     * @param texturePath 纹理路径
     * @return 纹理指针
     */
    Texture2D* loadTexture(const std::string& texturePath);
    
    /**
     * @brief 加载字体资源
     * @param fontPath 字体路径
     * @return 字体指针
     */
    Font* loadFont(const std::string& fontPath);
    
    /**
     * @brief 创建卡牌纹理
     * @param suit 花色
     * @param rank 点数
     * @return 卡牌纹理
     */
    Texture2D* createCardTexture(int suit, int rank);
    
    /**
     * @brief 创建卡牌背面纹理
     * @return 卡牌背面纹理
     */
    Texture2D* createCardBackTexture();
    
private:
    CardResConfig* _cardResConfig;               ///< 卡牌资源配置
    std::map<std::string, Texture2D*> _textureCache;  ///< 纹理缓存
    std::map<std::string, Font*> _fontCache;           ///< 字体缓存
    
    bool _isInitialized;                         ///< 是否已初始化
    
    /**
     * @brief 清理资源
     */
    void cleanup();
    
    /**
     * @brief 验证资源路径
     * @param resourcePath 资源路径
     * @return 是否有效
     */
    bool validateResourcePath(const std::string& resourcePath);
    
    /**
     * @brief 获取缓存键
     * @param resourcePath 资源路径
     * @return 缓存键
     */
    std::string getCacheKey(const std::string& resourcePath);
};

#endif // __RESOURCE_MANAGER_H__
