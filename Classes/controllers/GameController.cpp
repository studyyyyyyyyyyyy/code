#include "GameController.h"
#include "../configs/loaders/LevelConfigLoader.h"
#include "../managers/UndoManager.h"
#include "../models/UndoModel.h"

GameController::GameController()
: _gameModel(nullptr)
, _gameView(nullptr)
, _resourceManager(nullptr)
, _undoManager(nullptr)
, _levelGenerator(nullptr)
, _levelConfigLoader(nullptr)
, _currentLevelId(0)
, _isInitialized(false)
, _isGameRunning(false)
, _handCardZOrder(100)  // 手牌 zOrder 起始值
{
}

GameController::~GameController()
{
    cleanup();
    CC_SAFE_RELEASE_NULL(_resourceManager);
}

bool GameController::initGame(int levelId)
{
    if (_isInitialized)
    {
        return false;
    }
    
    _resourceManager = ResourceManager::create();
    if (!_resourceManager)
    {
        return false;
    }
    _resourceManager->retain();
    
    if (!_resourceManager->preloadAllResources())
    {
        return false;
    }
    
    _currentLevelId = levelId;
    
    if (!initSubControllers())
    {
        return false;
    }
    
    if (!initGameModel())
    {
        return false;
    }
    
    if (_undoManager && !_undoManager->init(_gameModel, _gameModel->_maxUndoCount))
    {
        return false;
    }
    
    if (!initGameView())
    {
        return false;
    }
    
    setupEventCallbacks();
    
    _isInitialized = true;
    return true;
}

bool GameController::startGame()
{
    if (!_isInitialized || !_gameModel || !_gameView)
    {
        return false;
    }
    
    _gameModel->startGame();
    _isGameRunning = true;
    _handCardZOrder = 100;
    
    updateGameUI();
    
    return true;
}

void GameController::pauseGame()
{
    if (!_isGameRunning || !_gameModel)
    {
        return;
    }
    
    _gameModel->pauseGame();
    _isGameRunning = false;
    
    updateGameUI();
}

void GameController::resumeGame()
{
    if (_isGameRunning || !_gameModel)
    {
        return;
    }
    
    _gameModel->resumeGame();
    _isGameRunning = true;
    
    updateGameUI();
}

void GameController::restartGame()
{
    if (!_isInitialized || !_gameModel)
    {
        return;
    }
    
    _gameModel->resetGame();
    _isGameRunning = false;
    _handCardZOrder = 100;
    
    updateGameUI();
}

void GameController::endGame()
{
    if (!_gameModel)
    {
        return;
    }
    
    _gameModel->endGame();
    _isGameRunning = false;
    
    handleGameEnd();
}

void GameController::exitGame()
{
    if (!_isInitialized)
    {
        return;
    }
    
    endGame();
    cleanup();
}

bool GameController::handleCardClick(int cardId)
{
    if (!_isGameRunning || !_gameModel)
    {
        return false;
    }
    
    CardModel* card = _gameModel->getCardById(cardId);
    if (!card)
    {
        return false;
    }
    
    recordUndoAction(UndoModel::ACTION_SELECT_CARD, cardId, "", "");
    
    bool selected = !card->_isSelected;
    if (processCardSelect(cardId, selected))
    {
        updateGameUI();
        return true;
    }
    
    return false;
}

bool GameController::handleCardDragStart(int cardId)
{
    if (!_isGameRunning || !_gameModel)
    {
        return false;
    }
    
    CardModel* card = _gameModel->getCardById(cardId);
    if (!card || !card->_isMovable)
    {
        return false;
    }
    
    return true;
}

bool GameController::handleCardDragMove(int cardId, const Vec2& position)
{
    if (!_isGameRunning || !_gameModel)
    {
        return false;
    }
    
    CardModel* card = _gameModel->getCardById(cardId);
    if (!card)
    {
        return false;
    }
    
    card->setPosition(position);
    updateGameUI();
    
    return true;
}

bool GameController::handleCardMatch(int playfieldCardId, int handCardId)
{
    if (!_isGameRunning || !_gameModel)
    {
        return false;
    }
    
    recordUndoAction(UndoModel::ACTION_MOVE_CARD, playfieldCardId, "playfield", "hand");
    
    CardModel* playfieldCard = _gameModel->getCardById(playfieldCardId);
    Vec2 originalPos = playfieldCard ? playfieldCard->_position : Vec2::ZERO;
    
    if (GameRulesService::executeCardMatch(_gameModel, playfieldCardId, handCardId))
    {
        CardModel* newHandCard = _gameModel->getTopHandCard();
        if (newHandCard && newHandCard->_cardId == playfieldCardId)
        {
            Vec2 targetPos = Vec2(800, 400);
            newHandCard->_position = targetPos;
            
            if (_gameView)
            {
                _handCardZOrder += 10;
                
                CardView* newCardView = _gameView->getCardView(playfieldCardId);
                if (newCardView)
                {
                    newCardView->updateCardTexture();
                    newCardView->setLocalZOrder(_handCardZOrder);
                    newHandCard->_zOrder = _handCardZOrder;  // 同步 Model zOrder，防止被 updateCardViewZOrders 覆盖
                    
                    newCardView->playMoveAnimation(targetPos, 0.3f);
                }
                
                if (handCardId != playfieldCardId)
                {
                    auto delayAction = DelayTime::create(0.3f);
                    auto hideAction = CallFunc::create([this, handCardId]() {
                        CardView* oldHandCardView = _gameView->getCardView(handCardId);
                        if (oldHandCardView)
                        {
                            oldHandCardView->setVisible(false);
                        }
                    });
                    _gameView->runAction(Sequence::create(delayAction, hideAction, nullptr));
                }
            }
        }
        
        checkAndFlipCardsBelow(originalPos);
        
        if (_gameView)
        {
            _gameView->playCardMatchAnimation(playfieldCardId, handCardId);
        }
        
        updateGameUI();
        
        if (checkGameEndCondition())
        {
            executeGameEndLogic();
        }
        
        return true;
    }
    
    return false;
}

void GameController::checkAndFlipCardsBelow(const Vec2& cardPos)
{
    if (cardPos == Vec2::ZERO)
    {
        return;
    }
    
    CardModel* cardToFlip = nullptr;
    float maxY = 0;
    
    for (CardModel* card : _gameModel->_playfieldCards)
    {
        if (card && 
            card->_position.x == cardPos.x && 
            card->_position.y < cardPos.y &&
            !card->_isFaceUp)
        {
            if (card->_position.y > maxY)
            {
                maxY = card->_position.y;
                cardToFlip = card;
            }
        }
    }
    
    if (cardToFlip)
    {
        cardToFlip->_isFaceUp = true;
        
        if (_gameView)
        {
            CardView* cardView = _gameView->getCardView(cardToFlip->_cardId);
            if (cardView)
            {
                cardView->updateCardDisplay();
            }
        }
    }
}

bool GameController::handleStackCardFlip(int stackCardId)
{
    if (!_isGameRunning || !_gameModel)
    {
        return false;
    }
    
    CardModel* stackCard = _gameModel->getCardById(stackCardId);
    CardModel* topHandCard = _gameModel->getTopHandCard();
    
    if (!stackCard || !topHandCard)
    {
        return false;
    }
    
    recordUndoAction(UndoModel::ACTION_MOVE_CARD, stackCardId, "stack", "hand");
    
    int oldHandCardId = topHandCard->_cardId;
    Vec2 originalPos = stackCard->_position;
    
    if (_gameModel->moveCardToArea(stackCardId, "hand", -1))
    {
        stackCard->_isFaceUp = true;
        Vec2 targetPos = Vec2(800, 400);
        stackCard->_position = targetPos;
        
        if (_gameView)
        {
            _handCardZOrder += 10;  // 递增 zOrder 确保新手牌覆盖旧手牌
            
            CardView* newCardView = _gameView->getCardView(stackCardId);
            if (newCardView)
            {
                newCardView->updateCardTexture();
                newCardView->setLocalZOrder(_handCardZOrder);
                stackCard->_zOrder = _handCardZOrder;  // 同步 Model zOrder，防止被 updateCardViewZOrders 覆盖
                
                newCardView->playMoveAnimation(targetPos, 0.3f);
            }
            
            if (oldHandCardId != stackCardId)
            {
                auto delayAction = DelayTime::create(0.3f);
                auto hideAction = CallFunc::create([this, oldHandCardId]() {
                    CardView* oldHandCardView = _gameView->getCardView(oldHandCardId);
                    if (oldHandCardView)
                    {
                        oldHandCardView->setVisible(false);
                    }
                });
                _gameView->runAction(Sequence::create(delayAction, hideAction, nullptr));
            }
        }
        
        updateGameUI();
        
        if (checkGameEndCondition())
        {
            executeGameEndLogic();
        }
        
        return true;
    }
    
    return false;
}

bool GameController::handleUndo()
{
    if (!_isGameRunning || !_gameModel || !_undoManager)
    {
        return false;
    }
    
    if (!_gameModel->canUndo())
    {
        return false;
    }
    
    UndoModel* undoModel = _undoManager->getLastUndoAction();
    if (!undoModel)
    {
        return false;
    }
    
    if (_undoManager->undo())
    {
        updateViewAfterUndo(undoModel);
        updateGameUI();
        return true;
    }
    
    return false;
}

void GameController::updateViewAfterUndo(UndoModel* undoModel)
{
    CardModel* card = _gameModel->getCardById(undoModel->_cardId);
    if (!card || !_gameView)
    {
        return;
    }
    
    updateHandCardDisplay();
    
    CardView* cardView = _gameView->getCardView(undoModel->_cardId);
    if (cardView)
    {
        Vec2 targetPos = card->_position;
        
        cardView->setVisible(true);
        _handCardZOrder += 10;
        cardView->setLocalZOrder(_handCardZOrder);
        card->_zOrder = _handCardZOrder;
        
        cardView->updateCardTexture();
        
        cardView->playMoveAnimation(targetPos, 0.3f, [this, undoModel, card]() {
            if (undoModel->_fromPosition == "playfield")
            {
                card->_zOrder = 50;
                CardView* view = _gameView->getCardView(card->_cardId);
                if (view)
                {
                    view->setLocalZOrder(50);
                }
            }
            else if (undoModel->_fromPosition == "stack")
            {
                card->_zOrder = 30;
                CardView* view = _gameView->getCardView(card->_cardId);
                if (view)
                {
                    view->setLocalZOrder(30);
                }
            }
        });
    }
    
    if (undoModel->_fromPosition == "playfield" && undoModel->_toPosition == "hand")
    {
        flipBackAutoFlippedCard(card->_position);
    }
}

void GameController::flipBackAutoFlippedCard(const Vec2& cardPos)
{
    CardModel* cardToFlipBack = nullptr;
    float maxY = 0;
    
    for (CardModel* playfieldCard : _gameModel->_playfieldCards)
    {
        if (playfieldCard && 
            playfieldCard->_position.x == cardPos.x && 
            playfieldCard->_position.y < cardPos.y &&
            playfieldCard->_isFaceUp)
        {
            if (playfieldCard->_position.y > maxY)
            {
                maxY = playfieldCard->_position.y;
                cardToFlipBack = playfieldCard;
            }
        }
    }
    
    if (cardToFlipBack)
    {
        cardToFlipBack->_isFaceUp = false;
        
        if (_gameView)
        {
            CardView* cardViewToFlipBack = _gameView->getCardView(cardToFlipBack->_cardId);
            if (cardViewToFlipBack)
            {
                cardViewToFlipBack->updateCardDisplay();
            }
        }
    }
}

void GameController::updateHandCardDisplay()
{
    if (!_gameModel || !_gameView)
    {
        return;
    }
    
    CardModel* currentHandCard = _gameModel->getTopHandCard();
    if (!currentHandCard)
    {
        return;
    }
    
    CardView* handCardView = _gameView->getCardView(currentHandCard->_cardId);
    if (handCardView)
    {
        Vec2 currentPos = handCardView->getPosition();
        Vec2 targetPos = Vec2(800, 400);
        
        handCardView->setVisible(true);
        
        if (currentPos != targetPos)
        {
            _handCardZOrder += 10;
            handCardView->setLocalZOrder(_handCardZOrder);
            currentHandCard->_zOrder = _handCardZOrder;
            
            handCardView->updateCardTexture();
            handCardView->playMoveAnimation(targetPos, 0.3f);
        }
        else
        {
            handCardView->setPosition(targetPos);
            handCardView->setLocalZOrder(_handCardZOrder);
            currentHandCard->_zOrder = _handCardZOrder;
            handCardView->updateCardDisplay();
        }
    }
    
    for (CardModel* handCard : _gameModel->_handCards)
    {
        if (handCard && handCard->_cardId != currentHandCard->_cardId)
        {
            CardView* otherView = _gameView->getCardView(handCard->_cardId);
            if (otherView)
            {
                otherView->setVisible(false);
            }
        }
    }
}

bool GameController::handleHint()
{
    if (!_isGameRunning || !_gameModel || !_gameView)
    {
        return false;
    }
    
    if (!_gameModel->canHint())
    {
        return false;
    }
    
    CardModel* currentHandCard = _gameModel->getTopHandCard();
    if (!currentHandCard)
    {
        return false;
    }
    
    std::vector<int> matchableCards = findMatchableCards(currentHandCard);
    
    if (matchableCards.empty())
    {
        if (!_gameModel->_stackCards.empty())
        {
            CardModel* topStackCard = _gameModel->_stackCards.back();
            if (topStackCard)
            {
                _gameView->showStackHintEffect(topStackCard->_cardId);
                _gameModel->incrementHints();
                return true;
            }
        }
        return false;
    }
    
    showHintAnimation(matchableCards);
    
    _gameModel->incrementHints();
    
    return true;
}

std::vector<int> GameController::findMatchableCards(CardModel* handCard)
{
    std::vector<int> matchableCards;
    
    for (CardModel* playfieldCard : _gameModel->_playfieldCards)
    {
        if (!playfieldCard || !playfieldCard->_isFaceUp) continue;
        
        if (!isTopCardInColumn(playfieldCard)) continue;
        
        if (GameRulesService::canCardsMatch(playfieldCard, handCard))
        {
            matchableCards.push_back(playfieldCard->_cardId);
        }
    }
    
    return matchableCards;
}

bool GameController::isTopCardInColumn(CardModel* card)
{
    for (CardModel* otherCard : _gameModel->_playfieldCards)
    {
        if (otherCard != card && 
            otherCard->_position.x == card->_position.x &&
            otherCard->_position.y > card->_position.y)
        {
            return false;
        }
    }
    return true;
}

void GameController::showHintAnimation(const std::vector<int>& matchableCards)
{
    for (int cardId : matchableCards)
    {
        CardView* cardView = _gameView->getCardView(cardId);
        if (cardView)
        {
            cardView->playSelectAnimation(true);
        }
    }
    
    auto delay = DelayTime::create(1.0f);
    auto callback = CallFunc::create([this, matchableCards]() {
        for (int cardId : matchableCards)
        {
            CardView* cardView = _gameView->getCardView(cardId);
            if (cardView)
            {
                cardView->playSelectAnimation(false);
            }
        }
    });
    _gameView->runAction(Sequence::create(delay, callback, nullptr));
}

void GameController::updateGame(float deltaTime)
{
    if (!_isGameRunning || !_gameModel)
    {
        return;
    }
    
    updateGameTime(deltaTime);
    
    if (checkGameRules())
    {
        executeGameLogic();
    }
    
    if (checkGameEndCondition())
    {
        executeGameEndLogic();
    }
    
    updateGameUI();
}

GameView* GameController::getGameView() const
{
    return _gameView;
}

GameModel* GameController::getGameModel() const
{
    return _gameModel;
}

void GameController::setGameView(GameView* gameView)
{
    _gameView = gameView;
}

void GameController::setGameModel(GameModel* gameModel)
{
    _gameModel = gameModel;
}

bool GameController::saveGameState()
{
    if (!_gameModel)
    {
        return false;
    }
    
    // TODO: 实现游戏状态保存逻辑
    return true;
}

bool GameController::loadGameState()
{
    if (!_gameModel)
    {
        return false;
    }
    
    // TODO: 实现游戏状态加载逻辑
    return true;
}

bool GameController::isGameFinished() const
{
    return _gameModel ? _gameModel->isGameFinished() : false;
}

bool GameController::isGameWon() const
{
    return _gameModel ? _gameModel->isGameWon() : false;
}

bool GameController::isGameLost() const
{
    return _gameModel ? _gameModel->isGameLost() : false;
}

bool GameController::initSubControllers()
{
    _undoManager = new UndoManager();
    if (!_undoManager)
    {
        return false;
    }
    
    _levelGenerator = new GameModelGenerator();
    if (!_levelGenerator)
    {
        return false;
    }
    
    _levelConfigLoader = new LevelConfigLoader();
    if (!_levelConfigLoader)
    {
        return false;
    }
    _levelConfigLoader->setConfigPath("configs/levels/");
    
    return true;
}

bool GameController::initGameView()
{
    if (!_gameModel)
    {
        return false;
    }
    
    _gameView = GameView::create(_gameModel, _resourceManager);
    if (!_gameView)
    {
        return false;
    }
    
    return true;
}

bool GameController::initGameModel()
{
    if (!_levelGenerator)
    {
        return false;
    }
    
    _gameModel = _levelGenerator->generateGameModel(_currentLevelId, _levelConfigLoader);
    if (!_gameModel)
    {
        return false;
    }
    
    return true;
}

void GameController::setupEventCallbacks()
{
    if (!_gameView)
    {
        return;
    }
    
    _gameView->setGameStartCallback([this]() {
        startGame();
    });
    
    _gameView->setGamePauseCallback([this]() {
        pauseGame();
    });
    
    _gameView->setGameResumeCallback([this]() {
        resumeGame();
    });
    
    _gameView->setGameRestartCallback([this]() {
        restartGame();
    });
    
    _gameView->setGameExitCallback([this]() {
        exitGame();
    });
    
    _gameView->setUndoCallback([this]() {
        handleUndo();
    });
    
    _gameView->setHintCallback([this]() {
        handleHint();
    });
    
    _gameView->setCardClickCallback([this](int cardId) {
        handleCardClick(cardId);
    });
}

void GameController::handleGameStateChange()
{
    if (!_gameModel)
    {
        return;
    }
    
    switch (_gameModel->_gameState)
    {
        case GameModel::GAME_STATE_PLAYING:
            _isGameRunning = true;
            break;
        case GameModel::GAME_STATE_PAUSED:
            _isGameRunning = false;
            break;
        case GameModel::GAME_STATE_FINISHED:
            _isGameRunning = false;
            handleGameWin();
            break;
        case GameModel::GAME_STATE_GAME_OVER:
            _isGameRunning = false;
            handleGameLose();
            break;
        default:
            break;
    }
}

void GameController::handleGameWin()
{
    if (!_gameView)
    {
        return;
    }
    
    _gameView->playWinAnimation();
}

void GameController::handleGameLose()
{
    if (!_gameView)
    {
        return;
    }
    
    _gameView->playLoseAnimation();
}

void GameController::handleGameEnd()
{
    if (!_gameModel || !_gameView)
    {
        return;
    }
    
    bool isWon = _gameModel->isGameWon();
    int score = _gameModel->_score;
    int stars = _gameModel->calculateStars();
    
    if (isWon)
    {
        handleGameWin();
    }
    else if (_gameModel->isGameLost())
    {
        handleGameLose();
    }
    
    _gameView->showGameResultWithStars(isWon, score, stars);
}

void GameController::updateGameUI()
{
    if (!_gameView)
    {
        return;
    }
    
    _gameView->updateGameDisplay();
}

bool GameController::checkGameRules()
{
    if (!_gameModel)
    {
        return false;
    }
    
    return true;
}

void GameController::executeGameLogic()
{
    if (!_gameModel)
    {
        return;
    }
}

void GameController::cleanup()
{
    if (_undoManager)
    {
        delete _undoManager;
        _undoManager = nullptr;
    }
    
    if (_levelGenerator)
    {
        delete _levelGenerator;
        _levelGenerator = nullptr;
    }
    
    if (_levelConfigLoader)
    {
        delete _levelConfigLoader;
        _levelConfigLoader = nullptr;
    }
    
    if (_gameModel)
    {
        delete _gameModel;
        _gameModel = nullptr;
    }
    
    if (_gameView)
    {
        _gameView->removeFromParent();
        _gameView = nullptr;
    }
    
    _isInitialized = false;
    _isGameRunning = false;
}

bool GameController::validateGameState()
{
    return _gameModel && _gameView && _isInitialized;
}

bool GameController::processCardMove(int cardId, const std::string& fromPosition, const std::string& toPosition)
{
    if (!_gameModel)
    {
        return false;
    }
    
    recordUndoAction(UndoModel::ACTION_MOVE_CARD, cardId, fromPosition, toPosition);
    
    if (_gameModel->moveCardToArea(cardId, toPosition))
    {
        _gameModel->incrementMoves();
        return true;
    }
    
    return false;
}

bool GameController::processCardFlip(int cardId)
{
    if (!_gameModel)
    {
        return false;
    }
    
    CardModel* card = _gameModel->getCardById(cardId);
    if (!card)
    {
        return false;
    }
    
    recordUndoAction(UndoModel::ACTION_FLIP_CARD, cardId, "", "");
    
    card->flipCard();
    
    return true;
}

bool GameController::processDealCards(int count, const std::string& toPosition)
{
    if (!_gameModel)
    {
        return false;
    }
    
    recordUndoAction(UndoModel::ACTION_DEAL_CARD, 0, "", toPosition);
    
    if (GameRulesService::dealCards(_gameModel, count, toPosition))
    {
        return true;
    }
    
    return false;
}

bool GameController::processShuffle()
{
    if (!_gameModel)
    {
        return false;
    }
    
    recordUndoAction(UndoModel::ACTION_SHUFFLE, 0, "", "");
    
    GameRulesService::shuffleCards(_gameModel->_allCards);
    
    return true;
}

bool GameController::processCardSelect(int cardId, bool selected)
{
    if (!_gameModel)
    {
        return false;
    }
    
    CardModel* card = _gameModel->getCardById(cardId);
    if (!card)
    {
        return false;
    }
    
    card->setSelected(selected);
    
    return true;
}

void GameController::recordUndoAction(int actionType, int cardId, const std::string& fromPosition, const std::string& toPosition)
{
    if (!_undoManager || !_gameModel)
    {
        return;
    }
    
    CardModel* card = _gameModel->getCardById(cardId);
    if (!card)
    {
        return;
    }
    
    UndoModel* undoModel = new UndoModel(
        static_cast<UndoModel::ActionType>(actionType),
        cardId,
        fromPosition,
        toPosition
    );
    
    undoModel->_positionBefore = card->_position;
    undoModel->_faceUpBefore = card->_isFaceUp;
    undoModel->_selectedBefore = card->_isSelected;
    
    if (!_undoManager->addUndoAction(undoModel))
    {
        delete undoModel;
    }
}

void GameController::updateGameScore(int deltaScore)
{
    if (!_gameModel)
    {
        return;
    }
    
    _gameModel->updateScore(deltaScore);
}

void GameController::updateGameTime(float deltaTime)
{
    if (!_gameModel)
    {
        return;
    }
    
    _gameModel->_gameTime += deltaTime;
}

bool GameController::checkGameEndCondition()
{
    if (!_gameModel)
    {
        return false;
    }
    
    if (_gameModel->isGameFinished())
    {
        return true;
    }
    
    // 胜利条件：主牌区所有卡牌都被清除
    if (GameRulesService::checkGameWon(_gameModel))
    {
        _gameModel->_gameState = GameModel::GAME_STATE_FINISHED;
        return true;
    }
    
    // 失败条件：备用牌堆没有牌且主牌区没有可消除的牌
    if (GameRulesService::checkGameLost(_gameModel))
    {
        _gameModel->_gameState = GameModel::GAME_STATE_GAME_OVER;
        return true;
    }
    
    return false;
}

void GameController::executeGameEndLogic()
{
    if (!_gameModel)
    {
        return;
    }
    
    handleGameEnd();
}
