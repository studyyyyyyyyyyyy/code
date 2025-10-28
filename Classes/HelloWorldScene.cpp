#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "configs/loaders/LevelConfigLoader.h"
#include "managers/GameModelGenerator.h"

using namespace cocos2d;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool HelloWorld::init()
{
    if ( !Scene::init() )
    {
        CCLOGERROR("Scene::init() failed");
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto bgGradient = LayerGradient::create(
        Color4B(135, 206, 250, 255),  // 浅天蓝色（顶部）
        Color4B(25, 25, 112, 255)     // 午夜蓝（底部）
    );
    this->addChild(bgGradient, -2);
    
    auto titleLabel = Label::createWithSystemFont("扑克消除", "Arial Bold", 72);
    if (titleLabel)
    {
        titleLabel->setPosition(Vec2(visibleSize.width/2 + origin.x, 
                                     visibleSize.height - 100 + origin.y));
        titleLabel->setColor(Color3B::YELLOW);
        titleLabel->enableOutline(Color4B::BLACK, 4);
        titleLabel->enableShadow(Color4B(0, 0, 0, 180), Size(3, -3));
        this->addChild(titleLabel, 10);
    }
    
    auto helpLabel = Label::createWithSystemFont(
        "消除规则：数字相差1即可匹配\n"
        "左下蓝牌 = 抽新牌 | 右下红框 = 当前底牌",
        "Arial", 28);
    if (helpLabel)
    {
        helpLabel->setPosition(Vec2(visibleSize.width/2 + origin.x, 
                                    visibleSize.height - 220 + origin.y));
        helpLabel->setColor(Color3B::WHITE);
        helpLabel->setAlignment(TextHAlignment::CENTER);
        helpLabel->enableShadow(Color4B(0, 0, 0, 150), Size(2, -2));
        this->addChild(helpLabel, 10);
    }

    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
        CCLOGWARN("Close button images not found, continuing anyway");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
        
        auto menu = Menu::create(closeItem, NULL);
        menu->setPosition(Vec2::ZERO);
        this->addChild(menu, 1);
    }

    _gameController = new GameController();
    
    if (!_gameController->initGame(1))
    {
        CCLOGERROR("========================================");
        CCLOGERROR("Failed to initialize game controller - continuing with demo mode");
        CCLOGERROR("========================================");
        
        auto errorLabel = Label::createWithSystemFont(
            "Game initialization failed!\nCheck console for details.\n\n"
            "Possible issues:\n"
            "- Missing resource files in Resources folder\n"
            "- Missing configs/levels/level_1.json\n"
            "- Missing cards/card_config.json",
            "Arial", 24);
        if (errorLabel)
        {
            errorLabel->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
            errorLabel->setColor(Color3B::RED);
            this->addChild(errorLabel, 1);
        }
        
        delete _gameController;
        _gameController = nullptr;
        return true;
    }
    
    GameView* gameView = _gameController->getGameView();
    if (gameView)
    {
        this->addChild(gameView, 1);
        
        gameView->setCardClickCallback([this](int cardId) {
            GameModel* gameModel = _gameController->getGameModel();
            if (gameModel)
            {
                CardModel* clickedCard = gameModel->getCardById(cardId);
                CardModel* topHandCard = gameModel->getTopHandCard();
                
                if (clickedCard && topHandCard)
                {
                    if (clickedCard->_cardArea == "playfield")
                    {
                        if (!clickedCard->_isFaceUp)
                        {
                            return;
                        }
                        
                        bool isTopCard = true;
                        for (CardModel* otherCard : gameModel->_playfieldCards)
                        {
                            if (otherCard != clickedCard && 
                                otherCard->_position.x == clickedCard->_position.x &&
                                otherCard->_position.y > clickedCard->_position.y)
                            {
                                isTopCard = false;
                                break;
                            }
                        }
                        
                        if (!isTopCard)
                        {
                            return;
                        }
                        
                        _gameController->handleCardMatch(cardId, topHandCard->_cardId);
                    }
                    else if (clickedCard->_cardArea == "stack")
                    {
                        _gameController->handleStackCardFlip(cardId);
                    }
                }
            }
        });
        
        _gameController->startGame();
        this->scheduleUpdate();
    }
    else
    {
        CCLOGERROR("Failed to get game view");
    }
    
    return true;
}

void HelloWorld::update(float delta)
{
    if (_gameController)
    {
        _gameController->updateGame(delta);
    }
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}