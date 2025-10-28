#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include "cocos2d.h"
#include "../models/GameModel.h"
#include "../views/GameView.h"
#include "../managers/ResourceManager.h"
#include "../managers/GameModelGenerator.h"
#include "../services/GameRulesService.h"

// 前向声明
class UndoManager;
class UndoModel;
class LevelConfig;

using namespace cocos2d;

/**
 * @brief 游戏控制器类
 * 
 * 职责：
 * - 管理整个游戏流程和状态
 * - 协调模型和视图之间的交互
 * - 处理用户操作和游戏逻辑
 * - 管理子控制器的生命周期
 * 
 * 使用场景：
 * - 游戏初始化、开始、暂停、结束
 * - 处理用户界面交互事件
 * - 协调各个子控制器的协作
 * - 管理游戏状态转换
 */
class GameController
{
public:
    /**
     * @brief 构造函数
     */
    GameController();
    
    /**
     * @brief 析构函数
     */
    ~GameController();
    
    /**
     * @brief 初始化游戏控制器
     * @param levelId 关卡ID
     * @return 是否成功
     */
    bool initGame(int levelId);
    
    /**
     * @brief 开始游戏
     * @return 是否成功
     */
    bool startGame();
    
    /**
     * @brief 暂停游戏
     */
    void pauseGame();
    
    /**
     * @brief 恢复游戏
     */
    void resumeGame();
    
    /**
     * @brief 重启游戏
     */
    void restartGame();
    
    /**
     * @brief 结束游戏
     */
    void endGame();
    
    /**
     * @brief 退出游戏
     */
    void exitGame();
    
    /**
     * @brief 处理卡牌点击事件
     * @param cardId 卡牌ID
     * @return 是否处理成功
     */
    bool handleCardClick(int cardId);
    
    /**
     * @brief 处理卡牌拖拽开始事件
     * @param cardId 卡牌ID
     * @return 是否处理成功
     */
    bool handleCardDragStart(int cardId);
    
    /**
     * @brief 处理卡牌拖拽移动事件
     * @param cardId 卡牌ID
     * @param position 位置
     * @return 是否处理成功
     */
    bool handleCardDragMove(int cardId, const Vec2& position);
    
    /**
     * @brief 处理卡牌匹配事件
     * @param playfieldCardId 主牌区卡牌ID
     * @param handCardId 手牌区卡牌ID
     * @return 是否处理成功
     */
    bool handleCardMatch(int playfieldCardId, int handCardId);
    
    /**
     * @brief 处理手牌区翻牌事件
     * @param stackCardId 备用牌堆卡牌ID
     * @return 是否处理成功
     */
    bool handleStackCardFlip(int stackCardId);
    
    /**
     * @brief 处理撤销操作
     * @return 是否处理成功
     */
    bool handleUndo();
    
    /**
     * @brief 处理提示操作
     * @return 是否处理成功
     */
    bool handleHint();
    
    /**
     * @brief 更新游戏状态
     * @param deltaTime 时间间隔
     */
    void updateGame(float deltaTime);
    
    /**
     * @brief 获取游戏视图
     * @return 游戏视图指针
     */
    GameView* getGameView() const;
    
    /**
     * @brief 获取游戏模型
     * @return 游戏模型指针
     */
    GameModel* getGameModel() const;
    
    /**
     * @brief 设置游戏视图
     * @param gameView 游戏视图
     */
    void setGameView(GameView* gameView);
    
    /**
     * @brief 设置游戏模型
     * @param gameModel 游戏模型
     */
    void setGameModel(GameModel* gameModel);
    
    /**
     * @brief 保存游戏状态
     * @return 是否成功
     */
    bool saveGameState();
    
    /**
     * @brief 加载游戏状态
     * @return 是否成功
     */
    bool loadGameState();
    
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
    
protected:
    /**
     * @brief 初始化子控制器
     * @return 是否成功
     */
    bool initSubControllers();
    
    /**
     * @brief 初始化游戏视图
     * @return 是否成功
     */
    bool initGameView();
    
    /**
     * @brief 初始化游戏模型
     * @return 是否成功
     */
    bool initGameModel();
    
    /**
     * @brief 设置事件回调
     */
    void setupEventCallbacks();
    
    /**
     * @brief 处理游戏状态变化
     */
    void handleGameStateChange();
    
    /**
     * @brief 处理游戏胜利
     */
    void handleGameWin();
    
    /**
     * @brief 处理游戏失败
     */
    void handleGameLose();
    
    /**
     * @brief 处理游戏结束
     */
    void handleGameEnd();
    
    /**
     * @brief 更新游戏UI
     */
    void updateGameUI();
    
    /**
     * @brief 检查游戏规则
     * @return 是否满足规则
     */
    bool checkGameRules();
    
    /**
     * @brief 执行游戏逻辑
     */
    void executeGameLogic();
    
private:
    // 核心组件
    GameModel* _gameModel;                      ///< 游戏数据模型
    GameView* _gameView;                        ///< 游戏视图
    ResourceManager* _resourceManager;          ///< 资源管理器
    UndoManager* _undoManager;                  ///< 撤销管理器
    
    // 游戏状态
    bool _isGameRunning;                       ///< 游戏是否正在运行
    bool _isInitialized;                       ///< 控制器是否已初始化
    
    // 游戏数据
    int _currentLevelId;                       ///< 当前关卡ID
    GameModelGenerator* _levelGenerator; ///< 关卡生成器（Manager）
    LevelConfigLoader* _levelConfigLoader;     ///< 关卡配置加载器
    int _handCardZOrder;                       ///< 手牌区卡牌的 zOrder 计数器（每次递增）
    
    /**
     * @brief 清理资源
     */
    void cleanup();
    
    /**
     * @brief 验证游戏状态
     * @return 是否有效
     */
    bool validateGameState();
    
    /**
     * @brief 处理卡牌移动逻辑
     * @param cardId 卡牌ID
     * @param fromPosition 起始位置
     * @param toPosition 目标位置
     * @return 是否成功
     */
    bool processCardMove(int cardId, const std::string& fromPosition, const std::string& toPosition);
    
    /**
     * @brief 处理卡牌翻转逻辑
     * @param cardId 卡牌ID
     * @return 是否成功
     */
    bool processCardFlip(int cardId);
    
    /**
     * @brief 处理发牌逻辑
     * @param count 发牌数量
     * @param toPosition 目标位置
     * @return 是否成功
     */
    bool processDealCards(int count, const std::string& toPosition);
    
    /**
     * @brief 处理洗牌逻辑
     * @return 是否成功
     */
    bool processShuffle();
    
    /**
     * @brief 处理卡牌选中逻辑
     * @param cardId 卡牌ID
     * @param selected 是否选中
     * @return 是否成功
     */
    bool processCardSelect(int cardId, bool selected);
    
    /**
     * @brief 记录操作到撤销历史
     * @param actionType 操作类型
     * @param cardId 卡牌ID
     * @param fromPosition 起始位置
     * @param toPosition 目标位置
     */
    void recordUndoAction(int actionType, int cardId, const std::string& fromPosition, const std::string& toPosition);
    
    /**
     * @brief 更新游戏分数
     * @param deltaScore 分数变化
     */
    void updateGameScore(int deltaScore);
    
    /**
     * @brief 更新游戏时间
     * @param deltaTime 时间间隔
     */
    void updateGameTime(float deltaTime);
    
    /**
     * @brief 检查游戏结束条件
     * @return 是否结束
     */
    bool checkGameEndCondition();
    
    /**
     * @brief 执行游戏结束逻辑
     */
    void executeGameEndLogic();
    
    /**
     * @brief 检查并翻开下方的卡牌
     * @param cardPos 被移除卡牌的位置
     */
    void checkAndFlipCardsBelow(const Vec2& cardPos);
    
    /**
     * @brief 撤销后更新视图
     * @param undoModel 撤销模型
     */
    void updateViewAfterUndo(UndoModel* undoModel);
    
    /**
     * @brief 翻回自动翻开的卡牌
     * @param cardPos 原卡牌位置
     */
    void flipBackAutoFlippedCard(const Vec2& cardPos);
    
    /**
     * @brief 更新手牌区显示
     */
    void updateHandCardDisplay();
    
    /**
     * @brief 查找可匹配的卡牌
     * @param handCard 手牌
     * @return 可匹配的卡牌ID列表
     */
    std::vector<int> findMatchableCards(CardModel* handCard);
    
    /**
     * @brief 检查是否是列顶部卡牌
     * @param card 卡牌
     * @return 是否是顶部
     */
    bool isTopCardInColumn(CardModel* card);
    
    /**
     * @brief 显示提示动画
     * @param matchableCards 可匹配的卡牌ID列表
     */
    void showHintAnimation(const std::vector<int>& matchableCards);
};

#endif // __GAME_CONTROLLER_H__
