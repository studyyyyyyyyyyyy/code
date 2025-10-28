#include "UndoManager.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

UndoManager::UndoManager()
: _gameModel(nullptr)
, _maxUndoCount(50)
, _isInitialized(false)
{
}

UndoManager::~UndoManager()
{
    cleanup();
}

bool UndoManager::init(GameModel* gameModel, int maxUndoCount)
{
    if (_isInitialized) return false;
    
    _gameModel = gameModel;
    _maxUndoCount = maxUndoCount;
    
    if (!validateManagerState())
    {
        return false;
    }
    
    _isInitialized = true;
    return true;
}

bool UndoManager::addUndoAction(UndoModel* undoModel)
{
    if (!_isInitialized || !undoModel) return false;
    
    if (!validateUndoAction(undoModel))
    {
        return false;
    }
    
    // 设置时间戳
    undoModel->_timestamp = getCurrentTimestamp();
    
    // 添加到撤销历史
    _undoHistory.push_back(undoModel);
    
    // 清空重做历史（新操作会清除重做历史）
    clearRedoHistory();
    
    // 处理历史溢出
    handleUndoHistoryOverflow();
    
    logOperation("addUndoAction", true);
    return true;
}

bool UndoManager::undo()
{
    if (!canUndo()) return false;
    
    UndoModel* undoModel = _undoHistory.back();
    _undoHistory.pop_back();
    
    // 执行撤销操作
    bool success = executeUndoAction(undoModel);
    
    if (success)
    {
        // 添加到重做历史
        _redoHistory.push_back(undoModel);
        
        // 处理重做历史溢出
        handleRedoHistoryOverflow();
        
        logOperation("undo", true);
    }
    else
    {
        // 撤销失败，重新添加到撤销历史
        _undoHistory.push_back(undoModel);
        logOperation("undo", false);
    }
    
    return success;
}

UndoModel* UndoManager::getLastUndoAction() const
{
    if (_undoHistory.empty()) return nullptr;
    return _undoHistory.back();
}

bool UndoManager::redo()
{
    if (!canRedo()) return false;
    
    UndoModel* undoModel = _redoHistory.back();
    _redoHistory.pop_back();
    
    // 执行重做操作
    bool success = executeRedoAction(undoModel);
    
    if (success)
    {
        // 重新添加到撤销历史
        _undoHistory.push_back(undoModel);
        
        // 处理撤销历史溢出
        handleUndoHistoryOverflow();
        
        logOperation("redo", true);
    }
    else
    {
        // 重做失败，重新添加到重做历史
        _redoHistory.push_back(undoModel);
        logOperation("redo", false);
    }
    
    return success;
}

bool UndoManager::canUndo() const
{
    return _isInitialized && !_undoHistory.empty();
}

bool UndoManager::canRedo() const
{
    return _isInitialized && !_redoHistory.empty();
}

int UndoManager::getUndoCount() const
{
    return static_cast<int>(_undoHistory.size());
}

int UndoManager::getRedoCount() const
{
    return static_cast<int>(_redoHistory.size());
}

int UndoManager::getMaxUndoCount() const
{
    return _maxUndoCount;
}

void UndoManager::setMaxUndoCount(int maxCount)
{
    _maxUndoCount = maxCount;
    
    // 处理历史溢出
    handleUndoHistoryOverflow();
    handleRedoHistoryOverflow();
}

void UndoManager::clearUndoHistory()
{
    cleanupUndoHistory();
    logOperation("clearUndoHistory", true);
}

void UndoManager::clearRedoHistory()
{
    cleanupRedoHistory();
    logOperation("clearRedoHistory", true);
}

void UndoManager::clearAllHistory()
{
    clearUndoHistory();
    clearRedoHistory();
    logOperation("clearAllHistory", true);
}

size_t UndoManager::getUndoHistorySize() const
{
    return _undoHistory.size();
}

size_t UndoManager::getRedoHistorySize() const
{
    return _redoHistory.size();
}

std::string UndoManager::getNextUndoDescription() const
{
    if (!canUndo()) return "";
    
    return _undoHistory.back()->getActionDescription();
}

std::string UndoManager::getNextRedoDescription() const
{
    if (!canRedo()) return "";
    
    return _redoHistory.back()->getActionDescription();
}

std::string UndoManager::serializeUndoHistory() const
{
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    // 序列化撤销历史
    rapidjson::Value undoArray(rapidjson::kArrayType);
    for (auto undoModel : _undoHistory)
    {
        rapidjson::Value undoObj(rapidjson::kObjectType);
        undoObj.AddMember("actionType", static_cast<int>(undoModel->_actionType), allocator);
        undoObj.AddMember("cardId", undoModel->_cardId, allocator);
        undoObj.AddMember("fromPosition", rapidjson::StringRef(undoModel->_fromPosition.c_str()), allocator);
        undoObj.AddMember("toPosition", rapidjson::StringRef(undoModel->_toPosition.c_str()), allocator);
        undoObj.AddMember("timestamp", undoModel->_timestamp, allocator);
        undoObj.AddMember("moveIndex", undoModel->_moveIndex, allocator);
        undoArray.PushBack(undoObj, allocator);
    }
    doc.AddMember("undoHistory", undoArray, allocator);
    
    // 序列化重做历史
    rapidjson::Value redoArray(rapidjson::kArrayType);
    for (auto undoModel : _redoHistory)
    {
        rapidjson::Value undoObj(rapidjson::kObjectType);
        undoObj.AddMember("actionType", static_cast<int>(undoModel->_actionType), allocator);
        undoObj.AddMember("cardId", undoModel->_cardId, allocator);
        undoObj.AddMember("fromPosition", rapidjson::StringRef(undoModel->_fromPosition.c_str()), allocator);
        undoObj.AddMember("toPosition", rapidjson::StringRef(undoModel->_toPosition.c_str()), allocator);
        undoObj.AddMember("timestamp", undoModel->_timestamp, allocator);
        undoObj.AddMember("moveIndex", undoModel->_moveIndex, allocator);
        redoArray.PushBack(undoObj, allocator);
    }
    doc.AddMember("redoHistory", redoArray, allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    return buffer.GetString();
}

bool UndoManager::deserializeUndoHistory(const std::string& jsonStr)
{
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    
    if (doc.HasParseError())
    {
        return false;
    }
    
    // 清空现有历史
    clearAllHistory();
    
    // 反序列化撤销历史
    if (doc.HasMember("undoHistory") && doc["undoHistory"].IsArray())
    {
        const rapidjson::Value& undoArray = doc["undoHistory"];
        for (rapidjson::SizeType i = 0; i < undoArray.Size(); i++)
        {
            const rapidjson::Value& undoObj = undoArray[i];
            UndoModel* undoModel = new UndoModel();
            
            if (undoObj.HasMember("actionType") && undoObj["actionType"].IsInt())
                undoModel->_actionType = static_cast<UndoModel::ActionType>(undoObj["actionType"].GetInt());
            
            if (undoObj.HasMember("cardId") && undoObj["cardId"].IsInt())
                undoModel->_cardId = undoObj["cardId"].GetInt();
            
            if (undoObj.HasMember("fromPosition") && undoObj["fromPosition"].IsString())
                undoModel->_fromPosition = undoObj["fromPosition"].GetString();
            
            if (undoObj.HasMember("toPosition") && undoObj["toPosition"].IsString())
                undoModel->_toPosition = undoObj["toPosition"].GetString();
            
            if (undoObj.HasMember("timestamp") && undoObj["timestamp"].IsNumber())
                undoModel->_timestamp = undoObj["timestamp"].GetFloat();
            
            if (undoObj.HasMember("moveIndex") && undoObj["moveIndex"].IsInt())
                undoModel->_moveIndex = undoObj["moveIndex"].GetInt();
            
            _undoHistory.push_back(undoModel);
        }
    }
    
    // 反序列化重做历史
    if (doc.HasMember("redoHistory") && doc["redoHistory"].IsArray())
    {
        const rapidjson::Value& redoArray = doc["redoHistory"];
        for (rapidjson::SizeType i = 0; i < redoArray.Size(); i++)
        {
            const rapidjson::Value& undoObj = redoArray[i];
            UndoModel* undoModel = new UndoModel();
            
            if (undoObj.HasMember("actionType") && undoObj["actionType"].IsInt())
                undoModel->_actionType = static_cast<UndoModel::ActionType>(undoObj["actionType"].GetInt());
            
            if (undoObj.HasMember("cardId") && undoObj["cardId"].IsInt())
                undoModel->_cardId = undoObj["cardId"].GetInt();
            
            if (undoObj.HasMember("fromPosition") && undoObj["fromPosition"].IsString())
                undoModel->_fromPosition = undoObj["fromPosition"].GetString();
            
            if (undoObj.HasMember("toPosition") && undoObj["toPosition"].IsString())
                undoModel->_toPosition = undoObj["toPosition"].GetString();
            
            if (undoObj.HasMember("timestamp") && undoObj["timestamp"].IsNumber())
                undoModel->_timestamp = undoObj["timestamp"].GetFloat();
            
            if (undoObj.HasMember("moveIndex") && undoObj["moveIndex"].IsInt())
                undoModel->_moveIndex = undoObj["moveIndex"].GetInt();
            
            _redoHistory.push_back(undoModel);
        }
    }
    
    return true;
}

bool UndoManager::saveUndoHistoryToFile(const std::string& filePath)
{
    std::string jsonStr = serializeUndoHistory();
    if (jsonStr.empty()) return false;
    
    // TODO: 实现文件保存逻辑
    return true;
}

bool UndoManager::loadUndoHistoryFromFile(const std::string& filePath)
{
    // TODO: 实现文件加载逻辑
    return true;
}

void UndoManager::setGameModel(GameModel* gameModel)
{
    _gameModel = gameModel;
}

GameModel* UndoManager::getGameModel() const
{
    return _gameModel;
}

bool UndoManager::isValid() const
{
    return _isInitialized && _gameModel;
}

bool UndoManager::executeUndoAction(UndoModel* undoModel)
{
    if (!undoModel || !_gameModel) return false;
    
    return undoModel->executeUndo(_gameModel);
}

bool UndoManager::executeRedoAction(UndoModel* undoModel)
{
    if (!undoModel || !_gameModel) return false;
    
    return undoModel->executeRedo(_gameModel);
}

bool UndoManager::validateUndoAction(UndoModel* undoModel)
{
    if (!undoModel) return false;
    
    return undoModel->isValid();
}

void UndoManager::handleUndoHistoryOverflow()
{
    while (_undoHistory.size() > static_cast<size_t>(_maxUndoCount))
    {
        UndoModel* undoModel = _undoHistory.front();
        _undoHistory.erase(_undoHistory.begin());
        delete undoModel;
    }
}

void UndoManager::handleRedoHistoryOverflow()
{
    while (_redoHistory.size() > static_cast<size_t>(_maxUndoCount))
    {
        UndoModel* undoModel = _redoHistory.front();
        _redoHistory.erase(_redoHistory.begin());
        delete undoModel;
    }
}

void UndoManager::updateUndoState()
{
    // 状态更新逻辑在具体操作中完成
}

void UndoManager::updateRedoState()
{
    // 状态更新逻辑在具体操作中完成
}

void UndoManager::cleanup()
{
    cleanupUndoHistory();
    cleanupRedoHistory();
    _gameModel = nullptr;
    _isInitialized = false;
}

void UndoManager::cleanupUndoHistory()
{
    for (auto undoModel : _undoHistory)
    {
        delete undoModel;
    }
    _undoHistory.clear();
}

void UndoManager::cleanupRedoHistory()
{
    for (auto undoModel : _redoHistory)
    {
        delete undoModel;
    }
    _redoHistory.clear();
}

bool UndoManager::validateManagerState()
{
    return _gameModel != nullptr && _maxUndoCount > 0;
}

void UndoManager::logOperation(const std::string& operation, bool success)
{
    // 日志功能暂未实现
}

float UndoManager::getCurrentTimestamp() const
{
    return static_cast<float>(time(nullptr));
}
