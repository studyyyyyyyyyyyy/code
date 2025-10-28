#ifndef __LEVEL_CONFIG_H__
#define __LEVEL_CONFIG_H__

#include "cocos2d.h"

using namespace cocos2d;

/**
 * @brief 关卡配置类
 * 
 * 职责：
 * - 存储关卡相关的静态配置数据
 * - 包含关卡ID、名称、难度、目标分数等信息
 * - 支持序列化和反序列化
 * 
 * 使用场景：
 * - 关卡选择界面显示关卡信息
 * - 游戏初始化时加载关卡配置
 * - 存档系统中保存关卡进度
 */
class LevelConfig
{
public:
    /**
     * @brief 构造函数
     */
    LevelConfig();
    
    /**
     * @brief 析构函数
     */
    ~LevelConfig();
    
    // 关卡基本信息
    int _levelId;                      ///< 关卡ID
    std::string _levelName;            ///< 关卡名称
    std::string _levelDescription;     ///< 关卡描述
    int _difficulty;                   ///< 难度等级 (1-5)
    int _targetScore;                  ///< 目标分数
    int _timeLimit;                    ///< 时间限制(秒，0表示无限制)
    
    // 游戏规则配置
    int _initialCardCount;             ///< 初始卡牌数量
    int _maxUndoCount;                 ///< 最大撤销次数
    bool _allowHint;                   ///< 是否允许提示
    bool _allowUndo;                   ///< 是否允许撤销
    
    // 奖励配置
    int _star1Score;                   ///< 一星分数
    int _star2Score;                   ///< 二星分数
    int _star3Score;                   ///< 三星分数
    
    // 解锁条件
    int _requiredLevelId;              ///< 前置关卡ID (0表示无前置要求)
    int _requiredStars;                 ///< 前置关卡需要的星星数
    
    // 游戏区域配置
    struct PlayfieldCard {
        int cardFace;           ///< 卡牌正面（0-13）
        int cardSuit;           ///< 卡牌花色（0-3）
        Vec2 position;          ///< 卡牌位置
        bool isFaceUp;          ///< 是否翻开
        
        PlayfieldCard() : cardFace(0), cardSuit(0), position(Vec2::ZERO), isFaceUp(true) {}
    };
    
    struct StackCard {
        int cardFace;           ///< 卡牌正面（0-13）
        int cardSuit;           ///< 卡牌花色（0-3）
        Vec2 position;          ///< 卡牌位置
    };
    
    std::vector<PlayfieldCard> _playfieldCards;  ///< 主牌区卡牌
    std::vector<StackCard> _stackCards;          ///< 备用牌堆卡牌
    
    /**
     * @brief 序列化为JSON字符串
     * @return JSON格式的字符串
     */
    std::string serialize() const;
    
    /**
     * @brief 从JSON字符串反序列化
     * @param jsonStr JSON格式的字符串
     * @return 是否成功
     */
    bool deserialize(const std::string& jsonStr);
    
    /**
     * @brief 验证配置数据的有效性
     * @return 是否有效
     */
    bool isValid() const;
    
    /**
     * @brief 获取关卡显示名称
     * @return 格式化的关卡名称
     */
    std::string getDisplayName() const;
    
    /**
     * @brief 检查关卡是否已解锁
     * @param completedLevels 已完成的关卡ID列表
     * @return 是否已解锁
     */
    bool isUnlocked(const std::vector<int>& completedLevels) const;
};

#endif // __LEVEL_CONFIG_H__
