#ifndef __CARD_CONTROLLER_H__
#define __CARD_CONTROLLER_H__

#include "cocos2d.h"
#include "../models/CardModel.h"
#include "../views/CardView.h"

using namespace cocos2d;

/**
 * @brief 卡牌控制器类
 * 
 * 职责：
 * - 处理卡牌相关的具体逻辑
 * - 管理卡牌的状态变化和动画
 * - 协调卡牌模型和视图之间的交互
 * - 处理卡牌的游戏规则逻辑
 * 
 * 使用场景：
 * - 卡牌移动、翻转、选中等操作
 * - 卡牌动画效果控制
 * - 卡牌游戏规则验证
 * - 卡牌状态同步
 */
class CardController
{
public:
    /**
     * @brief 构造函数
     */
    CardController();
    
    /**
     * @brief 析构函数
     */
    ~CardController();
    
    /**
     * @brief 初始化卡牌控制器
     * @param cardModel 卡牌数据模型
     * @param cardView 卡牌视图
     * @return 是否成功
     */
    bool init(CardModel* cardModel, CardView* cardView);
    
    /**
     * @brief 处理卡牌点击事件
     * @return 是否处理成功
     */
    bool handleCardClick();
    
    /**
     * @brief 处理卡牌拖拽开始事件
     * @return 是否处理成功
     */
    bool handleCardDragStart();
    
    /**
     * @brief 处理卡牌拖拽移动事件
     * @param position 位置
     * @return 是否处理成功
     */
    bool handleCardDragMove(const Vec2& position);
    
    /**
     * @brief 处理卡牌拖拽结束事件
     * @param position 位置
     * @return 是否处理成功
     */
    bool handleCardDragEnd(const Vec2& position);
    
    /**
     * @brief 翻转卡牌
     * @param animated 是否播放动画
     * @return 是否成功
     */
    bool flipCard(bool animated = true);
    
    /**
     * @brief 移动卡牌
     * @param targetPosition 目标位置
     * @param animated 是否播放动画
     * @return 是否成功
     */
    bool moveCard(const Vec2& targetPosition, bool animated = true);
    
    /**
     * @brief 选中卡牌
     * @param selected 是否选中
     * @param animated 是否播放动画
     * @return 是否成功
     */
    bool selectCard(bool selected, bool animated = true);
    
    /**
     * @brief 设置卡牌可见性
     * @param visible 是否可见
     * @return 是否成功
     */
    bool setCardVisible(bool visible);
    
    /**
     * @brief 设置卡牌可移动性
     * @param movable 是否可移动
     * @return 是否成功
     */
    bool setCardMovable(bool movable);
    
    /**
     * @brief 播放匹配动画
     * @return 是否成功
     */
    bool playMatchAnimation();
    
    /**
     * @brief 播放提示动画
     * @return 是否成功
     */
    bool playHintAnimation();
    
    /**
     * @brief 更新卡牌状态
     * @return 是否成功
     */
    bool updateCardState();
    
    /**
     * @brief 同步模型到视图
     * @return 是否成功
     */
    bool syncModelToView();
    
    /**
     * @brief 同步视图到模型
     * @return 是否成功
     */
    bool syncViewToModel();
    
    /**
     * @brief 获取卡牌ID
     * @return 卡牌ID
     */
    int getCardId() const;
    
    /**
     * @brief 获取卡牌数据模型
     * @return 卡牌数据模型
     */
    CardModel* getCardModel() const;
    
    /**
     * @brief 获取卡牌视图
     * @return 卡牌视图
     */
    CardView* getCardView() const;
    
    /**
     * @brief 设置卡牌数据模型
     * @param cardModel 卡牌数据模型
     */
    void setCardModel(CardModel* cardModel);
    
    /**
     * @brief 设置卡牌视图
     * @param cardView 卡牌视图
     */
    void setCardView(CardView* cardView);
    
    /**
     * @brief 检查卡牌是否有效
     * @return 是否有效
     */
    bool isValid() const;
    
    /**
     * @brief 检查卡牌是否可操作
     * @return 是否可操作
     */
    bool isOperable() const;
    
    /**
     * @brief 检查卡牌是否被选中
     * @return 是否被选中
     */
    bool isSelected() const;
    
    /**
     * @brief 检查卡牌是否正面朝上
     * @return 是否正面朝上
     */
    bool isFaceUp() const;
    
    /**
     * @brief 检查卡牌是否可移动
     * @return 是否可移动
     */
    bool isMovable() const;
    
    /**
     * @brief 检查卡牌是否可见
     * @return 是否可见
     */
    bool isVisible() const;
    
protected:
    /**
     * @brief 验证操作权限
     * @param operation 操作类型
     * @return 是否有权限
     */
    bool validateOperation(const std::string& operation);
    
    /**
     * @brief 执行卡牌翻转逻辑
     * @return 是否成功
     */
    bool executeFlipLogic();
    
    /**
     * @brief 执行卡牌移动逻辑
     * @param targetPosition 目标位置
     * @return 是否成功
     */
    bool executeMoveLogic(const Vec2& targetPosition);
    
    /**
     * @brief 执行卡牌选中逻辑
     * @param selected 是否选中
     * @return 是否成功
     */
    bool executeSelectLogic(bool selected);
    
    /**
     * @brief 执行卡牌可见性逻辑
     * @param visible 是否可见
     * @return 是否成功
     */
    bool executeVisibleLogic(bool visible);
    
    /**
     * @brief 执行卡牌可移动性逻辑
     * @param movable 是否可移动
     * @return 是否成功
     */
    bool executeMovableLogic(bool movable);
    
    /**
     * @brief 更新卡牌渲染状态
     * @return 是否成功
     */
    bool updateRenderState();
    
    /**
     * @brief 更新卡牌交互状态
     * @return 是否成功
     */
    bool updateInteractionState();
    
private:
    CardModel* _cardModel;           ///< 卡牌数据模型
    CardView* _cardView;             ///< 卡牌视图
    bool _isInitialized;             ///< 是否已初始化
    
    /**
     * @brief 清理资源
     */
    void cleanup();
    
    /**
     * @brief 验证卡牌状态
     * @return 是否有效
     */
    bool validateCardState();
    
    /**
     * @brief 处理卡牌状态变化
     * @param oldState 旧状态
     * @param newState 新状态
     */
    void handleStateChange(const std::string& oldState, const std::string& newState);
    
    /**
     * @brief 处理卡牌位置变化
     * @param oldPosition 旧位置
     * @param newPosition 新位置
     */
    void handlePositionChange(const Vec2& oldPosition, const Vec2& newPosition);
    
    /**
     * @brief 处理卡牌层级变化
     * @param oldZOrder 旧层级
     * @param newZOrder 新层级
     */
    void handleZOrderChange(int oldZOrder, int newZOrder);
    
    /**
     * @brief 获取卡牌状态描述
     * @return 状态描述
     */
    std::string getCardStateDescription() const;
    
    /**
     * @brief 记录操作日志
     * @param operation 操作类型
     * @param success 是否成功
     */
    void logOperation(const std::string& operation, bool success);
};

#endif // __CARD_CONTROLLER_H__
