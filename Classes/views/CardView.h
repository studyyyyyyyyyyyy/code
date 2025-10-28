#ifndef __CARD_VIEW_H__
#define __CARD_VIEW_H__

#include "cocos2d.h"
#include "../models/CardModel.h"
#include "../managers/ResourceManager.h"

using namespace cocos2d;

/**
 * @brief 卡牌视图类
 * 
 * 职责：
 * - 负责卡牌的UI显示和用户交互
 * - 处理卡牌的动画效果（翻转、移动、缩放等）
 * - 响应触摸事件并通知控制器
 * 
 * 使用场景：
 * - 游戏界面中显示卡牌
 * - 处理卡牌的点击、拖拽等交互
 * - 播放卡牌相关的动画效果
 */
class CardView : public Sprite
{
public:
    /**
     * @brief 创建卡牌视图
     * @param cardModel 卡牌数据模型（const，View层只读）
     * @param resConfig 资源配置
     * @param resourceManager 资源管理器（可选，用于加载纹理）
     * @return 卡牌视图指针
     */
    static CardView* create(const CardModel* cardModel, CardResConfig* resConfig, class ResourceManager* resourceManager = nullptr);
    
    /**
     * @brief 初始化
     * @param cardModel 卡牌数据模型（const，View层只读）
     * @param resConfig 资源配置
     * @param resourceManager 资源管理器（可选，用于加载纹理）
     * @return 是否成功
     */
    virtual bool init(const CardModel* cardModel, CardResConfig* resConfig, class ResourceManager* resourceManager = nullptr);
    
    /**
     * @brief 析构函数
     */
    virtual ~CardView();
    
    // 回调函数类型定义
    typedef std::function<void(int cardId)> CardClickCallback;
    typedef std::function<void(int cardId)> CardDragStartCallback;
    typedef std::function<void(int cardId, const Vec2& position)> CardDragMoveCallback;
    typedef std::function<void(int cardId, const Vec2& position)> CardDragEndCallback;
    
    // 回调函数
    CardClickCallback _onCardClickCallback;          ///< 卡牌点击回调
    CardDragStartCallback _onCardDragStartCallback; ///< 卡牌拖拽开始回调
    CardDragMoveCallback _onCardDragMoveCallback;   ///< 卡牌拖拽移动回调
    CardDragEndCallback _onCardDragEndCallback;     ///< 卡牌拖拽结束回调
    
    /**
     * @brief 设置卡牌点击回调
     * @param callback 回调函数
     */
    void setCardClickCallback(CardClickCallback callback);
    
    /**
     * @brief 设置卡牌拖拽开始回调
     * @param callback 回调函数
     */
    void setCardDragStartCallback(CardDragStartCallback callback);
    
    /**
     * @brief 设置卡牌拖拽移动回调
     * @param callback 回调函数
     */
    void setCardDragMoveCallback(CardDragMoveCallback callback);
    
    /**
     * @brief 设置卡牌拖拽结束回调
     * @param callback 回调函数
     */
    void setCardDragEndCallback(CardDragEndCallback callback);
    
    /**
     * @brief 更新卡牌显示
     */
    void updateCardDisplay();
    
    /**
     * @brief 使用ResourceManager更新卡牌纹理
     */
    void updateCardTexture();
    
    /**
     * @brief 播放翻转动画
     * @param duration 动画持续时间
     */
    void playFlipAnimation(float duration = 0.5f);
    
    /**
     * @brief 播放移动动画
     * @param targetPosition 目标位置
     * @param duration 动画持续时间
     * @param onComplete 动画完成回调
     */
    void playMoveAnimation(const Vec2& targetPosition, float duration = 0.3f, const std::function<void()>& onComplete = nullptr);
    
    /**
     * @brief 播放缩放动画
     * @param targetScale 目标缩放
     * @param duration 动画持续时间
     */
    void playScaleAnimation(float targetScale, float duration = 0.2f);
    
    /**
     * @brief 播放选中动画
     * @param selected 是否选中
     */
    void playSelectAnimation(bool selected);
    
    /**
     * @brief 播放匹配动画
     * @param cardId 卡牌ID
     */
    void playMatchAnimation(int cardId);
    
    /**
     * @brief 设置卡牌是否可交互
     * @param interactive 是否可交互
     */
    void setInteractive(bool interactive);
    
    /**
     * @brief 获取卡牌ID
     * @return 卡牌ID
     */
    int getCardId() const;
    
    /**
     * @brief 获取卡牌数据模型
     * @return 卡牌数据模型（const）
     */
    const CardModel* getCardModel() const;
    
    /**
     * @brief 设置卡牌数据模型
     * @param cardModel 卡牌数据模型（const，View层只读）
     */
    void setCardModel(const CardModel* cardModel);
    
    /**
     * @brief 刷新卡牌状态
     */
    void refreshCardState();
    
protected:
    /**
     * @brief 触摸事件处理
     */
    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void onTouchMoved(Touch* touch, Event* event);
    virtual void onTouchEnded(Touch* touch, Event* event);
    virtual void onTouchCancelled(Touch* touch, Event* event);
    
    /**
     * @brief 更新卡牌位置
     */
    void updateCardPosition();
    
    /**
     * @brief 更新卡牌层级
     */
    void updateCardZOrder();
    
public:
    /**
     * @brief 检查是否正在播放移动动画
     */
    bool isPlayingMoveAnimation() const { return _isPlayingMoveAnimation; }
    
private:
    const CardModel* _cardModel;     ///< 卡牌数据模型（只读）
    CardResConfig* _resConfig;       ///< 资源配置
    class ResourceManager* _resourceManager;  ///< 资源管理器（用于加载纹理）
    bool _isInteractive;             ///< 是否可交互
    bool _isDragging;                ///< 是否正在拖拽
    bool _isPlayingMoveAnimation;    ///< 是否正在播放移动动画
    Vec2 _dragStartPosition;         ///< 拖拽开始位置
    Vec2 _originalPosition;          ///< 原始位置
};

#endif // __CARD_VIEW_H__
