#ifndef __GAME_VIEW_H__
#define __GAME_VIEW_H__

#include "cocos2d.h"
#include "../models/GameModel.h"
#include "CardView.h"

using namespace cocos2d;

/**
 * @brief 游戏视图类
 * 
 * 职责：
 * - 负责游戏界面的整体显示和布局
 * - 管理所有卡牌视图的显示和交互
 * - 处理游戏UI元素（按钮、标签、进度条等）
 * - 响应游戏状态变化并更新UI
 * 
 * 使用场景：
 * - 游戏主界面显示
 * - 卡牌布局和动画管理
 * - 游戏状态UI更新
 * - 用户交互事件处理
 */
class GameView : public Layer
{
public:
    /**
     * @brief 创建游戏视图
     * @param gameModel 游戏数据模型（const，View层只读）
     * @param resourceManager 资源管理器（用于创建卡牌视图）
     * @return 游戏视图指针
     */
    static GameView* create(const GameModel* gameModel, class ResourceManager* resourceManager = nullptr);
    
    /**
     * @brief 初始化
     * @param gameModel 游戏数据模型（const，View层只读）
     * @param resourceManager 资源管理器（用于创建卡牌视图）
     * @return 是否成功
     */
    virtual bool init(const GameModel* gameModel, class ResourceManager* resourceManager = nullptr);
    
    /**
     * @brief 析构函数
     */
    virtual ~GameView();
    
    // 回调函数类型定义
    typedef std::function<void()> GameStartCallback;
    typedef std::function<void()> GamePauseCallback;
    typedef std::function<void()> GameResumeCallback;
    typedef std::function<void()> GameRestartCallback;
    typedef std::function<void()> GameExitCallback;
    typedef std::function<void()> UndoCallback;
    typedef std::function<void()> HintCallback;
    typedef std::function<void(int cardId)> CardClickCallback;
    
    // 回调函数
    GameStartCallback _onGameStartCallback;          ///< 游戏开始回调
    GamePauseCallback _onGamePauseCallback;          ///< 游戏暂停回调
    GameResumeCallback _onGameResumeCallback;        ///< 游戏恢复回调
    GameRestartCallback _onGameRestartCallback;      ///< 游戏重启回调
    GameExitCallback _onGameExitCallback;            ///< 游戏退出回调
    UndoCallback _onUndoCallback;                    ///< 撤销回调
    HintCallback _onHintCallback;                     ///< 提示回调
    CardClickCallback _onCardClickCallback;           ///< 卡牌点击回调
    
    /**
     * @brief 设置游戏开始回调
     * @param callback 回调函数
     */
    void setGameStartCallback(GameStartCallback callback);
    
    /**
     * @brief 设置游戏暂停回调
     * @param callback 回调函数
     */
    void setGamePauseCallback(GamePauseCallback callback);
    
    /**
     * @brief 设置游戏恢复回调
     * @param callback 回调函数
     */
    void setGameResumeCallback(GameResumeCallback callback);
    
    /**
     * @brief 设置游戏重启回调
     * @param callback 回调函数
     */
    void setGameRestartCallback(GameRestartCallback callback);
    
    /**
     * @brief 设置游戏退出回调
     * @param callback 回调函数
     */
    void setGameExitCallback(GameExitCallback callback);
    
    /**
     * @brief 设置撤销回调
     * @param callback 回调函数
     */
    void setUndoCallback(UndoCallback callback);
    
    /**
     * @brief 设置提示回调
     * @param callback 回调函数
     */
    void setHintCallback(HintCallback callback);
    
    /**
     * @brief 设置卡牌点击回调
     * @param callback 回调函数
     */
    void setCardClickCallback(CardClickCallback callback);
    
    /**
     * @brief 更新游戏显示
     */
    void updateGameDisplay();
    
    /**
     * @brief 更新分数显示
     */
    void updateScoreDisplay();
    
    /**
     * @brief 更新时间显示
     */
    void updateTimeDisplay();
    
    /**
     * @brief 更新按钮状态
     */
    void updateButtonStates();
    
    /**
     * @brief 播放匹配动画
     * @param cardId 卡牌ID
     */
    void playMatchAnimation(int cardId);
    
    /**
     * @brief 播放游戏胜利动画
     */
    void playWinAnimation();
    
    /**
     * @brief 播放游戏失败动画
     */
    void playLoseAnimation();
    
    /**
     * @brief 显示提示效果
     * @param cardIds 提示的卡牌ID列表
     */
    void showHintEffect(const std::vector<int>& cardIds);
    
    /**
     * @brief 隐藏提示效果
     */
    void hideHintEffect();
    
    /**
     * @brief 显示备用堆提示效果（让备用堆顶部卡牌闪烁）
     * @param cardId 备用堆顶部卡牌ID
     */
    void showStackHintEffect(int cardId);
    
    /**
     * @brief 显示游戏结果
     * @param isWin 是否胜利
     * @param score 最终分数
     */
    void showGameResult(bool isWin, int score);
    
    /**
     * @brief 显示游戏结果（带星级）
     * @param isWin 是否胜利
     * @param score 最终分数
     * @param stars 星级数量（0-3）
     */
    void showGameResultWithStars(bool isWin, int score, int stars);
    
    /**
     * @brief 隐藏游戏结果
     */
    void hideGameResult();
    
    /**
     * @brief 设置游戏是否可交互
     * @param interactive 是否可交互
     */
    void setGameInteractive(bool interactive);
    
    /**
     * @brief 获取卡牌视图
     * @param cardId 卡牌ID
     * @return 卡牌视图指针
     */
    CardView* getCardView(int cardId);
    
    /**
     * @brief 添加卡牌视图
     * @param cardView 卡牌视图
     */
    void addCardView(CardView* cardView);
    
    /**
     * @brief 移除卡牌视图
     * @param cardId 卡牌ID
     */
    void removeCardView(int cardId);
    
    /**
     * @brief 清理所有卡牌视图
     */
    void clearAllCardViews();
    
    /**
     * @brief 播放卡牌移动动画
     * @param cardId 卡牌ID
     * @param targetPosition 目标位置
     * @param duration 动画持续时间
     */
    void playCardMoveAnimation(int cardId, const Vec2& targetPosition, float duration = 0.3f);
    
    /**
     * @brief 播放卡牌匹配动画
     * @param cardId1 卡牌1 ID
     * @param cardId2 卡牌2 ID
     */
    void playCardMatchAnimation(int cardId1, int cardId2);
    
protected:
    /**
     * @brief 创建UI元素
     */
    void createUIElements();
    
    /**
     * @brief 创建按钮
     */
    void createButtons();
    
    /**
     * @brief 创建标签
     */
    void createLabels();
    
    /**
     * @brief 创建卡牌区域
     */
    void createCardAreas();
    
    /**
     * @brief 创建游戏结果面板
     */
    void createGameResultPanel();
    
    /**
     * @brief 按钮点击事件处理
     */
    void onStartButtonClicked(Ref* sender);
    void onPauseButtonClicked(Ref* sender);
    void onResumeButtonClicked(Ref* sender);
    void onRestartButtonClicked(Ref* sender);
    void onExitButtonClicked(Ref* sender);
    void onUndoButtonClicked(Ref* sender);
    void onHintButtonClicked(Ref* sender);
    
private:
    const GameModel* _gameModel;    ///< 游戏数据模型（只读）
    class ResourceManager* _resourceManager;  ///< 资源管理器（用于创建卡牌视图）
    
    // UI元素
    Menu* _buttonMenu;               ///< 按钮菜单
    Label* _scoreLabel;              ///< 分数标签
    Label* _timeLabel;               ///< 时间标签
    Label* _levelLabel;              ///< 关卡标签
    Label* _movesLabel;              ///< 移动次数标签
    
    // 按钮
    MenuItemImage* _startButton;     ///< 开始按钮
    MenuItemImage* _pauseButton;     ///< 暂停按钮
    MenuItemImage* _resumeButton;    ///< 恢复按钮
    MenuItem* _restartButton;   ///< 重启按钮
    MenuItem* _exitButton;      ///< 退出按钮
    MenuItem* _undoButton;      ///< 撤销按钮
    MenuItem* _hintButton;      ///< 提示按钮
    
    // 卡牌区域
    Layer* _playfieldArea;          ///< 主牌区
    Layer* _stackArea;              ///< 备用牌堆区域
    Layer* _handArea;               ///< 手牌区域
    
    // 游戏结果面板
    Layer* _gameResultPanel;         ///< 游戏结果面板
    Label* _resultLabel;             ///< 结果标签
    Label* _finalScoreLabel;         ///< 最终分数标签
    
    // 卡牌视图管理
    std::map<int, CardView*> _cardViews;  ///< 卡牌视图映射
    
    // 提示效果
    std::vector<CardView*> _hintCardViews;  ///< 提示卡牌视图
    
    /**
     * @brief 更新卡牌视图位置
     */
    void updateCardViewPositions();
    
    /**
     * @brief 更新卡牌视图层级
     */
    void updateCardViewZOrders();
    
    /**
     * @brief 创建卡牌视图
     * @param cardModel 卡牌数据模型
     * @return 卡牌视图指针
     */
    CardView* createCardView(CardModel* cardModel);
    
    /**
     * @brief 布局卡牌区域
     */
    void layoutCardAreas();
    
    /**
     * @brief 获取卡牌区域位置
     * @param areaName 区域名称
     * @return 区域位置
     */
    Vec2 getCardAreaPosition(const std::string& areaName);
    
    /**
     * @brief 获取卡牌在区域内的位置
     * @param cardIndex 卡牌索引
     * @param totalCards 总卡牌数
     * @param areaPosition 区域位置
     * @return 卡牌位置
     */
    Vec2 getCardPositionInArea(int cardIndex, int totalCards, const Vec2& areaPosition);
};

#endif // __GAME_VIEW_H__
