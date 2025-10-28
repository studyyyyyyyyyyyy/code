#ifndef __CARD_RES_CONFIG_H__
#define __CARD_RES_CONFIG_H__

#include "cocos2d.h"

using namespace cocos2d;

/**
 * @brief 卡牌UI资源配置类
 * 
 * 职责：
 * - 存储卡牌相关的UI资源路径和配置
 * - 包含卡牌正面、背面纹理路径
 * - 包含卡牌尺寸、位置等UI参数
 * 
 * 使用场景：
 * - 卡牌视图初始化时加载资源
 * - 不同主题切换时更新资源路径
 * - UI布局计算时获取尺寸参数
 */
class CardResConfig : public Ref
{
public:
    /**
     * @brief 构造函数
     */
    CardResConfig();
    
    /**
     * @brief 析构函数
     */
    ~CardResConfig();
    
    /**
     * @brief 初始化
     * @return 是否成功
     */
    virtual bool init();
    
    // 卡牌尺寸配置
    Size _cardSize;                     ///< 卡牌尺寸
    float _cardCornerRadius;            ///< 卡牌圆角半径
    float _cardBorderWidth;             ///< 卡牌边框宽度
    
    // 纹理资源路径
    std::string _cardBackTexturePath;   ///< 卡牌背面纹理路径
    std::string _cardFrontTexturePath;  ///< 卡牌正面纹理路径（模板）
    std::string _suitTexturePath;       ///< 花色纹理路径
    std::string _rankTexturePath;       ///< 点数纹理路径
    
    // 颜色配置
    Color4F _cardBackColor;              ///< 卡牌背面颜色
    Color4F _cardFrontColor;             ///< 卡牌正面颜色
    Color4F _cardBorderColor;            ///< 卡牌边框颜色
    Color4F _redSuitColor;               ///< 红色花色颜色
    Color4F _blackSuitColor;             ///< 黑色花色颜色
    
    // 字体配置
    std::string _rankFontPath;           ///< 点数字体路径
    int _rankFontSize;                   ///< 点数字体大小
    std::string _suitFontPath;           ///< 花色字体路径
    int _suitFontSize;                   ///< 花色字体大小
    
    // 动画配置
    float _flipAnimationDuration;        ///< 翻转动画持续时间
    float _moveAnimationDuration;         ///< 移动动画持续时间
    float _scaleAnimationDuration;       ///< 缩放动画持续时间
    
    // 主题配置
    std::string _themeName;              ///< 主题名称
    std::string _themeDescription;       ///< 主题描述
    
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
     * @brief 获取指定花色的颜色
     * @param suit 花色 (0-3: 红桃、方块、梅花、黑桃)
     * @return 花色对应的颜色
     */
    Color4F getSuitColor(int suit) const;
    
    /**
     * @brief 获取卡牌显示区域
     * @return 卡牌显示区域大小
     */
    Size getDisplaySize() const;
    
    /**
     * @brief 设置默认配置
     */
    void setDefaultConfig();
    
    /**
     * @brief 创建CardResConfig实例
     * @return CardResConfig实例
     */
    static CardResConfig* create();
    
    /**
     * @brief 获取卡牌背面纹理路径
     * @return 卡牌背面纹理路径
     */
    std::string getCardBackTexturePath() const;
    
    /**
     * @brief 获取花色纹理路径
     * @return 花色纹理路径
     */
    std::string getSuitTexturePath() const;
    
    /**
     * @brief 获取点数纹理路径
     * @return 点数纹理路径
     */
    std::string getRankTexturePath() const;
    
    /**
     * @brief 获取卡牌正面颜色
     * @return 卡牌正面颜色
     */
    Color4F getCardFrontColor() const;
    
    /**
     * @brief 获取卡牌边框颜色
     * @return 卡牌边框颜色
     */
    Color4F getCardBorderColor() const;
    
    /**
     * @brief 获取卡牌背面颜色
     * @return 卡牌背面颜色
     */
    Color4F getCardBackColor() const;
    
    /**
     * @brief 获取花色字体路径
     * @return 花色字体路径
     */
    std::string getSuitFontPath() const;
    
    /**
     * @brief 获取花色字体大小
     * @return 花色字体大小
     */
    int getSuitFontSize() const;
    
    /**
     * @brief 获取点数字体路径
     * @return 点数字体路径
     */
    std::string getRankFontPath() const;
    
    /**
     * @brief 获取点数字体大小
     * @return 点数字体大小
     */
    int getRankFontSize() const;
};

#endif // __CARD_RES_CONFIG_H__
