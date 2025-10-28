#include "GameView.h"
#include "../configs/models/CardResConfig.h"

GameView* GameView::create(const GameModel* gameModel, ResourceManager* resourceManager)
{
    GameView* gameView = new GameView();
    if (gameView && gameView->init(gameModel, resourceManager))
    {
        gameView->autorelease();
        return gameView;
    }
    else
    {
        delete gameView;
        gameView = nullptr;
        return nullptr;
    }
}

bool GameView::init(const GameModel* gameModel, ResourceManager* resourceManager)
{
    if (!Layer::init())
    {
        return false;
    }
    
    _gameModel = gameModel;
    _resourceManager = resourceManager;
    
    createUIElements();
    
    if (_gameModel)
    {
        for (CardModel* card : _gameModel->_playfieldCards)
        {
            if (card)
            {
                CardView* cardView = createCardView(card);
                if (cardView)
                {
                    addCardView(cardView);
                }
            }
        }
        
        for (CardModel* card : _gameModel->_handCards)
        {
            if (card)
            {
                CardView* cardView = createCardView(card);
                if (cardView)
                {
                    addCardView(cardView);
                }
            }
        }
        
        for (CardModel* card : _gameModel->_stackCards)
        {
            if (card)
            {
                CardView* cardView = createCardView(card);
                if (cardView)
                {
                    addCardView(cardView);
                }
            }
        }
    }
    
    updateGameDisplay();
    return true;
}

GameView::~GameView()
{
    clearAllCardViews();
}

void GameView::setGameStartCallback(GameStartCallback callback)
{
    _onGameStartCallback = callback;
}

void GameView::setGamePauseCallback(GamePauseCallback callback)
{
    _onGamePauseCallback = callback;
}

void GameView::setGameResumeCallback(GameResumeCallback callback)
{
    _onGameResumeCallback = callback;
}

void GameView::setGameRestartCallback(GameRestartCallback callback)
{
    _onGameRestartCallback = callback;
}

void GameView::setGameExitCallback(GameExitCallback callback)
{
    _onGameExitCallback = callback;
}

void GameView::setUndoCallback(UndoCallback callback)
{
    _onUndoCallback = callback;
}

void GameView::setHintCallback(HintCallback callback)
{
    _onHintCallback = callback;
}

void GameView::setCardClickCallback(CardClickCallback callback)
{
    _onCardClickCallback = callback;
}

void GameView::updateGameDisplay()
{
    if (!_gameModel)
    {
        return;
    }
    
    updateScoreDisplay();
    updateTimeDisplay();
    updateButtonStates();
    updateCardViewPositions();
    updateCardViewZOrders();
}

void GameView::updateScoreDisplay()
{
    if (!_scoreLabel || !_gameModel)
    {
        return;
    }
    
    char scoreText[64];
    sprintf(scoreText, "分数: %d / %d", _gameModel->_score, _gameModel->_targetScore);
    _scoreLabel->setString(scoreText);
    
    if (_levelLabel)
    {
        char levelText[32];
        sprintf(levelText, "关卡: %d", _gameModel->_levelId);
        _levelLabel->setString(levelText);
    }
    
    if (_movesLabel)
    {
        char movesText[32];
        sprintf(movesText, "移动: %d", _gameModel->_movesCount);
        _movesLabel->setString(movesText);
    }
}

void GameView::updateTimeDisplay()
{
    if (!_timeLabel || !_gameModel)
    {
        return;
    }
    
    int minutes = static_cast<int>(_gameModel->_gameTime) / 60;
    int seconds = static_cast<int>(_gameModel->_gameTime) % 60;
    
    char timeStr[32];
    sprintf(timeStr, "时间: %02d:%02d", minutes, seconds);
    _timeLabel->setString(timeStr);
}

void GameView::updateButtonStates()
{
    if (!_gameModel)
    {
        return;
    }
    
    _startButton->setVisible(_gameModel->_gameState == GameModel::GAME_STATE_NONE);
    _pauseButton->setVisible(_gameModel->_gameState == GameModel::GAME_STATE_PLAYING);
    _resumeButton->setVisible(_gameModel->_gameState == GameModel::GAME_STATE_PAUSED);
    _restartButton->setVisible(_gameModel->_gameState == GameModel::GAME_STATE_FINISHED || _gameModel->_gameState == GameModel::GAME_STATE_GAME_OVER);
    _undoButton->setVisible(_gameModel->canUndo());
    _hintButton->setVisible(_gameModel->canHint());
}

void GameView::playMatchAnimation(int cardId)
{
    CardView* cardView = getCardView(cardId);
    if (cardView)
    {
        cardView->playMatchAnimation(cardId);
    }
}

void GameView::playWinAnimation()
{
    auto winAction = Sequence::create(
        ScaleTo::create(0.5f, 1.2f),
        ScaleTo::create(0.5f, 1.0f),
        nullptr
    );
    
    this->runAction(winAction);
    showGameResult(true, _gameModel->_score);
}

void GameView::playLoseAnimation()
{
    auto loseAction = Sequence::create(
        FadeOut::create(0.3f),
        FadeIn::create(0.3f),
        nullptr
    );
    
    this->runAction(loseAction);
    showGameResult(false, _gameModel->_score);
}

void GameView::showHintEffect(const std::vector<int>& cardIds)
{
    hideHintEffect();
    
    for (int cardId : cardIds)
    {
        CardView* cardView = getCardView(cardId);
        if (cardView)
        {
            _hintCardViews.push_back(cardView);
            
            auto hintAction = Sequence::create(
                ScaleTo::create(0.2f, 1.1f),
                ScaleTo::create(0.2f, 1.0f),
                ScaleTo::create(0.2f, 1.1f),
                ScaleTo::create(0.2f, 1.0f),
                nullptr
            );
            
            cardView->runAction(hintAction);
        }
    }
}

void GameView::hideHintEffect()
{
    for (auto cardView : _hintCardViews)
    {
        cardView->setOpacity(255);
        cardView->playSelectAnimation(false);
    }
    _hintCardViews.clear();
}

void GameView::showStackHintEffect(int cardId)
{
    hideHintEffect();
    
    CardView* cardView = getCardView(cardId);
    if (cardView)
    {
        _hintCardViews.push_back(cardView);
        cardView->playSelectAnimation(true);
    }
    
    auto delay = DelayTime::create(1.0f);
    auto callback = CallFunc::create([this]() {
        hideHintEffect();
    });
    this->runAction(Sequence::create(delay, callback, nullptr));
}

void GameView::showGameResult(bool isWin, int score)
{
    if (!_gameResultPanel) return;
    
    _gameResultPanel->setVisible(true);
    
    if (isWin)
    {
        _resultLabel->setString("胜利!");
        _resultLabel->setColor(Color3B::GREEN);
    }
    else
    {
        _resultLabel->setString("失败!");
        _resultLabel->setColor(Color3B::RED);
    }
    
    _finalScoreLabel->setString("最终分数: " + std::to_string(score));
}

void GameView::showGameResultWithStars(bool isWin, int score, int stars)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    this->removeChildByName("game_result_panel_with_stars");
    
    auto resultPanel = LayerColor::create(Color4B(0, 0, 0, 180));
    resultPanel->setName("game_result_panel_with_stars");
    this->addChild(resultPanel, 200);
    
    auto panelBg = LayerColor::create(Color4B(40, 40, 60, 255), 600, 500);
    panelBg->setPosition(Vec2(origin.x + visibleSize.width / 2 - 300, 
                              origin.y + visibleSize.height / 2 - 250));
    resultPanel->addChild(panelBg);
    
    auto border = DrawNode::create();
    border->drawRect(Vec2(0, 0), Vec2(600, 500), Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    border->setLineWidth(4);
    panelBg->addChild(border);
    
    std::string resultText;
    Color3B resultColor;
    if (isWin)
    {
        if (stars > 0)
        {
            resultText = "通关成功!";
            resultColor = Color3B(255, 215, 0);
        }
        else
        {
            resultText = "胜利!";
            resultColor = Color3B::GREEN;
        }
    }
    else
    {
        resultText = "游戏结束";
        resultColor = Color3B::RED;
    }
    
    auto resultLabel = Label::createWithSystemFont(resultText, "Arial Bold", 60);
    resultLabel->setPosition(Vec2(300, 420));
    resultLabel->setColor(resultColor);
    resultLabel->enableOutline(Color4B::BLACK, 3);
    panelBg->addChild(resultLabel);
    
    auto scoreLabel = Label::createWithSystemFont("最终分数: " + std::to_string(score), "Arial", 40);
    scoreLabel->setPosition(Vec2(300, 340));
    scoreLabel->setColor(Color3B::WHITE);
    scoreLabel->enableOutline(Color4B::BLACK, 2);
    panelBg->addChild(scoreLabel);
    
    {
        float starSize = 80.0f;
        float starY = 220;
        float startX = 300 - 110;
        
        for (int i = 0; i < 3; ++i)
        {
            float starX = startX + i * 110;
            
            auto star = DrawNode::create();
            
            Vec2 points[10];
            float outerRadius = starSize / 2;
            float innerRadius = outerRadius * 0.4f;
            
            for (int j = 0; j < 5; ++j)
            {
                float angle1 = -M_PI / 2 + j * 2 * M_PI / 5;
                float angle2 = angle1 + M_PI / 5;
                
                points[j * 2] = Vec2(outerRadius * cos(angle1), outerRadius * sin(angle1));
                points[j * 2 + 1] = Vec2(innerRadius * cos(angle2), innerRadius * sin(angle2));
            }
            
            Color4F starColor = (i < stars) ? 
                Color4F(1.0f, 0.843f, 0.0f, 1.0f) :  // 金色
                Color4F(0.3f, 0.3f, 0.3f, 1.0f);     // 灰色
            
            for (int j = 0; j < 5; ++j)
            {
                int next = (j + 1) % 5;
                Vec2 triangle[3] = {
                    Vec2(0, 0),
                    points[j * 2],
                    points[j * 2 + 1]
                };
                star->drawSolidPoly(triangle, 3, starColor);
                
                Vec2 triangle2[3] = {
                    Vec2(0, 0),
                    points[j * 2 + 1],
                    points[next * 2]
                };
                star->drawSolidPoly(triangle2, 3, starColor);
            }
            
            star->setPosition(Vec2(starX, starY));
            panelBg->addChild(star);
            
            if (i < stars)
            {
                auto scaleUp = ScaleTo::create(0.3f, 1.2f);
                auto scaleDown = ScaleTo::create(0.3f, 1.0f);
                auto delay = DelayTime::create(i * 0.2f);
                auto sequence = Sequence::create(delay, scaleUp, scaleDown, nullptr);
                star->runAction(sequence);
            }
        }
        
        if (stars > 0)
        {
            std::string starText;
            if (stars == 3)
            {
                starText = "完美通关!";
            }
            else if (stars == 2)
            {
                starText = "优秀!";
            }
            else if (stars == 1)
            {
                starText = "通过!";
            }
            
            auto starLabel = Label::createWithSystemFont(starText, "Arial Bold", 36);
            starLabel->setPosition(Vec2(300, 120));
            starLabel->setColor(Color3B(255, 215, 0));
            starLabel->enableOutline(Color4B::BLACK, 2);
            panelBg->addChild(starLabel);
        }
    }
    
    auto continueLabel = Label::createWithSystemFont("点击继续", "Arial", 32);
    continueLabel->setColor(Color3B::WHITE);
    auto continueButton = MenuItemLabel::create(continueLabel, [this, resultPanel](Ref* sender) {
        resultPanel->removeFromParent();
        if (_onGameRestartCallback)
        {
            _onGameRestartCallback();
        }
    });
    continueButton->setPosition(Vec2(300, 50));
    
    auto menu = Menu::create(continueButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    panelBg->addChild(menu);
}

void GameView::hideGameResult()
{
    if (_gameResultPanel)
    {
        _gameResultPanel->setVisible(false);
    }
    
    this->removeChildByName("game_result_panel_with_stars");
}

void GameView::setGameInteractive(bool interactive)
{
    if (_buttonMenu)
    {
        _buttonMenu->setEnabled(interactive);
    }
    
    for (auto& pair : _cardViews)
    {
        pair.second->setInteractive(interactive);
    }
}

CardView* GameView::getCardView(int cardId)
{
    auto it = _cardViews.find(cardId);
    return (it != _cardViews.end()) ? it->second : nullptr;
}

void GameView::addCardView(CardView* cardView)
{
    if (!cardView)
    {
        return;
    }
    
    int cardId = cardView->getCardId();
    _cardViews[cardId] = cardView;
    
    this->addChild(cardView);
}

void GameView::removeCardView(int cardId)
{
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end())
    {
        it->second->removeFromParent();
        _cardViews.erase(it);
    }
}

void GameView::clearAllCardViews()
{
    for (auto& pair : _cardViews)
    {
        pair.second->removeFromParent();
    }
    _cardViews.clear();
}

void GameView::createUIElements()
{
    createButtons();
    createLabels();
    createCardAreas();
    createGameResultPanel();
}

void GameView::createButtons()
{
    _buttonMenu = Menu::create();
    _buttonMenu->setPosition(Vec2::ZERO);
    this->addChild(_buttonMenu);
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float buttonY = 150;
    float buttonSpacing = 250;
    float startX = 150;
    
    auto undoLabel = Label::createWithSystemFont("↶ 撤销", "Arial Bold", 48);
    undoLabel->setColor(Color3B(100, 200, 255));
    undoLabel->enableOutline(Color4B::BLACK, 3);
    undoLabel->enableShadow(Color4B(0, 0, 0, 200), Size(3, -3));
    _undoButton = MenuItemLabel::create(undoLabel, CC_CALLBACK_1(GameView::onUndoButtonClicked, this));
    _undoButton->setPosition(Vec2(startX, buttonY));
    _buttonMenu->addChild(_undoButton);
    
    auto hintLabel = Label::createWithSystemFont("💡 提示", "Arial Bold", 48);
    hintLabel->setColor(Color3B(255, 220, 100));
    hintLabel->enableOutline(Color4B::BLACK, 3);
    hintLabel->enableShadow(Color4B(0, 0, 0, 200), Size(3, -3));
    _hintButton = MenuItemLabel::create(hintLabel, CC_CALLBACK_1(GameView::onHintButtonClicked, this));
    _hintButton->setPosition(Vec2(startX + buttonSpacing, buttonY));
    _buttonMenu->addChild(_hintButton);
    
    auto restartLabel = Label::createWithSystemFont("⟲ 重启", "Arial Bold", 48);
    restartLabel->setColor(Color3B(100, 255, 150));
    restartLabel->enableOutline(Color4B::BLACK, 3);
    restartLabel->enableShadow(Color4B(0, 0, 0, 200), Size(3, -3));
    _restartButton = MenuItemLabel::create(restartLabel, CC_CALLBACK_1(GameView::onRestartButtonClicked, this));
    _restartButton->setPosition(Vec2(startX + buttonSpacing * 2, buttonY));
    _buttonMenu->addChild(_restartButton);
    
    auto exitLabel = Label::createWithSystemFont("✕ 退出", "Arial Bold", 48);
    exitLabel->setColor(Color3B(255, 100, 100));
    exitLabel->enableOutline(Color4B::BLACK, 3);
    exitLabel->enableShadow(Color4B(0, 0, 0, 200), Size(3, -3));
    _exitButton = MenuItemLabel::create(exitLabel, CC_CALLBACK_1(GameView::onExitButtonClicked, this));
    _exitButton->setPosition(Vec2(startX + buttonSpacing * 3, buttonY));
    _buttonMenu->addChild(_exitButton);
    
    _startButton = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(GameView::onStartButtonClicked, this)
    );
    _startButton->setPosition(Vec2(-1000, -1000));
    _startButton->setVisible(false);
    _buttonMenu->addChild(_startButton);
    
    _pauseButton = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(GameView::onPauseButtonClicked, this)
    );
    _pauseButton->setPosition(Vec2(-1000, -1000));
    _pauseButton->setVisible(false);
    _buttonMenu->addChild(_pauseButton);
    
    _resumeButton = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(GameView::onResumeButtonClicked, this)
    );
    _resumeButton->setPosition(Vec2(-1000, -1000));
    _resumeButton->setVisible(false);
    _buttonMenu->addChild(_resumeButton);
}

void GameView::createLabels()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    _scoreLabel = Label::createWithSystemFont("分数: 0 / 1000", "Arial", 32);
    _scoreLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 300));
    _scoreLabel->setColor(Color3B::WHITE);
    _scoreLabel->enableOutline(Color4B::BLACK, 2);
    this->addChild(_scoreLabel, 10);
    
    _timeLabel = Label::createWithSystemFont("时间: 00:00", "Arial", 32);
    _timeLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 350));
    _timeLabel->setColor(Color3B::WHITE);
    _timeLabel->enableOutline(Color4B::BLACK, 2);
    this->addChild(_timeLabel, 10);
    
    _levelLabel = Label::createWithSystemFont("关卡: 1", "Arial", 28);
    _levelLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 400));
    _levelLabel->setColor(Color3B::YELLOW);
    _levelLabel->enableOutline(Color4B::BLACK, 2);
    this->addChild(_levelLabel, 10);
    
    _movesLabel = Label::createWithSystemFont("移动: 0", "Arial", 28);
    _movesLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 440));
    _movesLabel->setColor(Color3B::WHITE);
    _movesLabel->enableOutline(Color4B::BLACK, 2);
    this->addChild(_movesLabel, 10);
}

void GameView::createCardAreas()
{
    _playfieldArea = Layer::create();
    _playfieldArea->setPosition(Vec2(540, 1040));
    this->addChild(_playfieldArea);
    
    _stackArea = Layer::create();
    _stackArea->setPosition(Vec2(540, 290));
    this->addChild(_stackArea);
    
    _handArea = Layer::create();
    _handArea->setPosition(Vec2(540, 290));
    this->addChild(_handArea);
}

void GameView::createGameResultPanel()
{
    _gameResultPanel = Layer::create();
    _gameResultPanel->setVisible(false);
    this->addChild(_gameResultPanel);
    
    auto background = Sprite::create("HelloWorld.png");
    background->setPosition(Vec2(240, 240));
    background->setScale(2.0f);
    _gameResultPanel->addChild(background);
    
    _resultLabel = Label::createWithTTF("", "fonts/Marker Felt.ttf", 48);
    _resultLabel->setPosition(Vec2(240, 280));
    _gameResultPanel->addChild(_resultLabel);
    
    _finalScoreLabel = Label::createWithTTF("", "fonts/Marker Felt.ttf", 24);
    _finalScoreLabel->setPosition(Vec2(240, 200));
    _gameResultPanel->addChild(_finalScoreLabel);
}

void GameView::onStartButtonClicked(Ref* sender)
{
    if (_onGameStartCallback)
    {
        _onGameStartCallback();
    }
}

void GameView::onPauseButtonClicked(Ref* sender)
{
    if (_onGamePauseCallback)
    {
        _onGamePauseCallback();
    }
}

void GameView::onResumeButtonClicked(Ref* sender)
{
    if (_onGameResumeCallback)
    {
        _onGameResumeCallback();
    }
}

void GameView::onRestartButtonClicked(Ref* sender)
{
    if (_onGameRestartCallback)
    {
        _onGameRestartCallback();
    }
}

void GameView::onExitButtonClicked(Ref* sender)
{
    if (_onGameExitCallback)
    {
        _onGameExitCallback();
    }
    else
    {
        Director::getInstance()->end();
    }
}

void GameView::onUndoButtonClicked(Ref* sender)
{
    if (_onUndoCallback)
    {
        _onUndoCallback();
    }
}

void GameView::onHintButtonClicked(Ref* sender)
{
    if (_onHintCallback)
    {
        _onHintCallback();
    }
}

void GameView::updateCardViewPositions()
{
    if (!_gameModel)
    {
        return;
    }
    
    for (size_t i = 0; i < _gameModel->_playfieldCards.size(); ++i)
    {
        CardModel* card = _gameModel->_playfieldCards[i];
        CardView* cardView = getCardView(card->_cardId);
        if (cardView)
        {
            if (cardView->isPlayingMoveAnimation())
            {
                continue;
            }
            Vec2 position = Vec2(card->_position.x, card->_position.y);
            cardView->setPosition(position);
        }
    }
    
    for (size_t i = 0; i < _gameModel->_stackCards.size(); ++i)
    {
        CardModel* card = _gameModel->_stackCards[i];
        CardView* cardView = getCardView(card->_cardId);
        if (cardView)
        {
            if (cardView->isPlayingMoveAnimation())
            {
                continue;
            }
            cardView->setPosition(card->_position);
        }
    }
    
    for (size_t i = 0; i < _gameModel->_handCards.size(); ++i)
    {
        CardModel* card = _gameModel->_handCards[i];
        CardView* cardView = getCardView(card->_cardId);
        if (cardView)
        {
            if (cardView->isPlayingMoveAnimation())
            {
                continue;
            }
            cardView->setPosition(card->_position);
        }
    }
}

void GameView::updateCardViewZOrders()
{
    if (!_gameModel)
    {
        return;
    }
    
    for (auto card : _gameModel->_allCards)
    {
        CardView* cardView = getCardView(card->_cardId);
        if (cardView)
        {
            cardView->setLocalZOrder(card->_zOrder);
        }
    }
}

CardView* GameView::createCardView(CardModel* cardModel)
{
    if (!cardModel)
    {
        return nullptr;
    }
    
    CardResConfig* resConfig = new CardResConfig();
    resConfig->setDefaultConfig();
    
    CardView* cardView = CardView::create(cardModel, resConfig, _resourceManager);
    
    cardView->setCardClickCallback([this](int cardId) {
        if (_onCardClickCallback)
        {
            _onCardClickCallback(cardId);
        }
    });
    
    return cardView;
}

void GameView::layoutCardAreas()
{
}

Vec2 GameView::getCardAreaPosition(const std::string& areaName)
{
    if (areaName == "playfield")
        return _playfieldArea->getPosition();
    else if (areaName == "stack")
        return _stackArea->getPosition();
    else if (areaName == "hand")
        return _handArea->getPosition();
    
    return Vec2::ZERO;
}

void GameView::playCardMoveAnimation(int cardId, const Vec2& targetPosition, float duration)
{
    CardView* cardView = getCardView(cardId);
    if (cardView)
    {
        cardView->playMoveAnimation(targetPosition, duration);
    }
}

void GameView::playCardMatchAnimation(int cardId1, int cardId2)
{
    CardView* cardView1 = getCardView(cardId1);
    CardView* cardView2 = getCardView(cardId2);
    
    if (cardView1)
    {
        cardView1->playMatchAnimation(cardId1);
    }
    
    if (cardView2)
    {
        cardView2->playMatchAnimation(cardId2);
    }
}
