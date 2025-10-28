#ifndef __GAME_MODEL_H__
#define __GAME_MODEL_H__

#include "cocos2d.h"
#include "CardModel.h"

using namespace cocos2d;

/**
 * @brief 游戏数据模型
 * 
 * 职责：
 * - 存储游戏的整体状态和数据
 * - 管理卡牌集合、玩家状态、游戏进度等
 * - 提供游戏数据的序列化和反序列化
 * 
 * 使用场景：
 * - 游戏逻辑中表示当前游戏状态
 * - 存档系统中保存游戏进度
 * - 网络同步中传输游戏数据
 */
class GameModel
{
public:
    /**
     * @brief 游戏状态枚举
     */
    enum GameState {
        GAME_STATE_NONE = 0,        ///< 未开始
        GAME_STATE_PLAYING = 1,      ///< 游戏中
        GAME_STATE_PAUSED = 2,       ///< 暂停
        GAME_STATE_FINISHED = 3,     ///< 结束
        GAME_STATE_GAME_OVER = 4     ///< 游戏结束
    };
    
    /**
     * @brief 构造函数
     */
    GameModel();
    
    /**
     * @brief 析构函数
     */
    ~GameModel();
    
    // 游戏基本信息
    int _levelId;                    ///< 关卡ID
    int _gameId;                     ///< 游戏ID
    GameState _gameState;            ///< 游戏状态
    float _gameTime;                 ///< 游戏时间
    int _score;                      ///< 当前分数
    int _targetScore;                ///< 目标分数
    
    // 卡牌管理
    std::vector<CardModel*> _allCards;           ///< 所有卡牌
    std::vector<CardModel*> _playfieldCards;     ///< 主牌区卡牌
    std::vector<CardModel*> _stackCards;         ///< 备用牌堆卡牌
    std::vector<CardModel*> _handCards;          ///< 手牌区卡牌（顶部牌）
    
    // 游戏规则
    int _maxUndoCount;               ///< 最大撤销次数
    int _currentUndoCount;           ///< 当前撤销次数
    bool _allowHint;                 ///< 是否允许提示
    bool _allowUndo;                 ///< 是否允许撤销
    int _timeLimit;                  ///< 时间限制
    
    // 玩家状态
    bool _playerTurn;                ///< 是否玩家回合
    int _playerScore;                ///< 玩家分数
    int _dealerScore;                ///< 庄家分数
    bool _playerBusted;              ///< 玩家是否爆牌
    bool _dealerBusted;              ///< 庄家是否爆牌
    
    // 游戏进度
    int _movesCount;                 ///< 移动次数
    int _hintsUsed;                  ///< 使用提示次数
    std::vector<int> _completedActions;  ///< 已完成的操作列表
    
    /**
     * @brief 初始化游戏
     * @param levelId 关卡ID
     * @return 是否成功
     */
    bool initGame(int levelId);
    
    /**
     * @brief 重置游戏
     */
    void resetGame();
    
    /**
     * @brief 开始游戏
     */
    void startGame();
    
    /**
     * @brief 暂停游戏
     */
    void pauseGame();
    
    /**
     * @brief 恢复游戏
     */
    void resumeGame();
    
    /**
     * @brief 结束游戏
     */
    void endGame();
    
    /**
     * @brief 检查游戏是否结束
     * @return 是否结束
     */
    bool isGameFinished() const;
    
    /**
     * @brief 检查游戏是否胜利
     * @return 是否胜利
     */
    bool isGameWon() const;
    
    /**
     * @brief 检查游戏是否失败
     * @return 是否失败
     */
    bool isGameLost() const;
    
    /**
     * @brief 获取指定ID的卡牌
     * @param cardId 卡牌ID
     * @return 卡牌指针，未找到返回nullptr
     */
    CardModel* getCardById(int cardId);
    
    /**
     * @brief 移动卡牌到指定区域
     * @param cardId 卡牌ID
     * @param targetArea 目标区域
     * @param targetIndex 目标索引
     * @return 是否成功
     */
    bool moveCardToArea(int cardId, const std::string& targetArea, int targetIndex = -1);
    
    /**
     * @brief 从指定区域移除卡牌
     * @param cardId 卡牌ID
     * @param area 区域名称
     * @return 是否成功
     */
    bool removeCardFromArea(int cardId, const std::string& area);
    
    /**
     * @brief 添加卡牌到指定区域
     * @param card 卡牌
     * @param area 区域名称
     * @param index 插入索引
     * @return 是否成功
     */
    bool addCardToArea(CardModel* card, const std::string& area, int index = -1);
    
    /**
     * @brief 获取手牌区顶部卡牌
     * @return 顶部卡牌指针
     */
    CardModel* getTopHandCard();
    
    /**
     * @brief 更新分数（数据修改，非业务逻辑）
     * @param deltaScore 分数变化
     */
    void updateScore(int deltaScore);
    
    /**
     * @brief 增加移动次数（数据修改，非业务逻辑）
     */
    void incrementMoves();
    
    /**
     * @brief 增加提示使用次数（数据修改，非业务逻辑）
     */
    void incrementHints();
    
    /**
     * @brief 检查是否可以撤销
     * @return 是否可以撤销
     */
    bool canUndo() const;
    
    /**
     * @brief 检查是否可以提示
     * @return 是否可以提示
     */
    bool canHint() const;
    
    /**
     * @brief 计算获得的星级（基于分数）
     * @return 星级数量（0-3）
     */
    int calculateStars() const;
    
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
     * @brief 验证数据有效性
     * @return 是否有效
     */
    bool isValid() const;
    
    /**
     * @brief 清理资源
     */
    void cleanup();
    
private:
    // 成员变量
    std::vector<CardModel*> _playerHand;    ///< 玩家手牌
    std::vector<CardModel*> _dealerHand;    ///< 庄家手牌
    std::vector<CardModel*> _deck;          ///< 牌堆
    std::vector<CardModel*> _discardPile;   ///< 弃牌堆
};

#endif // __GAME_MODEL_H__
