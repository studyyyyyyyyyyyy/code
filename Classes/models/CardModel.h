#ifndef __CARD_MODEL_H__
#define __CARD_MODEL_H__

#include "cocos2d.h"

using namespace cocos2d;

/**
 * @brief 卡牌数据模型
 * 
 * 职责：
 * - 存储卡牌的基本数据（花色、点数、ID等）
 * - 管理卡牌的状态（是否正面朝上、是否被选中等）
 * - 提供卡牌数据的序列化和反序列化
 * 
 * 使用场景：
 * - 游戏逻辑中表示一张卡牌
 * - 存档系统中保存卡牌状态
 * - 网络同步中传输卡牌数据
 */
class CardModel
{
public:
    /**
     * @brief 花色枚举
     */
    enum Suit {
        CLUBS = 0,     ///< 梅花
        DIAMONDS = 1,  ///< 方块
        HEARTS = 2,    ///< 红桃
        SPADES = 3     ///< 黑桃
    };
    
    /**
     * @brief 点数枚举
     */
    enum Rank {
        ACE = 0,
        TWO = 1,
        THREE = 2,
        FOUR = 3,
        FIVE = 4,
        SIX = 5,
        SEVEN = 6,
        EIGHT = 7,
        NINE = 8,
        TEN = 9,
        JACK = 10,
        QUEEN = 11,
        KING = 12
    };
    
    /**
     * @brief 构造函数
     */
    CardModel();
    
    /**
     * @brief 构造函数
     * @param suit 花色
     * @param rank 点数
     * @param cardId 卡牌ID
     */
    CardModel(Suit suit, Rank rank, int cardId);
    
    /**
     * @brief 析构函数
     */
    ~CardModel();
    
    // 基本属性
    int _cardId;                    ///< 卡牌唯一ID
    Suit _suit;                     ///< 花色
    Rank _rank;                     ///< 点数
    
    // 状态属性
    bool _isFaceUp;                 ///< 是否正面朝上
    bool _isSelected;               ///< 是否被选中
    bool _isMovable;                ///< 是否可移动
    bool _isVisible;                ///< 是否可见
    
    // 位置属性
    Vec2 _position;                 ///< 卡牌位置
    int _zOrder;                    ///< 渲染层级
    
    // 游戏相关属性
    int _value;                     ///< 卡牌值（用于21点等游戏）
    std::string _cardName;          ///< 卡牌名称
    
    // 卡牌区域标识
    std::string _cardArea;          ///< 卡牌所在区域 ("playfield", "stack", "hand")
    int _areaIndex;                 ///< 在区域中的索引
    
    /**
     * @brief 获取卡牌值
     * @return 卡牌值
     */
    int getValue() const;
    
    /**
     * @brief 获取卡牌名称
     * @return 卡牌名称
     */
    std::string getCardName() const;
    
    /**
     * @brief 获取花色名称
     * @return 花色名称
     */
    std::string getSuitName() const;
    
    /**
     * @brief 获取点数名称
     * @return 点数名称
     */
    std::string getRankName() const;
    
    /**
     * @brief 检查是否为红色花色
     * @return 是否为红色
     */
    bool isRedSuit() const;
    
    /**
     * @brief 检查是否为黑色花色
     * @return 是否为黑色
     */
    bool isBlackSuit() const;
    
    /**
     * @brief 翻转卡牌
     */
    void flipCard();
    
    /**
     * @brief 设置选中状态
     * @param selected 是否选中
     */
    void setSelected(bool selected);
    
    /**
     * @brief 设置位置
     * @param position 新位置
     */
    void setPosition(const Vec2& position);
    
    /**
     * @brief 设置渲染层级
     * @param zOrder 渲染层级
     */
    void setZOrder(int zOrder);
    
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
     * @brief 重置为默认状态
     */
    void reset();
    
    /**
     * @brief 复制卡牌数据
     * @param other 源卡牌
     */
    void copyFrom(const CardModel& other);
    
    /**
     * @brief 比较两张卡牌是否相等
     * @param other 另一张卡牌
     * @return 是否相等
     */
    bool equals(const CardModel& other) const;
};

#endif // __CARD_MODEL_H__
