#ifndef __UNDO_MODEL_H__
#define __UNDO_MODEL_H__

#include "cocos2d.h"
#include "CardModel.h"

// 前向声明
class GameModel;

using namespace cocos2d;

/**
 * @brief 撤销操作数据模型
 * 
 * 职责：
 * - 存储单次操作的数据，用于撤销功能
 * - 记录操作类型、操作前后的状态等
 * - 提供撤销数据的序列化和反序列化
 * 
 * 使用场景：
 * - 撤销功能中记录操作历史
 * - 存档系统中保存操作历史
 * - 网络同步中传输操作数据
 */
class UndoModel
{
public:
    /**
     * @brief 操作类型枚举
     */
    enum ActionType {
        ACTION_NONE = 0,            ///< 无操作
        ACTION_MOVE_CARD = 1,       ///< 移动卡牌
        ACTION_FLIP_CARD = 2,       ///< 翻转卡牌
        ACTION_DEAL_CARD = 3,       ///< 发牌
        ACTION_SHUFFLE = 4,         ///< 洗牌
        ACTION_SELECT_CARD = 5,     ///< 选中卡牌
        ACTION_DESELECT_CARD = 6    ///< 取消选中卡牌
    };
    
    /**
     * @brief 构造函数
     */
    UndoModel();
    
    /**
     * @brief 构造函数
     * @param actionType 操作类型
     * @param cardId 卡牌ID
     * @param fromPosition 起始位置
     * @param toPosition 目标位置
     */
    UndoModel(ActionType actionType, int cardId, const std::string& fromPosition, const std::string& toPosition);
    
    /**
     * @brief 析构函数
     */
    ~UndoModel();
    
    // 操作基本信息
    ActionType _actionType;          ///< 操作类型
    int _cardId;                     ///< 卡牌ID
    std::string _fromPosition;       ///< 起始位置
    std::string _toPosition;         ///< 目标位置
    
    // 操作时间
    float _timestamp;                ///< 时间戳
    int _moveIndex;                  ///< 移动索引
    
    // 操作前状态
    CardModel* _cardBeforeState;     ///< 操作前卡牌状态
    Vec2 _positionBefore;            ///< 操作前位置
    bool _faceUpBefore;              ///< 操作前是否正面朝上
    bool _selectedBefore;             ///< 操作前是否选中
    
    // 操作后状态
    CardModel* _cardAfterState;      ///< 操作后卡牌状态
    Vec2 _positionAfter;             ///< 操作后位置
    bool _faceUpAfter;               ///< 操作后是否正面朝上
    bool _selectedAfter;              ///< 操作后是否选中
    
    // 额外数据
    std::string _extraData;          ///< 额外数据
    std::map<std::string, std::string> _metadata;  ///< 元数据
    
    /**
     * @brief 设置操作前状态
     * @param card 卡牌
     */
    void setBeforeState(CardModel* card);
    
    /**
     * @brief 设置操作后状态
     * @param card 卡牌
     */
    void setAfterState(CardModel* card);
    
    /**
     * @brief 获取操作描述
     * @return 操作描述
     */
    std::string getActionDescription() const;
    
    /**
     * @brief 检查是否可以撤销
     * @return 是否可以撤销
     */
    bool canUndo() const;
    
    /**
     * @brief 执行撤销操作
     * @param gameModel 游戏模型
     * @return 是否成功
     */
    bool executeUndo(GameModel* gameModel);
    
    /**
     * @brief 执行重做操作
     * @param gameModel 游戏模型
     * @return 是否成功
     */
    bool executeRedo(GameModel* gameModel);
    
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
    
    /**
     * @brief 复制撤销数据
     * @param other 源撤销数据
     */
    void copyFrom(const UndoModel& other);
    
    /**
     * @brief 比较两个撤销操作是否相等
     * @param other 另一个撤销操作
     * @return 是否相等
     */
    bool equals(const UndoModel& other) const;
    
private:
    /**
     * @brief 创建卡牌状态副本
     * @param card 源卡牌
     * @return 卡牌状态副本
     */
    CardModel* createCardStateCopy(CardModel* card);
    
    /**
     * @brief 应用卡牌状态
     * @param targetCard 目标卡牌
     * @param stateCard 状态卡牌
     */
    void applyCardState(CardModel* targetCard, CardModel* stateCard);
};

#endif // __UNDO_MODEL_H__
