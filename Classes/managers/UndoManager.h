#ifndef __UNDO_MANAGER_H__
#define __UNDO_MANAGER_H__

#include "cocos2d.h"
#include "../models/UndoModel.h"
#include "../models/GameModel.h"

using namespace cocos2d;

/**
 * @brief 撤销管理器类
 * 
 * 职责：
 * - 管理游戏操作的撤销和重做功能
 * - 维护操作历史记录
 * - 提供撤销/重做的状态查询
 * - 处理操作数据的生命周期管理
 * 
 * 使用场景：
 * - 游戏操作撤销功能
 * - 操作历史记录管理
 * - 游戏状态回滚
 * - 操作数据持久化
 */
class UndoManager
{
public:
    /**
     * @brief 构造函数
     */
    UndoManager();
    
    /**
     * @brief 析构函数
     */
    ~UndoManager();
    
    /**
     * @brief 初始化撤销管理器
     * @param gameModel 游戏模型
     * @param maxUndoCount 最大撤销次数
     * @return 是否成功
     */
    bool init(GameModel* gameModel, int maxUndoCount = 50);
    
    /**
     * @brief 添加撤销操作
     * @param undoModel 撤销操作模型
     * @return 是否成功
     */
    bool addUndoAction(UndoModel* undoModel);
    
    /**
     * @brief 执行撤销操作
     * @return 是否成功
     */
    bool undo();
    
    /**
     * @brief 执行重做操作
     * @return 是否成功
     */
    bool redo();
    
    /**
     * @brief 检查是否可以撤销
     * @return 是否可以撤销
     */
    bool canUndo() const;
    
    /**
     * @brief 检查是否可以重做
     * @return 是否可以重做
     */
    bool canRedo() const;
    
    /**
     * @brief 获取最后一个撤销操作（不执行）
     * @return 最后一个撤销操作，如果没有则返回nullptr
     */
    UndoModel* getLastUndoAction() const;
    
    /**
     * @brief 获取撤销次数
     * @return 撤销次数
     */
    int getUndoCount() const;
    
    /**
     * @brief 获取重做次数
     * @return 重做次数
     */
    int getRedoCount() const;
    
    /**
     * @brief 获取最大撤销次数
     * @return 最大撤销次数
     */
    int getMaxUndoCount() const;
    
    /**
     * @brief 设置最大撤销次数
     * @param maxCount 最大撤销次数
     */
    void setMaxUndoCount(int maxCount);
    
    /**
     * @brief 清空撤销历史
     */
    void clearUndoHistory();
    
    /**
     * @brief 清空重做历史
     */
    void clearRedoHistory();
    
    /**
     * @brief 清空所有历史
     */
    void clearAllHistory();
    
    /**
     * @brief 获取撤销历史大小
     * @return 撤销历史大小
     */
    size_t getUndoHistorySize() const;
    
    /**
     * @brief 获取重做历史大小
     * @return 重做历史大小
     */
    size_t getRedoHistorySize() const;
    
    /**
     * @brief 获取下一个撤销操作描述
     * @return 操作描述
     */
    std::string getNextUndoDescription() const;
    
    /**
     * @brief 获取下一个重做操作描述
     * @return 操作描述
     */
    std::string getNextRedoDescription() const;
    
    /**
     * @brief 序列化撤销历史
     * @return JSON格式的字符串
     */
    std::string serializeUndoHistory() const;
    
    /**
     * @brief 反序列化撤销历史
     * @param jsonStr JSON格式的字符串
     * @return 是否成功
     */
    bool deserializeUndoHistory(const std::string& jsonStr);
    
    /**
     * @brief 保存撤销历史到文件
     * @param filePath 文件路径
     * @return 是否成功
     */
    bool saveUndoHistoryToFile(const std::string& filePath);
    
    /**
     * @brief 从文件加载撤销历史
     * @param filePath 文件路径
     * @return 是否成功
     */
    bool loadUndoHistoryFromFile(const std::string& filePath);
    
    /**
     * @brief 设置游戏模型
     * @param gameModel 游戏模型
     */
    void setGameModel(GameModel* gameModel);
    
    /**
     * @brief 获取游戏模型
     * @return 游戏模型
     */
    GameModel* getGameModel() const;
    
    /**
     * @brief 检查管理器是否有效
     * @return 是否有效
     */
    bool isValid() const;
    
protected:
    /**
     * @brief 执行撤销操作的具体逻辑
     * @param undoModel 撤销操作模型
     * @return 是否成功
     */
    bool executeUndoAction(UndoModel* undoModel);
    
    /**
     * @brief 执行重做操作的具体逻辑
     * @param undoModel 撤销操作模型
     * @return 是否成功
     */
    bool executeRedoAction(UndoModel* undoModel);
    
    /**
     * @brief 验证撤销操作
     * @param undoModel 撤销操作模型
     * @return 是否有效
     */
    bool validateUndoAction(UndoModel* undoModel);
    
    /**
     * @brief 处理撤销历史溢出
     */
    void handleUndoHistoryOverflow();
    
    /**
     * @brief 处理重做历史溢出
     */
    void handleRedoHistoryOverflow();
    
    /**
     * @brief 更新撤销状态
     */
    void updateUndoState();
    
    /**
     * @brief 更新重做状态
     */
    void updateRedoState();
    
private:
    GameModel* _gameModel;              ///< 游戏模型
    std::vector<UndoModel*> _undoHistory;  ///< 撤销历史
    std::vector<UndoModel*> _redoHistory;  ///< 重做历史
    int _maxUndoCount;                  ///< 最大撤销次数
    bool _isInitialized;                ///< 是否已初始化
    
    /**
     * @brief 清理资源
     */
    void cleanup();
    
    /**
     * @brief 清理撤销历史
     */
    void cleanupUndoHistory();
    
    /**
     * @brief 清理重做历史
     */
    void cleanupRedoHistory();
    
    /**
     * @brief 验证管理器状态
     * @return 是否有效
     */
    bool validateManagerState();
    
    /**
     * @brief 记录操作日志
     * @param operation 操作类型
     * @param success 是否成功
     */
    void logOperation(const std::string& operation, bool success);
    
    /**
     * @brief 获取当前时间戳
     * @return 时间戳
     */
    float getCurrentTimestamp() const;
};

#endif // __UNDO_MANAGER_H__
