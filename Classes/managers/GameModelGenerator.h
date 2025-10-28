#ifndef __GAME_MODEL_GENERATOR_H__
#define __GAME_MODEL_GENERATOR_H__

#include "cocos2d.h"
#include "../models/GameModel.h"
#include "../configs/models/LevelConfig.h"
#include "../configs/loaders/LevelConfigLoader.h"

using namespace cocos2d;

/**
 * @brief 游戏模型生成器管理器类（Manager）
 * 
 * 职责：
 * - 将静态配置（LevelConfig）转换为动态运行时数据（GameModel）
 * - 处理卡牌随机生成策略
 * - 管理游戏模型初始化过程
 * - 持有随机种子等状态数据
 * 
 * 架构定位：
 * - 作为 Manager 层，可以持有状态数据（如 _randomSeed）
 * - 作为 GameController 的成员变量使用
 * - 不实现为单例模式
 * - 不反向依赖 Controller
 * 
 * 使用场景：
 * - 游戏初始化时生成游戏模型
 * - 关卡切换时重新生成游戏模型
 * - 游戏重启时重新初始化模型
 */
class GameModelGenerator
{
public:
    /**
     * @brief 构造函数
     */
    GameModelGenerator();
    
    /**
     * @brief 析构函数
     */
    ~GameModelGenerator();
    
    /**
     * @brief 生成游戏模型（通过参数传入 loader，不持有引用）
     * @param levelId 关卡ID
     * @param loader 关卡配置加载器
     * @return 游戏模型指针
     */
    GameModel* generateGameModel(int levelId, LevelConfigLoader* loader);
    
    /**
     * @brief 生成游戏模型
     * @param levelConfig 关卡配置
     * @return 游戏模型指针
     */
    GameModel* generateGameModel(LevelConfig* levelConfig);
    
    /**
     * @brief 设置随机种子
     * @param seed 随机种子
     */
    void setRandomSeed(unsigned int seed);
    
    /**
     * @brief 获取随机种子
     * @return 随机种子
     */
    unsigned int getRandomSeed() const;
    
    /**
     * @brief 重置随机种子
     */
    void resetRandomSeed();
    
    /**
     * @brief 验证生成器状态
     * @return 是否有效
     */
    bool isValid() const;
    
private:
    unsigned int _randomSeed;              ///< 随机种子（Manager 可以持有状态数据）
    
    /**
     * @brief 初始化游戏模型
     * @param gameModel 游戏模型
     * @param levelConfig 关卡配置
     * @return 是否成功
     */
    bool initializeGameModel(GameModel* gameModel, LevelConfig* levelConfig);
    
    /**
     * @brief 创建卡牌集合
     * @param gameModel 游戏模型
     * @param levelConfig 关卡配置
     * @return 是否成功
     */
    bool createCardCollection(GameModel* gameModel, LevelConfig* levelConfig);
    
    /**
     * @brief 设置游戏规则
     * @param gameModel 游戏模型
     * @param levelConfig 关卡配置
     * @return 是否成功
     */
    bool setupGameRules(GameModel* gameModel, LevelConfig* levelConfig);
    
    /**
     * @brief 初始化游戏状态
     * @param gameModel 游戏模型
     * @param levelConfig 关卡配置
     * @return 是否成功
     */
    bool initializeGameState(GameModel* gameModel, LevelConfig* levelConfig);
    
    /**
     * @brief 生成随机卡牌
     * @param count 卡牌数量
     * @return 卡牌模型列表
     */
    std::vector<CardModel*> generateRandomCards(int count);
    
    /**
     * @brief 洗牌算法
     * @param cards 卡牌列表
     */
    void shuffleCards(std::vector<CardModel*>& cards);
    
    /**
     * @brief 验证关卡配置
     * @param levelConfig 关卡配置
     * @return 是否有效
     */
    bool validateLevelConfig(LevelConfig* levelConfig);
    
    /**
     * @brief 清理资源
     */
    void cleanup();
};

#endif // __GAME_MODEL_GENERATOR_H__

