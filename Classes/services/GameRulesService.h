#ifndef __GAME_RULES_SERVICE_H__
#define __GAME_RULES_SERVICE_H__

#include "cocos2d.h"
#include "../models/GameModel.h"
#include "../models/CardModel.h"

using namespace cocos2d;

/**
 * @brief 游戏规则服务类（无状态）
 * 
 * 职责：
 * - 提供游戏规则判断逻辑（无状态）
 * - 执行卡牌匹配、发牌、洗牌等操作
 * - 处理游戏规则验证
 * - 不持有数据，通过参数操作数据或返回数据
 * 
 * 使用场景：
 * - 判断两张卡牌是否可以匹配
 * - 执行卡牌匹配操作
 * - 洗牌和发牌逻辑
 * - 游戏结束条件判断
 */
class GameRulesService
{
public:
    /**
     * @brief 检查两张卡牌是否可以匹配（TriPeaks规则）
     * @param card1 卡牌1
     * @param card2 卡牌2
     * @return 是否可以匹配
     */
    static bool canCardsMatch(CardModel* card1, CardModel* card2);
    
    /**
     * @brief 执行卡牌匹配
     * @param gameModel 游戏模型（通过引用传参修改）
     * @param playfieldCardId 主牌区卡牌ID
     * @param handCardId 手牌区卡牌ID
     * @return 是否成功
     */
    static bool executeCardMatch(GameModel* gameModel, int playfieldCardId, int handCardId);
    
    /**
     * @brief 洗牌
     * @param cards 卡牌列表（通过引用传参修改）
     */
    static void shuffleCards(std::vector<CardModel*>& cards);
    
    /**
     * @brief 发牌
     * @param gameModel 游戏模型（通过引用传参修改）
     * @param count 发牌数量
     * @param toArea 目标区域
     * @return 是否成功
     */
    static bool dealCards(GameModel* gameModel, int count, const std::string& toArea);
    
    /**
     * @brief 从备用牌堆翻一张牌到手牌区
     * @param gameModel 游戏模型（通过引用传参修改）
     * @return 是否成功
     */
    static bool flipStackCardToHand(GameModel* gameModel);
    
    /**
     * @brief 检查游戏是否胜利
     * @param gameModel 游戏模型
     * @return 是否胜利
     */
    static bool checkGameWon(const GameModel* gameModel);
    
    /**
     * @brief 检查游戏是否失败
     * @param gameModel 游戏模型
     * @return 是否失败
     */
    static bool checkGameLost(const GameModel* gameModel);
    
    /**
     * @brief 检查是否有可移动的卡牌
     * @param gameModel 游戏模型
     * @return 是否有可移动的卡牌
     */
    static bool hasAvailableMoves(const GameModel* gameModel);
    
    /**
     * @brief 查找可匹配的卡牌对
     * @param gameModel 游戏模型
     * @return 可匹配的卡牌ID对列表
     */
    static std::vector<std::pair<int, int>> findMatchingPairs(const GameModel* gameModel);
    
    /**
     * @brief 计算手牌分数（如果需要）
     * @param hand 手牌列表
     * @return 分数
     */
    static int calculateHandScore(const std::vector<CardModel*>& hand);
    
    /**
     * @brief 检查是否爆牌（如果需要）
     * @param hand 手牌列表
     * @return 是否爆牌
     */
    static bool isBusted(const std::vector<CardModel*>& hand);
    
private:
    /**
     * @brief 私有构造函数，防止实例化（纯静态工具类）
     */
    GameRulesService() = delete;
    ~GameRulesService() = delete;
};

#endif // __GAME_RULES_SERVICE_H__

