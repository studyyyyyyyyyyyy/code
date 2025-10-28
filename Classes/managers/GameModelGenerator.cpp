#include "GameModelGenerator.h"
#include <random>
#include <algorithm>

GameModelGenerator::GameModelGenerator()
: _randomSeed(0)
{
    resetRandomSeed();
}

GameModelGenerator::~GameModelGenerator()
{
    cleanup();
}

GameModel* GameModelGenerator::generateGameModel(int levelId, LevelConfigLoader* loader)
{
    if (!loader) return nullptr;
    
    LevelConfig* levelConfig = loader->loadLevelConfig(levelId);
    if (!levelConfig) return nullptr;
    
    return generateGameModel(levelConfig);
}

GameModel* GameModelGenerator::generateGameModel(LevelConfig* levelConfig)
{
    if (!levelConfig || !validateLevelConfig(levelConfig)) return nullptr;
    
    GameModel* gameModel = new GameModel();
    if (!gameModel) return nullptr;
    
    if (!initializeGameModel(gameModel, levelConfig))
    {
        delete gameModel;
        return nullptr;
    }
    
    return gameModel;
}

void GameModelGenerator::setRandomSeed(unsigned int seed)
{
    _randomSeed = seed;
}

unsigned int GameModelGenerator::getRandomSeed() const
{
    return _randomSeed;
}

void GameModelGenerator::resetRandomSeed()
{
    _randomSeed = static_cast<unsigned int>(time(nullptr));
}

bool GameModelGenerator::isValid() const
{
    // Manager 始终有效
    return true;
}

bool GameModelGenerator::initializeGameModel(GameModel* gameModel, LevelConfig* levelConfig)
{
    if (!gameModel || !levelConfig) return false;
    
    // 设置基本信息
    gameModel->_levelId = levelConfig->_levelId;
    gameModel->_targetScore = levelConfig->_targetScore;
    gameModel->_timeLimit = levelConfig->_timeLimit;
    gameModel->_maxUndoCount = levelConfig->_maxUndoCount;
    gameModel->_allowHint = levelConfig->_allowHint;
    gameModel->_allowUndo = levelConfig->_allowUndo;
    
    // 创建卡牌集合
    if (!createCardCollection(gameModel, levelConfig))
    {
        return false;
    }
    
    // 设置游戏规则
    if (!setupGameRules(gameModel, levelConfig))
    {
        return false;
    }
    
    // 初始化游戏状态
    if (!initializeGameState(gameModel, levelConfig))
    {
        return false;
    }
    
    return true;
}

bool GameModelGenerator::createCardCollection(GameModel* gameModel, LevelConfig* levelConfig)
{
    if (!gameModel || !levelConfig) return false;
    
    std::vector<CardModel*> deck;
    int cardId = 1;
    
    for (int suit = 0; suit < 4; ++suit)
    {
        for (int rank = 0; rank <= 12; ++rank)
        {
            CardModel* card = new CardModel(
                static_cast<CardModel::Suit>(suit),
                static_cast<CardModel::Rank>(rank),
                cardId++
            );
            deck.push_back(card);
        }
    }
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
    
    size_t deckIndex = 0;
    for (size_t i = 0; i < levelConfig->_playfieldCards.size() && deckIndex < deck.size(); ++i)
    {
        CardModel* card = deck[deckIndex++];
        card->_cardArea = "playfield";
        card->_position = levelConfig->_playfieldCards[i].position;
        card->_isFaceUp = levelConfig->_playfieldCards[i].isFaceUp;
        
        gameModel->_allCards.push_back(card);
        gameModel->_playfieldCards.push_back(card);
    }
    
    int stackIndex = 0;
    while (deckIndex < deck.size())
    {
        CardModel* card = deck[deckIndex++];
        card->_cardArea = "stack";
        card->_position = Vec2(200 + stackIndex * 15, 400); 
        card->_zOrder = stackIndex;
        card->_isFaceUp = false;
        
        gameModel->_allCards.push_back(card);
        gameModel->_stackCards.push_back(card);
        stackIndex++;
    }
    
    if (!gameModel->_stackCards.empty())
    {
        CardModel* topStackCard = gameModel->_stackCards.back();
        gameModel->_stackCards.pop_back();
        
        topStackCard->_cardArea = "hand";
        topStackCard->_position = Vec2(800, 400);
        topStackCard->_isFaceUp = true;
        
        gameModel->_handCards.push_back(topStackCard);
    }
    
    return true;
}

bool GameModelGenerator::setupGameRules(GameModel* gameModel, LevelConfig* levelConfig)
{
    if (!gameModel || !levelConfig) return false;
    
    // 设置游戏规则
    gameModel->_maxUndoCount = levelConfig->_maxUndoCount;
    gameModel->_allowHint = levelConfig->_allowHint;
    gameModel->_allowUndo = levelConfig->_allowUndo;
    
    return true;
}

bool GameModelGenerator::initializeGameState(GameModel* gameModel, LevelConfig* levelConfig)
{
    if (!gameModel || !levelConfig) return false;
    
    // 初始化游戏状态
    gameModel->_gameState = GameModel::GAME_STATE_NONE;
    gameModel->_gameTime = 0.0f;
    gameModel->_score = 0;
    gameModel->_currentUndoCount = 0;
    gameModel->_playerTurn = true;
    gameModel->_playerScore = 0;
    gameModel->_dealerScore = 0;
    gameModel->_playerBusted = false;
    gameModel->_dealerBusted = false;
    gameModel->_movesCount = 0;
    gameModel->_hintsUsed = 0;
    
    return true;
}

std::vector<CardModel*> GameModelGenerator::generateRandomCards(int count)
{
    std::vector<CardModel*> cards;
    
    // 生成标准52张牌
    int cardId = 1;
    for (int suit = 0; suit < 4; ++suit)
    {
        for (int rank = 1; rank <= 13; ++rank)
        {
            if (cardId > count) break;
            
            CardModel* card = new CardModel(
                static_cast<CardModel::Suit>(suit),
                static_cast<CardModel::Rank>(rank),
                cardId++
            );
            cards.push_back(card);
        }
        if (cardId > count) break;
    }
    
    return cards;
}

void GameModelGenerator::shuffleCards(std::vector<CardModel*>& cards)
{
    std::mt19937 g(_randomSeed);
    std::shuffle(cards.begin(), cards.end(), g);
}

bool GameModelGenerator::validateLevelConfig(LevelConfig* levelConfig)
{
    if (!levelConfig) return false;
    
    return levelConfig->isValid();
}

void GameModelGenerator::cleanup()
{
    // 无需清理
}

