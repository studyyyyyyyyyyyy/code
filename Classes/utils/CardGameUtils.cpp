#include "CardGameUtils.h"
#include "../models/CardModel.h"
#include <random>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>

int CardGameUtils::calculateBlackjackScore(const std::vector<CardModel*>& hand)
{
    int score = 0;
    int aceCount = 0;
    
    for (auto card : hand)
    {
        if (card->_rank == CardModel::ACE)
        {
            aceCount++;
            score += 11;
        }
        else if (card->_rank >= CardModel::JACK)
        {
            score += 10;
        }
        else
        {
            score += static_cast<int>(card->_rank);
        }
    }
    
    // 处理A的软硬点数
    while (score > 21 && aceCount > 0)
    {
        score -= 10;
        aceCount--;
    }
    
    return score;
}

bool CardGameUtils::isBusted(int score)
{
    return score > 21;
}

bool CardGameUtils::isBlackjack(int score)
{
    return score == 21;
}

int CardGameUtils::getCardValue(CardModel::Rank rank)
{
    switch (rank)
    {
        case CardModel::ACE:
            return 11;
        case CardModel::JACK:
        case CardModel::QUEEN:
        case CardModel::KING:
            return 10;
        default:
            return static_cast<int>(rank);
    }
}

Color4F CardGameUtils::getSuitColor(CardModel::Suit suit)
{
    switch (suit)
    {
        case CardModel::HEARTS:
        case CardModel::DIAMONDS:
            return Color4F::RED;
        case CardModel::CLUBS:
        case CardModel::SPADES:
            return Color4F::BLACK;
        default:
            return Color4F::WHITE;
    }
}

std::string CardGameUtils::getSuitName(CardModel::Suit suit)
{
    switch (suit)
    {
        case CardModel::HEARTS:
            return "Hearts";
        case CardModel::DIAMONDS:
            return "Diamonds";
        case CardModel::CLUBS:
            return "Clubs";
        case CardModel::SPADES:
            return "Spades";
        default:
            return "Unknown";
    }
}

std::string CardGameUtils::getRankName(CardModel::Rank rank)
{
    switch (rank)
    {
        case CardModel::ACE:
            return "Ace";
        case CardModel::TWO:
            return "Two";
        case CardModel::THREE:
            return "Three";
        case CardModel::FOUR:
            return "Four";
        case CardModel::FIVE:
            return "Five";
        case CardModel::SIX:
            return "Six";
        case CardModel::SEVEN:
            return "Seven";
        case CardModel::EIGHT:
            return "Eight";
        case CardModel::NINE:
            return "Nine";
        case CardModel::TEN:
            return "Ten";
        case CardModel::JACK:
            return "Jack";
        case CardModel::QUEEN:
            return "Queen";
        case CardModel::KING:
            return "King";
        default:
            return "Unknown";
    }
}

bool CardGameUtils::isCardMatch(CardModel* card1, CardModel* card2)
{
    if (!card1 || !card2) return false;
    
    // 检查花色和点数是否相同
    return card1->_suit == card2->_suit && card1->_rank == card2->_rank;
}

bool CardGameUtils::canCardMove(CardModel* card, const Vec2& targetPosition)
{
    if (!card) return false;
    
    // 检查卡牌是否可移动
    if (!card->_isMovable) return false;
    
    // 检查目标位置是否有效
    if (!validatePosition(targetPosition)) return false;
    
    return true;
}

float CardGameUtils::calculateDistance(const Vec2& point1, const Vec2& point2)
{
    Vec2 diff = point1 - point2;
    return diff.length();
}

bool CardGameUtils::isPointInRect(const Vec2& point, const Rect& rect)
{
    return rect.containsPoint(point);
}

int CardGameUtils::getRandomInt(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

float CardGameUtils::getRandomFloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

std::string CardGameUtils::formatTimeString(float seconds)
{
    int minutes = static_cast<int>(seconds) / 60;
    int secs = static_cast<int>(seconds) % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << secs;
    
    return oss.str();
}

std::string CardGameUtils::formatScoreString(int score)
{
    return std::to_string(score);
}

bool CardGameUtils::validateCardId(int cardId)
{
    return cardId > 0;
}

bool CardGameUtils::validatePosition(const Vec2& position)
{
    return !std::isnan(position.x) && !std::isnan(position.y);
}

bool CardGameUtils::validateScore(int score)
{
    return score >= 0;
}

int CardGameUtils::clampInt(int value, int min, int max)
{
    return std::max(min, std::min(max, value));
}

float CardGameUtils::clampFloat(float value, float min, float max)
{
    return std::max(min, std::min(max, value));
}

float CardGameUtils::lerp(float start, float end, float t)
{
    return start + t * (end - start);
}

Vec2 CardGameUtils::lerpVec2(const Vec2& start, const Vec2& end, float t)
{
    return Vec2(lerp(start.x, end.x, t), lerp(start.y, end.y, t));
}

bool CardGameUtils::isStringEmpty(const std::string& str)
{
    return str.empty();
}

std::string CardGameUtils::trimString(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string CardGameUtils::toLowerCase(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string CardGameUtils::toUpperCase(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::vector<std::string> CardGameUtils::splitString(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string CardGameUtils::joinStrings(const std::vector<std::string>& strings, const std::string& delimiter)
{
    if (strings.empty()) return "";
    
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i)
    {
        if (i > 0) oss << delimiter;
        oss << strings[i];
    }
    
    return oss.str();
}

float CardGameUtils::getCurrentTimestamp()
{
    return static_cast<float>(time(nullptr));
}

std::string CardGameUtils::getCurrentDateString()
{
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
    
    return std::string(buffer);
}

std::string CardGameUtils::getCurrentTimeString()
{
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    
    return std::string(buffer);
}
