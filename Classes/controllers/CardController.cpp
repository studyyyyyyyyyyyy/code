#include "CardController.h"

CardController::CardController()
: _cardModel(nullptr)
, _cardView(nullptr)
, _isInitialized(false)
{
}

CardController::~CardController()
{
    cleanup();
}

bool CardController::init(CardModel* cardModel, CardView* cardView)
{
    if (_isInitialized) return false;
    
    if (!cardModel || !cardView) return false;
    
    _cardModel = cardModel;
    _cardView = cardView;
    
    syncModelToView();
    
    _isInitialized = true;
    return true;
}

bool CardController::handleCardClick()
{
    if (!validateOperation("click")) return false;
    
    logOperation("click", true);
    
    bool newSelected = !_cardModel->_isSelected;
    return selectCard(newSelected);
}

bool CardController::handleCardDragStart()
{
    if (!validateOperation("dragStart")) return false;
    
    logOperation("dragStart", true);
    return true;
}

bool CardController::handleCardDragMove(const Vec2& position)
{
    if (!validateOperation("dragMove")) return false;
    
    _cardModel->setPosition(position);
    syncModelToView();
    
    logOperation("dragMove", true);
    return true;
}

bool CardController::handleCardDragEnd(const Vec2& position)
{
    if (!validateOperation("dragEnd")) return false;
    
    bool success = executeMoveLogic(position);
    
    logOperation("dragEnd", success);
    return success;
}

bool CardController::flipCard(bool animated)
{
    if (!validateOperation("flip")) return false;
    
    bool success = executeFlipLogic();
    
    if (success && _cardView)
    {
        if (animated)
        {
            _cardView->playFlipAnimation();
        }
        else
        {
            _cardView->updateCardDisplay();
        }
    }
    
    logOperation("flip", success);
    return success;
}

bool CardController::moveCard(const Vec2& targetPosition, bool animated)
{
    if (!validateOperation("move")) return false;
    
    bool success = executeMoveLogic(targetPosition);
    
    if (success && _cardView)
    {
        if (animated)
        {
            _cardView->playMoveAnimation(targetPosition);
        }
        else
        {
            _cardView->setPosition(targetPosition);
        }
    }
    
    logOperation("move", success);
    return success;
}

bool CardController::selectCard(bool selected, bool animated)
{
    if (!validateOperation("select")) return false;
    
    bool success = executeSelectLogic(selected);
    
    if (success && _cardView)
    {
        if (animated)
        {
            _cardView->playSelectAnimation(selected);
        }
        else
        {
            _cardView->refreshCardState();
        }
    }
    
    logOperation("select", success);
    return success;
}

bool CardController::setCardVisible(bool visible)
{
    if (!validateOperation("setVisible")) return false;
    
    bool success = executeVisibleLogic(visible);
    
    if (success && _cardView)
    {
        _cardView->setVisible(visible);
    }
    
    logOperation("setVisible", success);
    return success;
}

bool CardController::setCardMovable(bool movable)
{
    if (!validateOperation("setMovable")) return false;
    
    bool success = executeMovableLogic(movable);
    
    if (success && _cardView)
    {
        _cardView->setInteractive(movable);
    }
    
    logOperation("setMovable", success);
    return success;
}

bool CardController::playMatchAnimation()
{
    if (!validateOperation("playMatchAnimation")) return false;
    
    if (_cardView)
    {
        _cardView->playMatchAnimation(_cardModel->_cardId);
    }
    
    logOperation("playMatchAnimation", true);
    return true;
}

bool CardController::playHintAnimation()
{
    if (!validateOperation("playHintAnimation")) return false;
    
    if (_cardView)
    {
        _cardView->playScaleAnimation(1.1f, 0.2f);
    }
    
    logOperation("playHintAnimation", true);
    return true;
}

bool CardController::updateCardState()
{
    if (!_isInitialized) return false;
    
    // 更新渲染状态
    updateRenderState();
    
    // 更新交互状态
    updateInteractionState();
    
    return true;
}

bool CardController::syncModelToView()
{
    if (!_isInitialized || !_cardModel || !_cardView) return false;
    
    // 同步位置
    _cardView->setPosition(_cardModel->_position);
    
    // 同步层级
    _cardView->setLocalZOrder(_cardModel->_zOrder);
    
    // 同步可见性
    _cardView->setVisible(_cardModel->_isVisible);
    
    // 同步交互性
    _cardView->setInteractive(_cardModel->_isMovable);
    
    // 更新显示
    _cardView->updateCardDisplay();
    
    return true;
}

bool CardController::syncViewToModel()
{
    if (!_isInitialized || !_cardModel || !_cardView) return false;
    
    // 同步位置
    _cardModel->setPosition(_cardView->getPosition());
    
    // 同步层级
    _cardModel->setZOrder(_cardView->getLocalZOrder());
    
    // 同步可见性
    _cardModel->_isVisible = _cardView->isVisible();
    
    return true;
}

int CardController::getCardId() const
{
    return _cardModel ? _cardModel->_cardId : 0;
}

CardModel* CardController::getCardModel() const
{
    return _cardModel;
}

CardView* CardController::getCardView() const
{
    return _cardView;
}

void CardController::setCardModel(CardModel* cardModel)
{
    _cardModel = cardModel;
    
    if (_isInitialized)
    {
        syncModelToView();
    }
}

void CardController::setCardView(CardView* cardView)
{
    _cardView = cardView;
    
    if (_isInitialized)
    {
        syncModelToView();
    }
}

bool CardController::isValid() const
{
    return _isInitialized && _cardModel && _cardView;
}

bool CardController::isOperable() const
{
    return isValid() && _cardModel->_isVisible && _cardModel->_isMovable;
}

bool CardController::isSelected() const
{
    return _cardModel ? _cardModel->_isSelected : false;
}

bool CardController::isFaceUp() const
{
    return _cardModel ? _cardModel->_isFaceUp : false;
}

bool CardController::isMovable() const
{
    return _cardModel ? _cardModel->_isMovable : false;
}

bool CardController::isVisible() const
{
    return _cardModel ? _cardModel->_isVisible : false;
}

bool CardController::validateOperation(const std::string& operation)
{
    if (!isValid()) return false;
    
    // 检查卡牌状态
    if (!validateCardState()) return false;
    
    // 检查操作权限
    if (operation == "move" && !_cardModel->_isMovable) return false;
    if (operation == "flip" && !_cardModel->_isVisible) return false;
    
    return true;
}

bool CardController::executeFlipLogic()
{
    if (!_cardModel) return false;
    
    Vec2 oldPosition = _cardModel->_position;
    int oldZOrder = _cardModel->_zOrder;
    
    // 执行翻转
    _cardModel->flipCard();
    
    // 处理状态变化
    handleStateChange("flip", _cardModel->_isFaceUp ? "faceUp" : "faceDown");
    
    return true;
}

bool CardController::executeMoveLogic(const Vec2& targetPosition)
{
    if (!_cardModel) return false;
    
    Vec2 oldPosition = _cardModel->_position;
    
    // 执行移动
    _cardModel->setPosition(targetPosition);
    
    // 处理位置变化
    handlePositionChange(oldPosition, targetPosition);
    
    return true;
}

bool CardController::executeSelectLogic(bool selected)
{
    if (!_cardModel) return false;
    
    bool oldSelected = _cardModel->_isSelected;
    
    // 执行选中
    _cardModel->setSelected(selected);
    
    // 处理状态变化
    handleStateChange(oldSelected ? "selected" : "unselected", selected ? "selected" : "unselected");
    
    return true;
}

bool CardController::executeVisibleLogic(bool visible)
{
    if (!_cardModel) return false;
    
    bool oldVisible = _cardModel->_isVisible;
    
    // 执行可见性设置
    _cardModel->_isVisible = visible;
    
    // 处理状态变化
    handleStateChange(oldVisible ? "visible" : "invisible", visible ? "visible" : "invisible");
    
    return true;
}

bool CardController::executeMovableLogic(bool movable)
{
    if (!_cardModel) return false;
    
    bool oldMovable = _cardModel->_isMovable;
    
    // 执行可移动性设置
    _cardModel->_isMovable = movable;
    
    // 处理状态变化
    handleStateChange(oldMovable ? "movable" : "immovable", movable ? "movable" : "immovable");
    
    return true;
}

bool CardController::updateRenderState()
{
    if (!_cardView) return false;
    
    // 更新卡牌显示
    _cardView->updateCardDisplay();
    
    return true;
}

bool CardController::updateInteractionState()
{
    if (!_cardView) return false;
    
    // 更新交互状态
    _cardView->setInteractive(_cardModel->_isMovable);
    
    return true;
}

void CardController::cleanup()
{
    _cardModel = nullptr;
    _cardView = nullptr;
    _isInitialized = false;
}

bool CardController::validateCardState()
{
    if (!_cardModel) return false;
    
    return _cardModel->isValid();
}

void CardController::handleStateChange(const std::string& oldState, const std::string& newState)
{
    // 这里可以添加状态变化处理逻辑
    // 比如播放音效、触发事件等
}

void CardController::handlePositionChange(const Vec2& oldPosition, const Vec2& newPosition)
{
    // 这里可以添加位置变化处理逻辑
    // 比如检查碰撞、更新层级等
}

void CardController::handleZOrderChange(int oldZOrder, int newZOrder)
{
    // 这里可以添加层级变化处理逻辑
    // 比如更新渲染顺序等
}

std::string CardController::getCardStateDescription() const
{
    if (!_cardModel) return "Invalid";
    
    std::string state = "";
    state += _cardModel->_isFaceUp ? "FaceUp" : "FaceDown";
    state += _cardModel->_isSelected ? "Selected" : "Unselected";
    state += _cardModel->_isMovable ? "Movable" : "Immovable";
    state += _cardModel->_isVisible ? "Visible" : "Invisible";
    
    return state;
}

void CardController::logOperation(const std::string& operation, bool success)
{
}
