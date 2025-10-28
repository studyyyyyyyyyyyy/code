#include "CardModel.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

CardModel::CardModel()
: _cardId(0)
, _suit(HEARTS)
, _rank(ACE)
, _isFaceUp(false)
, _isSelected(false)
, _isMovable(true)
, _isVisible(true)
, _position(Vec2::ZERO)
, _zOrder(0)
, _value(0)
, _cardName("")
{
}

CardModel::CardModel(Suit suit, Rank rank, int cardId)
: _cardId(cardId)
, _suit(suit)
, _rank(rank)
, _isFaceUp(false)
, _isSelected(false)
, _isMovable(true)
, _isVisible(true)
, _position(Vec2::ZERO)
, _zOrder(0)
, _value(0)
, _cardName("")
{
    _value = getValue();
    _cardName = getCardName();
}

CardModel::~CardModel()
{
}

int CardModel::getValue() const
{
    // 21点游戏的点数计算
    switch (_rank)
    {
        case ACE:
            return 11; // A可以算1或11，这里默认11
        case JACK:
        case QUEEN:
        case KING:
            return 10;
        default:
            return static_cast<int>(_rank);
    }
}

std::string CardModel::getCardName() const
{
    return getRankName() + " of " + getSuitName();
}

std::string CardModel::getSuitName() const
{
    switch (_suit)
    {
        case HEARTS:
            return "Hearts";
        case DIAMONDS:
            return "Diamonds";
        case CLUBS:
            return "Clubs";
        case SPADES:
            return "Spades";
        default:
            return "Unknown";
    }
}

std::string CardModel::getRankName() const
{
    switch (_rank)
    {
        case ACE:
            return "Ace";
        case TWO:
            return "Two";
        case THREE:
            return "Three";
        case FOUR:
            return "Four";
        case FIVE:
            return "Five";
        case SIX:
            return "Six";
        case SEVEN:
            return "Seven";
        case EIGHT:
            return "Eight";
        case NINE:
            return "Nine";
        case TEN:
            return "Ten";
        case JACK:
            return "Jack";
        case QUEEN:
            return "Queen";
        case KING:
            return "King";
        default:
            return "Unknown";
    }
}

bool CardModel::isRedSuit() const
{
    return _suit == HEARTS || _suit == DIAMONDS;
}

bool CardModel::isBlackSuit() const
{
    return _suit == CLUBS || _suit == SPADES;
}

void CardModel::flipCard()
{
    _isFaceUp = !_isFaceUp;
}

void CardModel::setSelected(bool selected)
{
    _isSelected = selected;
}

void CardModel::setPosition(const Vec2& position)
{
    _position = position;
}

void CardModel::setZOrder(int zOrder)
{
    _zOrder = zOrder;
}

std::string CardModel::serialize() const
{
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    doc.AddMember("cardId", _cardId, allocator);
    doc.AddMember("suit", static_cast<int>(_suit), allocator);
    doc.AddMember("rank", static_cast<int>(_rank), allocator);
    doc.AddMember("isFaceUp", _isFaceUp, allocator);
    doc.AddMember("isSelected", _isSelected, allocator);
    doc.AddMember("isMovable", _isMovable, allocator);
    doc.AddMember("isVisible", _isVisible, allocator);
    
    rapidjson::Value positionObj(rapidjson::kObjectType);
    positionObj.AddMember("x", _position.x, allocator);
    positionObj.AddMember("y", _position.y, allocator);
    doc.AddMember("position", positionObj, allocator);
    
    doc.AddMember("zOrder", _zOrder, allocator);
    doc.AddMember("value", _value, allocator);
    doc.AddMember("cardName", rapidjson::StringRef(_cardName.c_str()), allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    return buffer.GetString();
}

bool CardModel::deserialize(const std::string& jsonStr)
{
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    
    if (doc.HasParseError())
    {
        return false;
    }
    
    if (doc.HasMember("cardId") && doc["cardId"].IsInt())
        _cardId = doc["cardId"].GetInt();
    
    if (doc.HasMember("suit") && doc["suit"].IsInt())
        _suit = static_cast<Suit>(doc["suit"].GetInt());
    
    if (doc.HasMember("rank") && doc["rank"].IsInt())
        _rank = static_cast<Rank>(doc["rank"].GetInt());
    
    if (doc.HasMember("isFaceUp") && doc["isFaceUp"].IsBool())
        _isFaceUp = doc["isFaceUp"].GetBool();
    
    if (doc.HasMember("isSelected") && doc["isSelected"].IsBool())
        _isSelected = doc["isSelected"].GetBool();
    
    if (doc.HasMember("isMovable") && doc["isMovable"].IsBool())
        _isMovable = doc["isMovable"].GetBool();
    
    if (doc.HasMember("isVisible") && doc["isVisible"].IsBool())
        _isVisible = doc["isVisible"].GetBool();
    
    if (doc.HasMember("position") && doc["position"].IsObject())
    {
        const rapidjson::Value& posObj = doc["position"];
        if (posObj.HasMember("x") && posObj["x"].IsNumber())
            _position.x = posObj["x"].GetFloat();
        if (posObj.HasMember("y") && posObj["y"].IsNumber())
            _position.y = posObj["y"].GetFloat();
    }
    
    if (doc.HasMember("zOrder") && doc["zOrder"].IsInt())
        _zOrder = doc["zOrder"].GetInt();
    
    if (doc.HasMember("value") && doc["value"].IsInt())
        _value = doc["value"].GetInt();
    
    if (doc.HasMember("cardName") && doc["cardName"].IsString())
        _cardName = doc["cardName"].GetString();
    
    return true;
}

bool CardModel::isValid() const
{
    return _cardId > 0 && 
           _suit >= HEARTS && 
           _suit <= SPADES && 
           _rank >= ACE && 
           _rank <= KING;
}

void CardModel::reset()
{
    _isFaceUp = false;
    _isSelected = false;
    _isMovable = true;
    _isVisible = true;
    _position = Vec2::ZERO;
    _zOrder = 0;
}

void CardModel::copyFrom(const CardModel& other)
{
    _cardId = other._cardId;
    _suit = other._suit;
    _rank = other._rank;
    _isFaceUp = other._isFaceUp;
    _isSelected = other._isSelected;
    _isMovable = other._isMovable;
    _isVisible = other._isVisible;
    _position = other._position;
    _zOrder = other._zOrder;
    _value = other._value;
    _cardName = other._cardName;
}

bool CardModel::equals(const CardModel& other) const
{
    return _cardId == other._cardId &&
           _suit == other._suit &&
           _rank == other._rank;
}
