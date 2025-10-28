#ifndef __GAME_CONSTANTS_H__
#define __GAME_CONSTANTS_H__

#include <string>

/**
 * @brief 游戏常量定义
 * 
 * 职责：
 * - 定义游戏中使用的所有常量
 * - 使用 k 前缀命名规范
 * - 避免魔法数字和硬编码字符串
 * 
 * 命名规范：
 * - 常量以小写字母 k 开头，后接驼峰命名
 * - 例如：kMaxUndoCount, kCardWidth, kAnimationDuration
 */

namespace GameConstants
{
    // ==================== 游戏规则常量 ====================
    
    /// 默认最大撤销次数
    const int kDefaultMaxUndoCount = 50;
    
    /// 默认目标分数
    const int kDefaultTargetScore = 1000;
    
    /// 三星分数线
    const int kThreeStarsScore = 1800;
    
    /// 两星分数线
    const int kTwoStarsScore = 1400;
    
    /// 一星分数线
    const int kOneStarScore = 1000;
    
    /// 每消除一张牌的分数
    const int kScorePerCard = 100;
    
    // ==================== 卡牌相关常量 ====================
    
    /// 卡牌宽度
    const float kCardWidth = 120.0f;
    
    /// 卡牌高度
    const float kCardHeight = 160.0f;
    
    /// 卡牌边框宽度（普通）
    const float kCardBorderWidthNormal = 2.0f;
    
    /// 卡牌边框宽度（手牌区）
    const float kCardBorderWidthHand = 4.0f;
    
    /// 卡牌高亮边框宽度
    const float kCardHighlightBorderWidth = 8.0f;
    
    // ==================== 动画相关常量 ====================
    
    /// 卡牌移动动画时长
    const float kCardMoveAnimationDuration = 0.3f;
    
    /// 卡牌翻转动画时长
    const float kCardFlipAnimationDuration = 0.5f;
    
    /// 卡牌匹配动画时长
    const float kCardMatchAnimationDuration = 0.2f;
    
    /// 提示动画时长
    const float kHintAnimationDuration = 1.0f;
    
    /// 卡牌缩放动画时长
    const float kCardScaleAnimationDuration = 0.2f;
    
    /// 卡牌提示缩放比例
    const float kCardHintScale = 1.1f;
    
    // ==================== 位置相关常量 ====================
    
    /// 手牌区X坐标
    const float kHandAreaX = 800.0f;
    
    /// 手牌区Y坐标
    const float kHandAreaY = 400.0f;
    
    /// 备用牌堆基础X坐标
    const float kStackAreaBaseX = 200.0f;
    
    /// 备用牌堆基础Y坐标
    const float kStackAreaBaseY = 400.0f;
    
    /// 备用牌堆卡牌间距
    const float kStackCardSpacing = 15.0f;
    
    // ==================== ZOrder 常量 ====================
    
    /// 手牌区初始ZOrder
    const int kHandCardZOrderBase = 100;
    
    /// 手牌区ZOrder递增值
    const int kHandCardZOrderIncrement = 10;
    
    /// 主牌区ZOrder
    const int kPlayfieldCardZOrder = 50;
    
    /// 备用牌堆ZOrder
    const int kStackCardZOrder = 30;
    
    // ==================== 区域名称常量 ====================
    
    /// 主牌区名称
    const std::string kAreaNamePlayfield = "playfield";
    
    /// 手牌区名称
    const std::string kAreaNameHand = "hand";
    
    /// 备用牌堆名称
    const std::string kAreaNameStack = "stack";
    
    // ==================== UI 相关常量 ====================
    
    /// 按钮Y坐标
    const float kButtonY = 150.0f;
    
    /// 按钮间距
    const float kButtonSpacing = 250.0f;
    
    /// 按钮起始X坐标
    const float kButtonStartX = 150.0f;
    
    /// 分数标签字体大小
    const int kScoreLabelFontSize = 32;
    
    /// 时间标签字体大小
    const int kTimeLabelFontSize = 32;
    
    /// 关卡标签字体大小
    const int kLevelLabelFontSize = 28;
    
    /// 移动次数标签字体大小
    const int kMovesLabelFontSize = 28;
    
    /// 按钮标签字体大小
    const int kButtonLabelFontSize = 48;
    
    // ==================== 配置文件路径常量 ====================
    
    /// 关卡配置路径
    const std::string kLevelConfigPath = "configs/levels/";
    
    /// 卡牌资源配置路径
    const std::string kCardConfigPath = "cards/card_config.json";
    
    // ==================== 其他常量 ====================
    
    /// 点击判定距离（小于此距离视为点击，大于视为拖拽）
    const float kClickThresholdDistance = 10.0f;
}

#endif // __GAME_CONSTANTS_H__

