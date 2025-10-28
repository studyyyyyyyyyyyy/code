#include "CardResConfig.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

CardResConfig::CardResConfig()
: _cardSize(Size(60, 90))
, _cardCornerRadius(5.0f)
, _cardBorderWidth(2.0f)
, _cardBackTexturePath("")
, _cardFrontTexturePath("")
, _suitTexturePath("")
, _rankTexturePath("")
, _cardBackColor(Color4F::BLUE)
, _cardFrontColor(Color4F::WHITE)
, _cardBorderColor(Color4F::BLACK)
, _redSuitColor(Color4F::RED)
, _blackSuitColor(Color4F::BLACK)
, _rankFontPath("fonts/Marker Felt.ttf")
, _rankFontSize(16)
, _suitFontPath("fonts/Marker Felt.ttf")
, _suitFontSize(20)
, _flipAnimationDuration(0.5f)
, _moveAnimationDuration(0.3f)
, _scaleAnimationDuration(0.2f)
, _themeName("Default")
, _themeDescription("默认主题")
{
}

CardResConfig::~CardResConfig()
{
}

bool CardResConfig::init()
{
    setDefaultConfig();
    return true;
}

std::string CardResConfig::serialize() const
{
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    // 尺寸配置
    rapidjson::Value sizeObj(rapidjson::kObjectType);
    sizeObj.AddMember("width", _cardSize.width, allocator);
    sizeObj.AddMember("height", _cardSize.height, allocator);
    doc.AddMember("cardSize", sizeObj, allocator);
    
    doc.AddMember("cardCornerRadius", _cardCornerRadius, allocator);
    doc.AddMember("cardBorderWidth", _cardBorderWidth, allocator);
    
    // 纹理路径
    doc.AddMember("cardBackTexturePath", rapidjson::StringRef(_cardBackTexturePath.c_str()), allocator);
    doc.AddMember("cardFrontTexturePath", rapidjson::StringRef(_cardFrontTexturePath.c_str()), allocator);
    doc.AddMember("suitTexturePath", rapidjson::StringRef(_suitTexturePath.c_str()), allocator);
    doc.AddMember("rankTexturePath", rapidjson::StringRef(_rankTexturePath.c_str()), allocator);
    
    // 颜色配置
    rapidjson::Value backColorObj(rapidjson::kObjectType);
    backColorObj.AddMember("r", _cardBackColor.r, allocator);
    backColorObj.AddMember("g", _cardBackColor.g, allocator);
    backColorObj.AddMember("b", _cardBackColor.b, allocator);
    backColorObj.AddMember("a", _cardBackColor.a, allocator);
    doc.AddMember("cardBackColor", backColorObj, allocator);
    
    rapidjson::Value frontColorObj(rapidjson::kObjectType);
    frontColorObj.AddMember("r", _cardFrontColor.r, allocator);
    frontColorObj.AddMember("g", _cardFrontColor.g, allocator);
    frontColorObj.AddMember("b", _cardFrontColor.b, allocator);
    frontColorObj.AddMember("a", _cardFrontColor.a, allocator);
    doc.AddMember("cardFrontColor", frontColorObj, allocator);
    
    // 字体配置
    doc.AddMember("rankFontPath", rapidjson::StringRef(_rankFontPath.c_str()), allocator);
    doc.AddMember("rankFontSize", _rankFontSize, allocator);
    doc.AddMember("suitFontPath", rapidjson::StringRef(_suitFontPath.c_str()), allocator);
    doc.AddMember("suitFontSize", _suitFontSize, allocator);
    
    // 动画配置
    doc.AddMember("flipAnimationDuration", _flipAnimationDuration, allocator);
    doc.AddMember("moveAnimationDuration", _moveAnimationDuration, allocator);
    doc.AddMember("scaleAnimationDuration", _scaleAnimationDuration, allocator);
    
    // 主题配置
    doc.AddMember("themeName", rapidjson::StringRef(_themeName.c_str()), allocator);
    doc.AddMember("themeDescription", rapidjson::StringRef(_themeDescription.c_str()), allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    return buffer.GetString();
}

bool CardResConfig::deserialize(const std::string& jsonStr)
{
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    
    if (doc.HasParseError())
    {
        return false;
    }
    
    // 解析尺寸配置
    if (doc.HasMember("cardSize") && doc["cardSize"].IsObject())
    {
        const rapidjson::Value& sizeObj = doc["cardSize"];
        if (sizeObj.HasMember("width") && sizeObj["width"].IsNumber())
            _cardSize.width = sizeObj["width"].GetFloat();
        if (sizeObj.HasMember("height") && sizeObj["height"].IsNumber())
            _cardSize.height = sizeObj["height"].GetFloat();
    }
    
    // 解析其他配置
    if (doc.HasMember("cardCornerRadius") && doc["cardCornerRadius"].IsNumber())
        _cardCornerRadius = doc["cardCornerRadius"].GetFloat();
    
    if (doc.HasMember("cardBorderWidth") && doc["cardBorderWidth"].IsNumber())
        _cardBorderWidth = doc["cardBorderWidth"].GetFloat();
    
    if (doc.HasMember("cardBackTexturePath") && doc["cardBackTexturePath"].IsString())
        _cardBackTexturePath = doc["cardBackTexturePath"].GetString();
    
    if (doc.HasMember("cardFrontTexturePath") && doc["cardFrontTexturePath"].IsString())
        _cardFrontTexturePath = doc["cardFrontTexturePath"].GetString();
    
    if (doc.HasMember("rankFontPath") && doc["rankFontPath"].IsString())
        _rankFontPath = doc["rankFontPath"].GetString();
    
    if (doc.HasMember("rankFontSize") && doc["rankFontSize"].IsInt())
        _rankFontSize = doc["rankFontSize"].GetInt();
    
    if (doc.HasMember("suitFontPath") && doc["suitFontPath"].IsString())
        _suitFontPath = doc["suitFontPath"].GetString();
    
    if (doc.HasMember("suitFontSize") && doc["suitFontSize"].IsInt())
        _suitFontSize = doc["suitFontSize"].GetInt();
    
    if (doc.HasMember("flipAnimationDuration") && doc["flipAnimationDuration"].IsNumber())
        _flipAnimationDuration = doc["flipAnimationDuration"].GetFloat();
    
    if (doc.HasMember("moveAnimationDuration") && doc["moveAnimationDuration"].IsNumber())
        _moveAnimationDuration = doc["moveAnimationDuration"].GetFloat();
    
    if (doc.HasMember("scaleAnimationDuration") && doc["scaleAnimationDuration"].IsNumber())
        _scaleAnimationDuration = doc["scaleAnimationDuration"].GetFloat();
    
    if (doc.HasMember("themeName") && doc["themeName"].IsString())
        _themeName = doc["themeName"].GetString();
    
    if (doc.HasMember("themeDescription") && doc["themeDescription"].IsString())
        _themeDescription = doc["themeDescription"].GetString();
    
    return true;
}

bool CardResConfig::isValid() const
{
    return _cardSize.width > 0 && 
           _cardSize.height > 0 && 
           _cardCornerRadius >= 0 && 
           _cardBorderWidth >= 0 &&
           _rankFontSize > 0 &&
           _suitFontSize > 0 &&
           _flipAnimationDuration > 0 &&
           _moveAnimationDuration > 0 &&
           _scaleAnimationDuration > 0;
}

Color4F CardResConfig::getSuitColor(int suit) const
{
    // 0-1: 红桃、方块 (红色)
    // 2-3: 梅花、黑桃 (黑色)
    return (suit == 0 || suit == 1) ? _redSuitColor : _blackSuitColor;
}

Size CardResConfig::getDisplaySize() const
{
    return _cardSize;
}

void CardResConfig::setDefaultConfig()
{
    _cardSize = Size(80, 120);
    _cardCornerRadius = 8.0f;
    _cardBorderWidth = 2.0f;
    _cardBackTexturePath = "cards/back/card_back.png";
    _cardFrontTexturePath = "cards/front/card_front.png";
    _suitTexturePath = "cards/suits/";
    _rankTexturePath = "cards/ranks/";
    _cardBackColor = Color4F(0.2f, 0.4f, 0.8f, 1.0f);
    _cardFrontColor = Color4F::WHITE;
    _cardBorderColor = Color4F::BLACK;
    _redSuitColor = Color4F::RED;
    _blackSuitColor = Color4F::BLACK;
    _rankFontPath = "fonts/Marker Felt.ttf";
    _rankFontSize = 18;
    _suitFontPath = "fonts/Marker Felt.ttf";
    _suitFontSize = 24;
    _flipAnimationDuration = 0.5f;
    _moveAnimationDuration = 0.3f;
    _scaleAnimationDuration = 0.2f;
    _themeName = "Default";
    _themeDescription = "默认卡牌主题";
}

CardResConfig* CardResConfig::create()
{
    CardResConfig* config = new (std::nothrow) CardResConfig();
    if (config && config->init())
    {
        config->autorelease();
        return config;
    }
    CC_SAFE_DELETE(config);
    return nullptr;
}

std::string CardResConfig::getCardBackTexturePath() const
{
    return _cardBackTexturePath;
}

std::string CardResConfig::getSuitTexturePath() const
{
    return _suitTexturePath;
}

std::string CardResConfig::getRankTexturePath() const
{
    return _rankTexturePath;
}

Color4F CardResConfig::getCardFrontColor() const
{
    return _cardFrontColor;
}

Color4F CardResConfig::getCardBorderColor() const
{
    return _cardBorderColor;
}

Color4F CardResConfig::getCardBackColor() const
{
    return _cardBackColor;
}

std::string CardResConfig::getSuitFontPath() const
{
    return _suitFontPath;
}

int CardResConfig::getSuitFontSize() const
{
    return _suitFontSize;
}

std::string CardResConfig::getRankFontPath() const
{
    return _rankFontPath;
}

int CardResConfig::getRankFontSize() const
{
    return _rankFontSize;
}