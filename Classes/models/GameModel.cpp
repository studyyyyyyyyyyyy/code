#include "GameModel.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <algorithm>
#include <random>

GameModel::GameModel()
: _levelId(0)
, _gameId(0)
, _gameState(GAME_STATE_NONE)
, _gameTime(0.0f)
, _score(0)
, _targetScore(1000)
, _maxUndoCount(10)
, _currentUndoCount(0)
, _allowHint(true)
, _allowUndo(true)
, _timeLimit(0)
, _playerTurn(true)
, _playerScore(0)
, _dealerScore(0)
, _playerBusted(false)
, _dealerBusted(false)
, _movesCount(0)
, _hintsUsed(0)
{
}

GameModel::~GameModel()
{
    cleanup();
}

bool GameModel::initGame(int levelId)
{
    _levelId = levelId;
    _gameId = levelId * 1000 + static_cast<int>(time(nullptr));
    _gameState = GAME_STATE_NONE;
    _gameTime = 0.0f;
    _score = 0;
    _currentUndoCount = 0;
    _playerTurn = true;
    _playerScore = 0;
    _dealerScore = 0;
    _playerBusted = false;
    _dealerBusted = false;
    _movesCount = 0;
    _hintsUsed = 0;
    
    // 清理现有卡牌
    cleanup();
    
    // 注意：卡牌创建和洗牌逻辑已移至 GameModelGenerator 管理器层（Manager）
    
    return true;
}

void GameModel::resetGame()
{
    _gameState = GAME_STATE_NONE;
    _gameTime = 0.0f;
    _score = 0;
    _currentUndoCount = 0;
    _playerTurn = true;
    _playerScore = 0;
    _dealerScore = 0;
    _playerBusted = false;
    _dealerBusted = false;
    _movesCount = 0;
    _hintsUsed = 0;
    
    // 重置所有卡牌
    for (auto card : _allCards)
    {
        card->reset();
    }
    
    // 清空手牌
    _playfieldCards.clear();
    _stackCards.clear();
    _handCards.clear();
    
    // 注意：洗牌逻辑已移至 GameRulesService
}

void GameModel::startGame()
{
    _gameState = GAME_STATE_PLAYING;
    _gameTime = 0.0f;
}

void GameModel::pauseGame()
{
    if (_gameState == GAME_STATE_PLAYING)
    {
        _gameState = GAME_STATE_PAUSED;
    }
}

void GameModel::resumeGame()
{
    if (_gameState == GAME_STATE_PAUSED)
    {
        _gameState = GAME_STATE_PLAYING;
    }
}

void GameModel::endGame()
{
    _gameState = GAME_STATE_FINISHED;
}

bool GameModel::isGameFinished() const
{
    return _gameState == GAME_STATE_FINISHED || _gameState == GAME_STATE_GAME_OVER;
}

bool GameModel::isGameWon() const
{
    // 只要分数达到目标分数，就算胜利（通关）
    return (_gameState == GAME_STATE_FINISHED || _gameState == GAME_STATE_GAME_OVER) 
            && _score >= _targetScore;
}

bool GameModel::isGameLost() const
{
    // 游戏结束但分数未达标才算失败
    return (_gameState == GAME_STATE_GAME_OVER || _gameState == GAME_STATE_FINISHED) 
            && _score < _targetScore;
}

CardModel* GameModel::getCardById(int cardId)
{
    for (auto card : _allCards)
    {
        if (card->_cardId == cardId)
        {
            return card;
        }
    }
    return nullptr;
}

bool GameModel::moveCardToArea(int cardId, const std::string& targetArea, int targetIndex)
{
    CardModel* card = getCardById(cardId);
    if (!card) return false;
    
    // 从原区域移除
    removeCardFromArea(cardId, card->_cardArea);
    
    // 添加到目标区域
    card->_cardArea = targetArea;
    if (targetIndex >= 0) {
        card->_areaIndex = targetIndex;
    }
    
    return addCardToArea(card, targetArea, targetIndex);
}

bool GameModel::removeCardFromArea(int cardId, const std::string& area)
{
    std::vector<CardModel*>* targetVector = nullptr;
    
    if (area == "playfield") {
        targetVector = &_playfieldCards;
    } else if (area == "stack") {
        targetVector = &_stackCards;
    } else if (area == "hand") {
        targetVector = &_handCards;
    }
    
    if (!targetVector) return false;
    
    auto it = std::find_if(targetVector->begin(), targetVector->end(),
        [cardId](CardModel* card) { return card->_cardId == cardId; });
    
    if (it != targetVector->end()) {
        targetVector->erase(it);
        return true;
    }
    
    return false;
}

bool GameModel::addCardToArea(CardModel* card, const std::string& area, int index)
{
    if (!card) return false;
    
    if (area == "playfield") {
        if (index >= 0 && static_cast<size_t>(index) <= _playfieldCards.size()) {
            _playfieldCards.insert(_playfieldCards.begin() + index, card);
        } else {
            _playfieldCards.push_back(card);
        }
    } else if (area == "stack") {
        if (index >= 0 && static_cast<size_t>(index) <= _stackCards.size()) {
            _stackCards.insert(_stackCards.begin() + index, card);
        } else {
            _stackCards.push_back(card);
        }
    } else if (area == "hand") {
        if (index >= 0 && static_cast<size_t>(index) <= _handCards.size()) {
            _handCards.insert(_handCards.begin() + index, card);
        } else {
            _handCards.push_back(card);
        }
    } else {
        return false;
    }
    
    return true;
}

CardModel* GameModel::getTopHandCard()
{
    return _handCards.empty() ? nullptr : _handCards.back();
}

void GameModel::updateScore(int deltaScore)
{
    _score += deltaScore;
}

void GameModel::incrementMoves()
{
    _movesCount++;
}

void GameModel::incrementHints()
{
    _hintsUsed++;
}

bool GameModel::canUndo() const
{
    return _allowUndo && _currentUndoCount < _maxUndoCount;
}

bool GameModel::canHint() const
{
    return _allowHint;
}

int GameModel::calculateStars() const
{
    // 根据分数计算星级
    if (_score >= 1800)
    {
        return 3; // 三颗星
    }
    else if (_score > 1400)
    {
        return 2; // 两颗星
    }
    else if (_score >= 1000)
    {
        return 1; // 一颗星
    }
    else
    {
        return 0; // 未通关
    }
}

std::string GameModel::serialize() const
{
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    // 基本信息
    doc.AddMember("levelId", _levelId, allocator);
    doc.AddMember("gameId", _gameId, allocator);
    doc.AddMember("gameState", static_cast<int>(_gameState), allocator);
    doc.AddMember("gameTime", _gameTime, allocator);
    doc.AddMember("score", _score, allocator);
    doc.AddMember("targetScore", _targetScore, allocator);
    
    // 游戏规则
    doc.AddMember("maxUndoCount", _maxUndoCount, allocator);
    doc.AddMember("currentUndoCount", _currentUndoCount, allocator);
    doc.AddMember("allowHint", _allowHint, allocator);
    doc.AddMember("allowUndo", _allowUndo, allocator);
    doc.AddMember("timeLimit", _timeLimit, allocator);
    
    // 玩家状态
    doc.AddMember("playerTurn", _playerTurn, allocator);
    doc.AddMember("playerScore", _playerScore, allocator);
    doc.AddMember("dealerScore", _dealerScore, allocator);
    doc.AddMember("playerBusted", _playerBusted, allocator);
    doc.AddMember("dealerBusted", _dealerBusted, allocator);
    
    // 游戏进度
    doc.AddMember("movesCount", _movesCount, allocator);
    doc.AddMember("hintsUsed", _hintsUsed, allocator);
    
    // 卡牌数据
    rapidjson::Value allCardsArray(rapidjson::kArrayType);
    for (auto card : _allCards)
    {
        rapidjson::Value cardObj(rapidjson::kObjectType);
        cardObj.AddMember("cardId", card->_cardId, allocator);
        cardObj.AddMember("suit", static_cast<int>(card->_suit), allocator);
        cardObj.AddMember("rank", static_cast<int>(card->_rank), allocator);
        cardObj.AddMember("isFaceUp", card->_isFaceUp, allocator);
        cardObj.AddMember("isSelected", card->_isSelected, allocator);
        cardObj.AddMember("isMovable", card->_isMovable, allocator);
        cardObj.AddMember("isVisible", card->_isVisible, allocator);
        
        rapidjson::Value positionObj(rapidjson::kObjectType);
        positionObj.AddMember("x", card->_position.x, allocator);
        positionObj.AddMember("y", card->_position.y, allocator);
        cardObj.AddMember("position", positionObj, allocator);
        
        cardObj.AddMember("zOrder", card->_zOrder, allocator);
        allCardsArray.PushBack(cardObj, allocator);
    }
    doc.AddMember("allCards", allCardsArray, allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    return buffer.GetString();
}

bool GameModel::deserialize(const std::string& jsonStr)
{
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    
    if (doc.HasParseError())
    {
        return false;
    }
    
    // 解析基本信息
    if (doc.HasMember("levelId") && doc["levelId"].IsInt())
        _levelId = doc["levelId"].GetInt();
    
    if (doc.HasMember("gameId") && doc["gameId"].IsInt())
        _gameId = doc["gameId"].GetInt();
    
    if (doc.HasMember("gameState") && doc["gameState"].IsInt())
        _gameState = static_cast<GameState>(doc["gameState"].GetInt());
    
    if (doc.HasMember("gameTime") && doc["gameTime"].IsNumber())
        _gameTime = doc["gameTime"].GetFloat();
    
    if (doc.HasMember("score") && doc["score"].IsInt())
        _score = doc["score"].GetInt();
    
    if (doc.HasMember("targetScore") && doc["targetScore"].IsInt())
        _targetScore = doc["targetScore"].GetInt();
    
    // 解析游戏规则
    if (doc.HasMember("maxUndoCount") && doc["maxUndoCount"].IsInt())
        _maxUndoCount = doc["maxUndoCount"].GetInt();
    
    if (doc.HasMember("currentUndoCount") && doc["currentUndoCount"].IsInt())
        _currentUndoCount = doc["currentUndoCount"].GetInt();
    
    if (doc.HasMember("allowHint") && doc["allowHint"].IsBool())
        _allowHint = doc["allowHint"].GetBool();
    
    if (doc.HasMember("allowUndo") && doc["allowUndo"].IsBool())
        _allowUndo = doc["allowUndo"].GetBool();
    
    if (doc.HasMember("timeLimit") && doc["timeLimit"].IsInt())
        _timeLimit = doc["timeLimit"].GetInt();
    
    // 解析玩家状态
    if (doc.HasMember("playerTurn") && doc["playerTurn"].IsBool())
        _playerTurn = doc["playerTurn"].GetBool();
    
    if (doc.HasMember("playerScore") && doc["playerScore"].IsInt())
        _playerScore = doc["playerScore"].GetInt();
    
    if (doc.HasMember("dealerScore") && doc["dealerScore"].IsInt())
        _dealerScore = doc["dealerScore"].GetInt();
    
    if (doc.HasMember("playerBusted") && doc["playerBusted"].IsBool())
        _playerBusted = doc["playerBusted"].GetBool();
    
    if (doc.HasMember("dealerBusted") && doc["dealerBusted"].IsBool())
        _dealerBusted = doc["dealerBusted"].GetBool();
    
    // 解析游戏进度
    if (doc.HasMember("movesCount") && doc["movesCount"].IsInt())
        _movesCount = doc["movesCount"].GetInt();
    
    if (doc.HasMember("hintsUsed") && doc["hintsUsed"].IsInt())
        _hintsUsed = doc["hintsUsed"].GetInt();
    
    // 解析卡牌数据
    if (doc.HasMember("allCards") && doc["allCards"].IsArray())
    {
        const rapidjson::Value& cardsArray = doc["allCards"];
        for (rapidjson::SizeType i = 0; i < cardsArray.Size(); i++)
        {
            const rapidjson::Value& cardObj = cardsArray[i];
            CardModel* card = new CardModel();
            
            if (cardObj.HasMember("cardId") && cardObj["cardId"].IsInt())
                card->_cardId = cardObj["cardId"].GetInt();
            
            if (cardObj.HasMember("suit") && cardObj["suit"].IsInt())
                card->_suit = static_cast<CardModel::Suit>(cardObj["suit"].GetInt());
            
            if (cardObj.HasMember("rank") && cardObj["rank"].IsInt())
                card->_rank = static_cast<CardModel::Rank>(cardObj["rank"].GetInt());
            
            if (cardObj.HasMember("isFaceUp") && cardObj["isFaceUp"].IsBool())
                card->_isFaceUp = cardObj["isFaceUp"].GetBool();
            
            if (cardObj.HasMember("isSelected") && cardObj["isSelected"].IsBool())
                card->_isSelected = cardObj["isSelected"].GetBool();
            
            if (cardObj.HasMember("isMovable") && cardObj["isMovable"].IsBool())
                card->_isMovable = cardObj["isMovable"].GetBool();
            
            if (cardObj.HasMember("isVisible") && cardObj["isVisible"].IsBool())
                card->_isVisible = cardObj["isVisible"].GetBool();
            
            if (cardObj.HasMember("position") && cardObj["position"].IsObject())
            {
                const rapidjson::Value& posObj = cardObj["position"];
                if (posObj.HasMember("x") && posObj["x"].IsNumber())
                    card->_position.x = posObj["x"].GetFloat();
                if (posObj.HasMember("y") && posObj["y"].IsNumber())
                    card->_position.y = posObj["y"].GetFloat();
            }
            
            if (cardObj.HasMember("zOrder") && cardObj["zOrder"].IsInt())
                card->_zOrder = cardObj["zOrder"].GetInt();
            
            _allCards.push_back(card);
        }
    }
    
    return true;
}

bool GameModel::isValid() const
{
    return _levelId > 0 && 
           _gameId > 0 && 
           _targetScore > 0 && 
           _maxUndoCount >= 0;
}

void GameModel::cleanup()
{
    for (auto card : _allCards)
    {
        delete card;
    }
    _allCards.clear();
    _playerHand.clear();
    _dealerHand.clear();
    _deck.clear();
    _discardPile.clear();
}
