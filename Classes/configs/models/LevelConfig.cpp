#include "LevelConfig.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

LevelConfig::LevelConfig()
: _levelId(0)
, _levelName("")
, _levelDescription("")
, _difficulty(1)
, _targetScore(1000)
, _timeLimit(0)
, _initialCardCount(52)
, _maxUndoCount(10)
, _allowHint(true)
, _allowUndo(true)
, _star1Score(500)
, _star2Score(1000)
, _star3Score(1500)
, _requiredLevelId(0)
, _requiredStars(0)
{
}

LevelConfig::~LevelConfig()
{
}

std::string LevelConfig::serialize() const
{
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    doc.AddMember("levelId", _levelId, allocator);
    doc.AddMember("levelName", rapidjson::StringRef(_levelName.c_str()), allocator);
    doc.AddMember("levelDescription", rapidjson::StringRef(_levelDescription.c_str()), allocator);
    doc.AddMember("difficulty", _difficulty, allocator);
    doc.AddMember("targetScore", _targetScore, allocator);
    doc.AddMember("timeLimit", _timeLimit, allocator);
    doc.AddMember("initialCardCount", _initialCardCount, allocator);
    doc.AddMember("maxUndoCount", _maxUndoCount, allocator);
    doc.AddMember("allowHint", _allowHint, allocator);
    doc.AddMember("allowUndo", _allowUndo, allocator);
    doc.AddMember("star1Score", _star1Score, allocator);
    doc.AddMember("star2Score", _star2Score, allocator);
    doc.AddMember("star3Score", _star3Score, allocator);
    doc.AddMember("requiredLevelId", _requiredLevelId, allocator);
    doc.AddMember("requiredStars", _requiredStars, allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    return buffer.GetString();
}

bool LevelConfig::deserialize(const std::string& jsonStr)
{
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    
    if (doc.HasParseError())
    {
        return false;
    }
    
    if (doc.HasMember("levelId") && doc["levelId"].IsInt())
        _levelId = doc["levelId"].GetInt();
    
    if (doc.HasMember("levelName") && doc["levelName"].IsString())
        _levelName = doc["levelName"].GetString();
    
    if (doc.HasMember("levelDescription") && doc["levelDescription"].IsString())
        _levelDescription = doc["levelDescription"].GetString();
    
    if (doc.HasMember("difficulty") && doc["difficulty"].IsInt())
        _difficulty = doc["difficulty"].GetInt();
    
    if (doc.HasMember("targetScore") && doc["targetScore"].IsInt())
        _targetScore = doc["targetScore"].GetInt();
    
    if (doc.HasMember("timeLimit") && doc["timeLimit"].IsInt())
        _timeLimit = doc["timeLimit"].GetInt();
    
    if (doc.HasMember("initialCardCount") && doc["initialCardCount"].IsInt())
        _initialCardCount = doc["initialCardCount"].GetInt();
    
    if (doc.HasMember("maxUndoCount") && doc["maxUndoCount"].IsInt())
        _maxUndoCount = doc["maxUndoCount"].GetInt();
    
    if (doc.HasMember("allowHint") && doc["allowHint"].IsBool())
        _allowHint = doc["allowHint"].GetBool();
    
    if (doc.HasMember("allowUndo") && doc["allowUndo"].IsBool())
        _allowUndo = doc["allowUndo"].GetBool();
    
    if (doc.HasMember("star1Score") && doc["star1Score"].IsInt())
        _star1Score = doc["star1Score"].GetInt();
    
    if (doc.HasMember("star2Score") && doc["star2Score"].IsInt())
        _star2Score = doc["star2Score"].GetInt();
    
    if (doc.HasMember("star3Score") && doc["star3Score"].IsInt())
        _star3Score = doc["star3Score"].GetInt();
    
    if (doc.HasMember("requiredLevelId") && doc["requiredLevelId"].IsInt())
        _requiredLevelId = doc["requiredLevelId"].GetInt();
    
    if (doc.HasMember("requiredStars") && doc["requiredStars"].IsInt())
        _requiredStars = doc["requiredStars"].GetInt();
    
    // 解析主牌区卡牌
    if (doc.HasMember("playfieldCards") && doc["playfieldCards"].IsArray())
    {
        const auto& cardsArray = doc["playfieldCards"];
        for (rapidjson::SizeType i = 0; i < cardsArray.Size(); i++)
        {
            const auto& cardObj = cardsArray[i];
            PlayfieldCard cardData;
            
            if (cardObj.HasMember("cardFace") && cardObj["cardFace"].IsInt())
                cardData.cardFace = cardObj["cardFace"].GetInt();
            
            if (cardObj.HasMember("cardSuit") && cardObj["cardSuit"].IsInt())
                cardData.cardSuit = cardObj["cardSuit"].GetInt();
            
            if (cardObj.HasMember("position") && cardObj["position"].IsObject())
            {
                const auto& pos = cardObj["position"];
                if (pos.HasMember("x") && pos["x"].IsNumber())
                    cardData.position.x = pos["x"].GetFloat();
                if (pos.HasMember("y") && pos["y"].IsNumber())
                    cardData.position.y = pos["y"].GetFloat();
            }
            
            if (cardObj.HasMember("isFaceUp") && cardObj["isFaceUp"].IsBool())
                cardData.isFaceUp = cardObj["isFaceUp"].GetBool();
            
            _playfieldCards.push_back(cardData);
        }
    }
    
    // 解析备用牌堆卡牌
    if (doc.HasMember("stackCards") && doc["stackCards"].IsArray())
    {
        const auto& cardsArray = doc["stackCards"];
        for (rapidjson::SizeType i = 0; i < cardsArray.Size(); i++)
        {
            const auto& cardObj = cardsArray[i];
            StackCard cardData;
            
            if (cardObj.HasMember("cardFace") && cardObj["cardFace"].IsInt())
                cardData.cardFace = cardObj["cardFace"].GetInt();
            
            if (cardObj.HasMember("cardSuit") && cardObj["cardSuit"].IsInt())
                cardData.cardSuit = cardObj["cardSuit"].GetInt();
            
            if (cardObj.HasMember("position") && cardObj["position"].IsObject())
            {
                const auto& pos = cardObj["position"];
                if (pos.HasMember("x") && pos["x"].IsNumber())
                    cardData.position.x = pos["x"].GetFloat();
                if (pos.HasMember("y") && pos["y"].IsNumber())
                    cardData.position.y = pos["y"].GetFloat();
            }
            
            _stackCards.push_back(cardData);
        }
    }
    
    return true;
}

bool LevelConfig::isValid() const
{
    return _levelId > 0 && 
           !_levelName.empty() && 
           _difficulty >= 1 && 
           _difficulty <= 5 && 
           _targetScore > 0 &&
           _initialCardCount > 0 &&
           _maxUndoCount >= 0;
}

std::string LevelConfig::getDisplayName() const
{
    return "关卡 " + std::to_string(_levelId) + ": " + _levelName;
}

bool LevelConfig::isUnlocked(const std::vector<int>& completedLevels) const
{
    if (_requiredLevelId == 0)
    {
        return true; // 没有前置要求
    }
    
    // 检查前置关卡是否已完成
    auto it = std::find(completedLevels.begin(), completedLevels.end(), _requiredLevelId);
    return it != completedLevels.end();
}
