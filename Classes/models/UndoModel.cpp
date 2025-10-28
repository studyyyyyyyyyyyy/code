#include "UndoModel.h"
#include "GameModel.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

UndoModel::UndoModel()
: _actionType(ACTION_NONE)
, _cardId(0)
, _fromPosition("")
, _toPosition("")
, _timestamp(0.0f)
, _moveIndex(0)
, _cardBeforeState(nullptr)
, _positionBefore(Vec2::ZERO)
, _faceUpBefore(false)
, _selectedBefore(false)
, _cardAfterState(nullptr)
, _positionAfter(Vec2::ZERO)
, _faceUpAfter(false)
, _selectedAfter(false)
, _extraData("")
{
}

UndoModel::UndoModel(ActionType actionType, int cardId, const std::string& fromPosition, const std::string& toPosition)
: _actionType(actionType)
, _cardId(cardId)
, _fromPosition(fromPosition)
, _toPosition(toPosition)
, _timestamp(0.0f)
, _moveIndex(0)
, _cardBeforeState(nullptr)
, _positionBefore(Vec2::ZERO)
, _faceUpBefore(false)
, _selectedBefore(false)
, _cardAfterState(nullptr)
, _positionAfter(Vec2::ZERO)
, _faceUpAfter(false)
, _selectedAfter(false)
, _extraData("")
{
}

UndoModel::~UndoModel()
{
    cleanup();
}

void UndoModel::setBeforeState(CardModel* card)
{
    if (!card) return;
    
    _cardBeforeState = createCardStateCopy(card);
    _positionBefore = card->_position;
    _faceUpBefore = card->_isFaceUp;
    _selectedBefore = card->_isSelected;
}

void UndoModel::setAfterState(CardModel* card)
{
    if (!card) return;
    
    _cardAfterState = createCardStateCopy(card);
    _positionAfter = card->_position;
    _faceUpAfter = card->_isFaceUp;
    _selectedAfter = card->_isSelected;
}

std::string UndoModel::getActionDescription() const
{
    switch (_actionType)
    {
        case ACTION_MOVE_CARD:
            return "移动卡牌 " + std::to_string(_cardId) + " 从 " + _fromPosition + " 到 " + _toPosition;
        case ACTION_FLIP_CARD:
            return "翻转卡牌 " + std::to_string(_cardId);
        case ACTION_DEAL_CARD:
            return "发牌到 " + _toPosition;
        case ACTION_SHUFFLE:
            return "洗牌";
        case ACTION_SELECT_CARD:
            return "选中卡牌 " + std::to_string(_cardId);
        case ACTION_DESELECT_CARD:
            return "取消选中卡牌 " + std::to_string(_cardId);
        default:
            return "未知操作";
    }
}

bool UndoModel::canUndo() const
{
    return _actionType != ACTION_NONE && _cardId > 0;
}

bool UndoModel::executeUndo(GameModel* gameModel)
{
    if (!gameModel || !canUndo()) return false;
    
    CardModel* card = gameModel->getCardById(_cardId);
    if (!card) return false;
    
    switch (_actionType)
    {
        case ACTION_MOVE_CARD:
            // 移动卡牌回原区域
            if (gameModel->moveCardToArea(_cardId, _fromPosition))
            {
                // 恢复原始位置和状态
                card->_position = _positionBefore;
                card->_isFaceUp = _faceUpBefore;
                card->_isSelected = _selectedBefore;
                return true;
            }
            return false;
            
        case ACTION_FLIP_CARD:
            card->flipCard();
            return true;
            
        case ACTION_SELECT_CARD:
            card->setSelected(false);
            return true;
            
        case ACTION_DESELECT_CARD:
            card->setSelected(true);
            return true;
            
        default:
            return false;
    }
}

bool UndoModel::executeRedo(GameModel* gameModel)
{
    if (!gameModel || !canUndo()) return false;
    
    CardModel* card = gameModel->getCardById(_cardId);
    if (!card) return false;
    
    switch (_actionType)
    {
        case ACTION_MOVE_CARD:
            return gameModel->moveCardToArea(_cardId, _toPosition);
            
        case ACTION_FLIP_CARD:
            card->flipCard();
            return true;
            
        case ACTION_SELECT_CARD:
            card->setSelected(true);
            return true;
            
        case ACTION_DESELECT_CARD:
            card->setSelected(false);
            return true;
            
        default:
            return false;
    }
}

std::string UndoModel::serialize() const
{
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    doc.AddMember("actionType", static_cast<int>(_actionType), allocator);
    doc.AddMember("cardId", _cardId, allocator);
    doc.AddMember("fromPosition", rapidjson::StringRef(_fromPosition.c_str()), allocator);
    doc.AddMember("toPosition", rapidjson::StringRef(_toPosition.c_str()), allocator);
    doc.AddMember("timestamp", _timestamp, allocator);
    doc.AddMember("moveIndex", _moveIndex, allocator);
    
    // 操作前状态
    rapidjson::Value beforeStateObj(rapidjson::kObjectType);
    beforeStateObj.AddMember("positionX", _positionBefore.x, allocator);
    beforeStateObj.AddMember("positionY", _positionBefore.y, allocator);
    beforeStateObj.AddMember("faceUp", _faceUpBefore, allocator);
    beforeStateObj.AddMember("selected", _selectedBefore, allocator);
    doc.AddMember("beforeState", beforeStateObj, allocator);
    
    // 操作后状态
    rapidjson::Value afterStateObj(rapidjson::kObjectType);
    afterStateObj.AddMember("positionX", _positionAfter.x, allocator);
    afterStateObj.AddMember("positionY", _positionAfter.y, allocator);
    afterStateObj.AddMember("faceUp", _faceUpAfter, allocator);
    afterStateObj.AddMember("selected", _selectedAfter, allocator);
    doc.AddMember("afterState", afterStateObj, allocator);
    
    doc.AddMember("extraData", rapidjson::StringRef(_extraData.c_str()), allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    return buffer.GetString();
}

bool UndoModel::deserialize(const std::string& jsonStr)
{
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    
    if (doc.HasParseError())
    {
        return false;
    }
    
    if (doc.HasMember("actionType") && doc["actionType"].IsInt())
        _actionType = static_cast<ActionType>(doc["actionType"].GetInt());
    
    if (doc.HasMember("cardId") && doc["cardId"].IsInt())
        _cardId = doc["cardId"].GetInt();
    
    if (doc.HasMember("fromPosition") && doc["fromPosition"].IsString())
        _fromPosition = doc["fromPosition"].GetString();
    
    if (doc.HasMember("toPosition") && doc["toPosition"].IsString())
        _toPosition = doc["toPosition"].GetString();
    
    if (doc.HasMember("timestamp") && doc["timestamp"].IsNumber())
        _timestamp = doc["timestamp"].GetFloat();
    
    if (doc.HasMember("moveIndex") && doc["moveIndex"].IsInt())
        _moveIndex = doc["moveIndex"].GetInt();
    
    // 解析操作前状态
    if (doc.HasMember("beforeState") && doc["beforeState"].IsObject())
    {
        const rapidjson::Value& beforeObj = doc["beforeState"];
        if (beforeObj.HasMember("positionX") && beforeObj["positionX"].IsNumber())
            _positionBefore.x = beforeObj["positionX"].GetFloat();
        if (beforeObj.HasMember("positionY") && beforeObj["positionY"].IsNumber())
            _positionBefore.y = beforeObj["positionY"].GetFloat();
        if (beforeObj.HasMember("faceUp") && beforeObj["faceUp"].IsBool())
            _faceUpBefore = beforeObj["faceUp"].GetBool();
        if (beforeObj.HasMember("selected") && beforeObj["selected"].IsBool())
            _selectedBefore = beforeObj["selected"].GetBool();
    }
    
    // 解析操作后状态
    if (doc.HasMember("afterState") && doc["afterState"].IsObject())
    {
        const rapidjson::Value& afterObj = doc["afterState"];
        if (afterObj.HasMember("positionX") && afterObj["positionX"].IsNumber())
            _positionAfter.x = afterObj["positionX"].GetFloat();
        if (afterObj.HasMember("positionY") && afterObj["positionY"].IsNumber())
            _positionAfter.y = afterObj["positionY"].GetFloat();
        if (afterObj.HasMember("faceUp") && afterObj["faceUp"].IsBool())
            _faceUpAfter = afterObj["faceUp"].GetBool();
        if (afterObj.HasMember("selected") && afterObj["selected"].IsBool())
            _selectedAfter = afterObj["selected"].GetBool();
    }
    
    if (doc.HasMember("extraData") && doc["extraData"].IsString())
        _extraData = doc["extraData"].GetString();
    
    return true;
}

bool UndoModel::isValid() const
{
    return _actionType != ACTION_NONE && _cardId > 0;
}

void UndoModel::cleanup()
{
    if (_cardBeforeState)
    {
        delete _cardBeforeState;
        _cardBeforeState = nullptr;
    }
    
    if (_cardAfterState)
    {
        delete _cardAfterState;
        _cardAfterState = nullptr;
    }
}

void UndoModel::copyFrom(const UndoModel& other)
{
    _actionType = other._actionType;
    _cardId = other._cardId;
    _fromPosition = other._fromPosition;
    _toPosition = other._toPosition;
    _timestamp = other._timestamp;
    _moveIndex = other._moveIndex;
    
    _positionBefore = other._positionBefore;
    _faceUpBefore = other._faceUpBefore;
    _selectedBefore = other._selectedBefore;
    
    _positionAfter = other._positionAfter;
    _faceUpAfter = other._faceUpAfter;
    _selectedAfter = other._selectedAfter;
    
    _extraData = other._extraData;
    _metadata = other._metadata;
    
    // 深拷贝卡牌状态
    if (other._cardBeforeState)
    {
        _cardBeforeState = createCardStateCopy(other._cardBeforeState);
    }
    
    if (other._cardAfterState)
    {
        _cardAfterState = createCardStateCopy(other._cardAfterState);
    }
}

bool UndoModel::equals(const UndoModel& other) const
{
    return _actionType == other._actionType &&
           _cardId == other._cardId &&
           _fromPosition == other._fromPosition &&
           _toPosition == other._toPosition;
}

CardModel* UndoModel::createCardStateCopy(CardModel* card)
{
    if (!card) return nullptr;
    
    CardModel* copy = new CardModel();
    copy->copyFrom(*card);
    return copy;
}

void UndoModel::applyCardState(CardModel* targetCard, CardModel* stateCard)
{
    if (!targetCard || !stateCard) return;
    
    targetCard->copyFrom(*stateCard);
}
