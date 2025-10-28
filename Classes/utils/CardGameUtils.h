#ifndef __CARD_GAME_UTILS_H__
#define __CARD_GAME_UTILS_H__

#include "cocos2d.h"
#include "../models/CardModel.h"

using namespace cocos2d;

/**
 * @brief 纸牌游戏工具类
 * 
 * 职责：
 * - 提供纸牌游戏相关的通用工具函数
 * - 处理卡牌计算、验证等辅助功能
 * - 提供游戏规则相关的工具方法
 * - 提供数学计算和算法工具
 * 
 * 使用场景：
 * - 卡牌值计算
 * - 游戏规则验证
 * - 数学计算辅助
 * - 通用工具函数
 */
class CardGameUtils
{
public:
    /**
     * @brief 计算21点手牌分数
     * @param hand 手牌
     * @return 分数
     */
    static int calculateBlackjackScore(const std::vector<CardModel*>& hand);
    
    /**
     * @brief 检查是否爆牌
     * @param score 分数
     * @return 是否爆牌
     */
    static bool isBusted(int score);
    
    /**
     * @brief 检查是否为21点
     * @param score 分数
     * @return 是否为21点
     */
    static bool isBlackjack(int score);
    
    /**
     * @brief 获取卡牌值
     * @param rank 点数
     * @return 卡牌值
     */
    static int getCardValue(CardModel::Rank rank);
    
    /**
     * @brief 获取花色颜色
     * @param suit 花色
     * @return 颜色
     */
    static Color4F getSuitColor(CardModel::Suit suit);
    
    /**
     * @brief 获取花色名称
     * @param suit 花色
     * @return 花色名称
     */
    static std::string getSuitName(CardModel::Suit suit);
    
    /**
     * @brief 获取点数名称
     * @param rank 点数
     * @return 点数名称
     */
    static std::string getRankName(CardModel::Rank rank);
    
    /**
     * @brief 检查两张卡牌是否匹配
     * @param card1 卡牌1
     * @param card2 卡牌2
     * @return 是否匹配
     */
    static bool isCardMatch(CardModel* card1, CardModel* card2);
    
    /**
     * @brief 检查卡牌是否可以移动
     * @param card 卡牌
     * @param targetPosition 目标位置
     * @return 是否可以移动
     */
    static bool canCardMove(CardModel* card, const Vec2& targetPosition);
    
    /**
     * @brief 计算两点间距离
     * @param point1 点1
     * @param point2 点2
     * @return 距离
     */
    static float calculateDistance(const Vec2& point1, const Vec2& point2);
    
    /**
     * @brief 检查点是否在矩形内
     * @param point 点
     * @param rect 矩形
     * @return 是否在矩形内
     */
    static bool isPointInRect(const Vec2& point, const Rect& rect);
    
    /**
     * @brief 获取随机整数
     * @param min 最小值
     * @param max 最大值
     * @return 随机整数
     */
    static int getRandomInt(int min, int max);
    
    /**
     * @brief 获取随机浮点数
     * @param min 最小值
     * @param max 最大值
     * @return 随机浮点数
     */
    static float getRandomFloat(float min, float max);
    
    /**
     * @brief 格式化时间字符串
     * @param seconds 秒数
     * @return 格式化的时间字符串
     */
    static std::string formatTimeString(float seconds);
    
    /**
     * @brief 格式化分数字符串
     * @param score 分数
     * @return 格式化的分数字符串
     */
    static std::string formatScoreString(int score);
    
    /**
     * @brief 验证卡牌ID
     * @param cardId 卡牌ID
     * @return 是否有效
     */
    static bool validateCardId(int cardId);
    
    /**
     * @brief 验证位置
     * @param position 位置
     * @return 是否有效
     */
    static bool validatePosition(const Vec2& position);
    
    /**
     * @brief 验证分数
     * @param score 分数
     * @return 是否有效
     */
    static bool validateScore(int score);
    
    /**
     * @brief 限制数值范围
     * @param value 数值
     * @param min 最小值
     * @param max 最大值
     * @return 限制后的数值
     */
    static int clampInt(int value, int min, int max);
    
    /**
     * @brief 限制浮点数范围
     * @param value 数值
     * @param min 最小值
     * @param max 最大值
     * @return 限制后的数值
     */
    static float clampFloat(float value, float min, float max);
    
    /**
     * @brief 线性插值
     * @param start 起始值
     * @param end 结束值
     * @param t 插值参数
     * @return 插值结果
     */
    static float lerp(float start, float end, float t);
    
    /**
     * @brief 向量线性插值
     * @param start 起始向量
     * @param end 结束向量
     * @param t 插值参数
     * @return 插值结果
     */
    static Vec2 lerpVec2(const Vec2& start, const Vec2& end, float t);
    
    /**
     * @brief 检查字符串是否为空
     * @param str 字符串
     * @return 是否为空
     */
    static bool isStringEmpty(const std::string& str);
    
    /**
     * @brief 去除字符串前后空格
     * @param str 字符串
     * @return 处理后的字符串
     */
    static std::string trimString(const std::string& str);
    
    /**
     * @brief 转换字符串为小写
     * @param str 字符串
     * @return 小写字符串
     */
    static std::string toLowerCase(const std::string& str);
    
    /**
     * @brief 转换字符串为大写
     * @param str 字符串
     * @return 大写字符串
     */
    static std::string toUpperCase(const std::string& str);
    
    /**
     * @brief 分割字符串
     * @param str 字符串
     * @param delimiter 分隔符
     * @return 分割后的字符串列表
     */
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    
    /**
     * @brief 连接字符串列表
     * @param strings 字符串列表
     * @param delimiter 分隔符
     * @return 连接后的字符串
     */
    static std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter);
    
    /**
     * @brief 获取当前时间戳
     * @return 时间戳
     */
    static float getCurrentTimestamp();
    
    /**
     * @brief 获取当前日期字符串
     * @return 日期字符串
     */
    static std::string getCurrentDateString();
    
    /**
     * @brief 获取当前时间字符串
     * @return 时间字符串
     */
    static std::string getCurrentTimeString();
    
private:
    /**
     * @brief 私有构造函数，防止实例化
     */
    CardGameUtils() = delete;
    
    /**
     * @brief 私有析构函数
     */
    ~CardGameUtils() = delete;
    
    /**
     * @brief 禁用拷贝构造函数
     */
    CardGameUtils(const CardGameUtils&) = delete;
    
    /**
     * @brief 禁用赋值操作符
     */
    CardGameUtils& operator=(const CardGameUtils&) = delete;
};

#endif // __CARD_GAME_UTILS_H__
