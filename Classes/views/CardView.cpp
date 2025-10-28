#include "CardView.h"
#include "SimpleAudioEngine.h"

CardView* CardView::create(const CardModel* cardModel, CardResConfig* resConfig, ResourceManager* resourceManager)
{
    CardView* cardView = new CardView();
    if (cardView && cardView->init(cardModel, resConfig, resourceManager))
    {
        cardView->autorelease();
        return cardView;
    }
    else
    {
        delete cardView;
        cardView = nullptr;
        return nullptr;
    }
}

bool CardView::init(const CardModel* cardModel, CardResConfig* resConfig, ResourceManager* resourceManager)
{
    if (!Sprite::init())
    {
        return false;
    }
    
    _cardModel = cardModel;
    _resConfig = resConfig;
    _resourceManager = resourceManager;
    _isInteractive = true;
    _isDragging = false;
    _isPlayingMoveAnimation = false;
    _dragStartPosition = Vec2::ZERO;
    _originalPosition = Vec2::ZERO;
    
    if (_resConfig)
    {
        this->setContentSize(_resConfig->_cardSize);
    }
    
    updateCardDisplay();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CardView::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CardView::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CardView::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CardView::onTouchCancelled, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

CardView::~CardView()
{
}

void CardView::setCardClickCallback(CardClickCallback callback)
{
    _onCardClickCallback = callback;
}

void CardView::setCardDragStartCallback(CardDragStartCallback callback)
{
    _onCardDragStartCallback = callback;
}

void CardView::setCardDragMoveCallback(CardDragMoveCallback callback)
{
    _onCardDragMoveCallback = callback;
}

void CardView::setCardDragEndCallback(CardDragEndCallback callback)
{
    _onCardDragEndCallback = callback;
}

void CardView::updateCardTexture()
{
    if (!_cardModel || !_resourceManager)
    {
        return;
    }
    
    if (this->getChildrenCount() > 0)
    {
        this->removeAllChildren();
    }
    
    Texture2D* texture = nullptr;
    
    if (_cardModel->_isFaceUp)
    {
        texture = _resourceManager->getCardFrontTexture(
            static_cast<int>(_cardModel->_suit),
            static_cast<int>(_cardModel->_rank)
        );
    }
    else
    {
        texture = _resourceManager->getCardBackTexture();
    }
    
    if (texture)
    {
        this->setTexture(texture);
        
        Size targetSize(120, 160);
        Size textureSize = texture->getContentSize();
        
        float scaleX = targetSize.width / textureSize.width;
        float scaleY = targetSize.height / textureSize.height;
        this->setScale(scaleX, scaleY);
        
        auto background = DrawNode::create();
        background->drawSolidRect(Vec2::ZERO, Vec2(targetSize.width, targetSize.height), 
                                  _cardModel->_isFaceUp ? Color4F::WHITE : Color4F(0.3f, 0.5f, 0.8f, 1.0f));
        this->addChild(background, -1);
        
        auto border = DrawNode::create();
        Color4F borderColor = (_cardModel->_cardArea == "hand") ? Color4F::RED : Color4F::BLACK;
        float borderWidth = (_cardModel->_cardArea == "hand") ? 4 : 2;
        border->drawRect(Vec2::ZERO, Vec2(targetSize.width, targetSize.height), borderColor);
        border->setLineWidth(borderWidth);
        this->addChild(border, 10);
        
        if (_cardModel->_isFaceUp)
        {
            std::string rankText = "";
            std::string suitText = "";
            
            switch(_cardModel->_rank)
            {
                case CardModel::Rank::ACE: rankText = "A"; break;
                case CardModel::Rank::TWO: rankText = "2"; break;
                case CardModel::Rank::THREE: rankText = "3"; break;
                case CardModel::Rank::FOUR: rankText = "4"; break;
                case CardModel::Rank::FIVE: rankText = "5"; break;
                case CardModel::Rank::SIX: rankText = "6"; break;
                case CardModel::Rank::SEVEN: rankText = "7"; break;
                case CardModel::Rank::EIGHT: rankText = "8"; break;
                case CardModel::Rank::NINE: rankText = "9"; break;
                case CardModel::Rank::TEN: rankText = "10"; break;
                case CardModel::Rank::JACK: rankText = "J"; break;
                case CardModel::Rank::QUEEN: rankText = "Q"; break;
                case CardModel::Rank::KING: rankText = "K"; break;
            }
            
            switch(_cardModel->_suit)
            {
                case CardModel::Suit::CLUBS: suitText = "♣"; break;
                case CardModel::Suit::DIAMONDS: suitText = "♦"; break;
                case CardModel::Suit::HEARTS: suitText = "♥"; break;
                case CardModel::Suit::SPADES: suitText = "♠"; break;
            }
            
            auto rankLabel = Label::createWithSystemFont(rankText, "Arial", 24);
            if (rankLabel)
            {
                rankLabel->setPosition(Vec2(15, targetSize.height - 20));
                rankLabel->setColor((_cardModel->_suit == CardModel::Suit::HEARTS || 
                                    _cardModel->_suit == CardModel::Suit::DIAMONDS) ? 
                                   Color3B::RED : Color3B::BLACK);
                rankLabel->setAnchorPoint(Vec2(0, 0.5));
                this->addChild(rankLabel, 5);
            }
            
            auto suitLabel = Label::createWithSystemFont(suitText, "Arial", 36);
            if (suitLabel)
            {
                suitLabel->setPosition(Vec2(targetSize.width/2, targetSize.height/2));
                suitLabel->setColor((_cardModel->_suit == CardModel::Suit::HEARTS || 
                                    _cardModel->_suit == CardModel::Suit::DIAMONDS) ? 
                                   Color3B::RED : Color3B::BLACK);
                this->addChild(suitLabel, 5);
            }
        }
    }
}

void CardView::playFlipAnimation(float duration)
{
    if (!_cardModel)
    {
        return;
    }
    
    auto flipAction = Sequence::create(
        ScaleTo::create(duration * 0.5f, 0.0f, 1.0f),
        CallFunc::create([this]() {
            updateCardTexture();
        }),
        ScaleTo::create(duration * 0.5f, 1.0f, 1.0f),
        nullptr
    );
    
    this->runAction(flipAction);
}

void CardView::updateCardDisplay()
{
    if (!_cardModel)
    {
        return;
    }
    
    updateCardTexture();
    updateCardPosition();
    updateCardZOrder();
}

void CardView::playMoveAnimation(const Vec2& targetPosition, float duration, const std::function<void()>& onComplete)
{
    if (!_cardModel)
    {
        return;
    }
    
    this->stopAllActions();
    
    _isPlayingMoveAnimation = true;
    
    auto moveAction = MoveTo::create(duration, targetPosition);
    auto callback = CallFunc::create([this, onComplete]() {
        _isPlayingMoveAnimation = false;
        if (onComplete)
        {
            onComplete();
        }
    });
    auto sequence = Sequence::create(moveAction, callback, nullptr);
    this->runAction(sequence);
}

void CardView::playScaleAnimation(float targetScale, float duration)
{
    auto scaleAction = ScaleTo::create(duration, targetScale);
    this->runAction(scaleAction);
}

void CardView::playSelectAnimation(bool selected)
{
    if (!_cardModel)
    {
        return;
    }
    
    this->removeChildByName("highlight_border");
    
    if (selected)
    {
        auto highlightBorder = DrawNode::create();
        highlightBorder->setName("highlight_border");
        
        Size cardSize(120, 160);
        float borderWidth = 8;
        
        highlightBorder->drawRect(
            Vec2(-borderWidth/2, -borderWidth/2), 
            Vec2(cardSize.width + borderWidth/2, cardSize.height + borderWidth/2), 
            Color4F::YELLOW
        );
        highlightBorder->setLineWidth(borderWidth);
        this->addChild(highlightBorder, 20);
        
        auto blinkAction = Sequence::create(
            FadeOut::create(0.3f),
            FadeIn::create(0.3f),
            FadeOut::create(0.3f),
            FadeIn::create(0.3f),
            nullptr
        );
        highlightBorder->runAction(blinkAction);
    }
}

void CardView::playMatchAnimation(int cardId)
{
    if (!_cardModel || _cardModel->_cardId != cardId)
    {
        return;
    }
    
    auto blinkAction = Sequence::create(
        FadeOut::create(0.2f),
        FadeIn::create(0.2f),
        FadeOut::create(0.2f),
        FadeIn::create(0.2f),
        nullptr
    );
    
    this->runAction(blinkAction);
}

void CardView::setInteractive(bool interactive)
{
    _isInteractive = interactive;
}

int CardView::getCardId() const
{
    return _cardModel ? _cardModel->_cardId : 0;
}

const CardModel* CardView::getCardModel() const
{
    return _cardModel;
}

void CardView::setCardModel(const CardModel* cardModel)
{
    _cardModel = cardModel;
    updateCardDisplay();
}

void CardView::refreshCardState()
{
    if (!_cardModel)
    {
        return;
    }
    
    if (_cardModel->_isSelected)
    {
        playSelectAnimation(true);
    }
    else
    {
        playSelectAnimation(false);
    }
    
    updateCardPosition();
    updateCardZOrder();
}

bool CardView::onTouchBegan(Touch* touch, Event* event)
{
    if (!_isInteractive || !_cardModel)
    {
        return false;
    }
    
    Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
    Rect rect = Rect(0, 0, this->getContentSize().width, this->getContentSize().height);
    
    if (rect.containsPoint(locationInNode))
    {
        _isDragging = true;
        _dragStartPosition = touch->getLocation();
        _originalPosition = this->getPosition();
        
        if (_onCardDragStartCallback)
        {
            _onCardDragStartCallback(_cardModel->_cardId);
        }
        
        return true;
    }
    
    return false;
}

void CardView::onTouchMoved(Touch* touch, Event* event)
{
    if (!_isDragging || !_cardModel)
    {
        return;
    }
    
    Vec2 currentPosition = touch->getLocation();
    Vec2 deltaPosition = currentPosition - _dragStartPosition;
    Vec2 newPosition = _originalPosition + deltaPosition;
    
    this->setPosition(newPosition);
    
    if (_onCardDragMoveCallback)
    {
        _onCardDragMoveCallback(_cardModel->_cardId, newPosition);
    }
}

void CardView::onTouchEnded(Touch* touch, Event* event)
{
    if (!_isDragging || !_cardModel)
    {
        return;
    }
    
    _isDragging = false;
    
    Vec2 currentPosition = touch->getLocation();
    Vec2 deltaPosition = currentPosition - _dragStartPosition;
    
    if (deltaPosition.length() < 10.0f)
    {
        if (_onCardClickCallback)
        {
            _onCardClickCallback(_cardModel->_cardId);
        }
    }
    else
    {
        if (_onCardDragEndCallback)
        {
            _onCardDragEndCallback(_cardModel->_cardId, this->getPosition());
        }
    }
}

void CardView::onTouchCancelled(Touch* touch, Event* event)
{
    if (!_isDragging || !_cardModel)
    {
        return;
    }
    
    _isDragging = false;
    
    this->setPosition(_originalPosition);
    
    if (_onCardDragEndCallback)
    {
        _onCardDragEndCallback(_cardModel->_cardId, _originalPosition);
    }
}

void CardView::updateCardPosition()
{
    if (!_cardModel)
    {
        return;
    }
    
    this->setPosition(_cardModel->_position);
}

void CardView::updateCardZOrder()
{
    if (!_cardModel)
    {
        return;
    }
    
    this->setLocalZOrder(_cardModel->_zOrder);
}
