#include "ResourceManager.h"
#include "../configs/models/CardResConfig.h"
#include "../utils/CardGameUtils.h"
#include "../models/CardModel.h"

using namespace cocos2d;

ResourceManager* ResourceManager::create()
{
    ResourceManager* ret = new (std::nothrow) ResourceManager();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}

bool ResourceManager::init()
{
    if (_isInitialized) return true;
    
    // 初始化卡牌资源配置
    _cardResConfig = CardResConfig::create();
    if (!_cardResConfig)
    {
        CCLOGERROR("Failed to create CardResConfig");
        return false;
    }
    
    // 加载卡牌资源配置
    if (!loadCardResConfig("cards/card_config.json"))
    {
        CCLOGWARN("Failed to load card config, using default config");
        _cardResConfig->setDefaultConfig();
    }
    
    _isInitialized = true;
    return true;
}

bool ResourceManager::preloadAllResources()
{
    bool success = true;
    
    // 预加载卡牌资源 - 允许失败，使用默认渲染
    if (!preloadCardResources())
    {
        // 不设置 success = false，允许继续
    }
    
    // 预加载字体资源 - 允许失败，使用系统字体
    if (!preloadFontResources())
    {
        // 不设置 success = false，允许继续
    }
    
    // 预加载音效资源 - 允许失败，游戏可以无声运行
    if (!preloadAudioResources())
    {
        // 不设置 success = false，允许继续
    }
    
    return success;
}

bool ResourceManager::preloadCardResources()
{
    if (!_cardResConfig) return false;
    
    // 预加载卡牌背面纹理
    Texture2D* cardBackTexture = getCardBackTexture();
    if (!cardBackTexture) return false;
    
    // 预加载所有花色和点数的卡牌纹理
    for (int suit = 0; suit < 4; ++suit)
    {
        for (int rank = 0; rank < 13; ++rank)
        {
            Texture2D* cardTexture = getCardFrontTexture(suit, rank);
            if (!cardTexture) return false;
        }
    }
    
    return true;
}

bool ResourceManager::preloadFontResources()
{
    // 预加载默认字体
    Font* defaultFont = getFont("fonts/Marker Felt.ttf");
    if (!defaultFont)
    {
        CCLOGERROR("Failed to preload default font");
        return false;
    }
    
    // 预加载Arial字体
    Font* arialFont = getFont("fonts/arial.ttf");
    if (!arialFont)
    {
        CCLOGERROR("Failed to preload Arial font");
        return false;
    }
    
    return true;
}

bool ResourceManager::preloadAudioResources()
{
    // 这里可以预加载音效文件
    // 由于当前项目没有音效文件，暂时返回true
    return true;
}

CardResConfig* ResourceManager::getCardResConfig()
{
    return _cardResConfig;
}

bool ResourceManager::loadCardResConfig(const std::string& configPath)
{
    if (!_cardResConfig) return false;
    
    std::string fullPath = getResourcePath(configPath);
    if (!isResourceExists(fullPath))
    {
        CCLOGWARN("Card config file not found: %s", fullPath.c_str());
        return false;
    }
    
    // 这里可以解析JSON配置文件
    // 暂时使用默认配置
    _cardResConfig->setDefaultConfig();
    
    return true;
}

Texture2D* ResourceManager::getCardBackTexture()
{
    std::string cacheKey = "card_back";
    
    // 检查缓存
    auto it = _textureCache.find(cacheKey);
    if (it != _textureCache.end())
    {
        return it->second;
    }
    
    // 尝试加载纹理文件
    std::string texturePath = _cardResConfig->getCardBackTexturePath();
    Texture2D* texture = loadTexture(texturePath);
    
    if (!texture)
    {
        // 如果加载失败，创建默认的卡牌背面纹理
        texture = createCardBackTexture();
    }
    
    if (texture)
    {
        _textureCache[cacheKey] = texture;
    }
    
    return texture;
}

Texture2D* ResourceManager::getCardFrontTexture(int suit, int rank)
{
    std::string cacheKey = StringUtils::format("card_%d_%d", suit, rank);
    
    // 检查缓存
    auto it = _textureCache.find(cacheKey);
    if (it != _textureCache.end())
    {
        return it->second;
    }
    
    // 尝试加载纹理文件
    std::string texturePath = StringUtils::format("%s%d_%d.png", 
        _cardResConfig->getSuitTexturePath().c_str(), suit, rank);
    Texture2D* texture = loadTexture(texturePath);
    
    if (!texture)
    {
        // 如果加载失败，创建默认的卡牌正面纹理
        texture = createCardTexture(suit, rank);
    }
    
    if (texture)
    {
        _textureCache[cacheKey] = texture;
    }
    
    return texture;
}

Texture2D* ResourceManager::getSuitTexture(int suit)
{
    std::string cacheKey = StringUtils::format("suit_%d", suit);
    
    // 检查缓存
    auto it = _textureCache.find(cacheKey);
    if (it != _textureCache.end())
    {
        return it->second;
    }
    
    // 尝试加载纹理文件
    std::string texturePath = StringUtils::format("%ssuit_%d.png", 
        _cardResConfig->getSuitTexturePath().c_str(), suit);
    Texture2D* texture = loadTexture(texturePath);
    
    if (texture)
    {
        _textureCache[cacheKey] = texture;
    }
    
    return texture;
}

Texture2D* ResourceManager::getRankTexture(int rank)
{
    std::string cacheKey = StringUtils::format("rank_%d", rank);
    
    // 检查缓存
    auto it = _textureCache.find(cacheKey);
    if (it != _textureCache.end())
    {
        return it->second;
    }
    
    // 尝试加载纹理文件
    std::string texturePath = StringUtils::format("%srank_%d.png", 
        _cardResConfig->getRankTexturePath().c_str(), rank);
    Texture2D* texture = loadTexture(texturePath);
    
    if (texture)
    {
        _textureCache[cacheKey] = texture;
    }
    
    return texture;
}

Font* ResourceManager::getFont(const std::string& fontPath)
{
    std::string cacheKey = getCacheKey(fontPath);
    
    // 检查缓存
    auto it = _fontCache.find(cacheKey);
    if (it != _fontCache.end())
    {
        return it->second;
    }
    
    // 加载字体
    Font* font = loadFont(fontPath);
    if (font)
    {
        _fontCache[cacheKey] = font;
    }
    
    return font;
}

bool ResourceManager::isResourceExists(const std::string& resourcePath)
{
    return FileUtils::getInstance()->isFileExist(resourcePath);
}

std::string ResourceManager::getResourcePath(const std::string& resourcePath)
{
    // 如果路径已经是完整路径，直接返回
    if (resourcePath.find("/") == 0 || resourcePath.find("\\") == 0)
    {
        return resourcePath;
    }
    
    // 否则，相对于Resources目录
    return "Resources/" + resourcePath;
}

void ResourceManager::clearResourceCache()
{
    // 清理纹理缓存
    for (auto& pair : _textureCache)
    {
        CC_SAFE_RELEASE(pair.second);
    }
    _textureCache.clear();
    
    // 清理字体缓存
    for (auto& pair : _fontCache)
    {
        CC_SAFE_RELEASE(pair.second);
    }
    _fontCache.clear();
}

std::string ResourceManager::getResourceStats()
{
    std::string stats = StringUtils::format(
        "Resource Statistics:\n"
        "Textures cached: %zu\n"
        "Fonts cached: %zu\n"
        "Card config loaded: %s\n"
        "Initialized: %s",
        _textureCache.size(),
        _fontCache.size(),
        _cardResConfig ? "Yes" : "No",
        _isInitialized ? "Yes" : "No"
    );
    
    return stats;
}

ResourceManager::ResourceManager()
    : _cardResConfig(nullptr)
    , _isInitialized(false)
{
}

ResourceManager::~ResourceManager()
{
    cleanup();
}

Texture2D* ResourceManager::loadTexture(const std::string& texturePath)
{
    std::string fullPath = getResourcePath(texturePath);
    
    if (!isResourceExists(fullPath))
    {
        return nullptr;
    }
    
    Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(fullPath);
    return texture;
}

Font* ResourceManager::loadFont(const std::string& fontPath)
{
    std::string fullPath = getResourcePath(fontPath);
    
    if (!isResourceExists(fullPath))
    {
        CCLOGWARN("Font file not found: %s", fullPath.c_str());
        return nullptr;
    }
    
    // 这里可以加载自定义字体
    // 暂时返回nullptr，使用系统默认字体
    return nullptr;
}

Texture2D* ResourceManager::createCardTexture(int suit, int rank)
{
    // 创建卡牌正面纹理
    Size cardSize = _cardResConfig->getDisplaySize();
    RenderTexture* renderTexture = RenderTexture::create(cardSize.width, cardSize.height);
    
    if (!renderTexture)
    {
        CCLOGERROR("Failed to create render texture for card");
        return nullptr;
    }
    
    renderTexture->begin();
    
    // 获取渲染器和变换矩阵
    Renderer* renderer = Director::getInstance()->getRenderer();
    Mat4 transform = Mat4::IDENTITY;
    uint32_t flags = 0;
    
    // 绘制卡牌背景
    DrawNode* background = DrawNode::create();
    background->drawSolidRect(Vec2::ZERO, Vec2(cardSize.width, cardSize.height), 
        _cardResConfig->getCardFrontColor());
    background->visit(renderer, transform, flags);
    
    // 绘制卡牌边框
    DrawNode* border = DrawNode::create();
    border->drawRect(Vec2::ZERO, Vec2(cardSize.width, cardSize.height), 
        _cardResConfig->getCardBorderColor());
    border->visit(renderer, transform, flags);
    
    // 绘制花色和点数
    std::string suitName = CardGameUtils::getSuitName(static_cast<CardModel::Suit>(suit));
    std::string rankName = CardGameUtils::getRankName(static_cast<CardModel::Rank>(rank));
    
    // 绘制左上角的花色和点数
    Label* suitLabel = Label::createWithTTF(suitName, _cardResConfig->getSuitFontPath(), 
        _cardResConfig->getSuitFontSize());
    if (suitLabel)
    {
        suitLabel->setPosition(Vec2(20, cardSize.height - 20));
        suitLabel->setColor(Color3B::BLACK);
        suitLabel->visit(renderer, transform, flags);
    }
    
    Label* rankLabel = Label::createWithTTF(rankName, _cardResConfig->getRankFontPath(), 
        _cardResConfig->getRankFontSize());
    if (rankLabel)
    {
        rankLabel->setPosition(Vec2(20, cardSize.height - 50));
        rankLabel->setColor(Color3B::BLACK);
        rankLabel->visit(renderer, transform, flags);
    }
    
    // 绘制中央的大花色
    Label* centerSuitLabel = Label::createWithTTF(suitName, _cardResConfig->getSuitFontPath(), 
        _cardResConfig->getSuitFontSize() * 2);
    if (centerSuitLabel)
    {
        centerSuitLabel->setPosition(Vec2(cardSize.width / 2, cardSize.height / 2));
        centerSuitLabel->setColor(Color3B::BLACK);
        centerSuitLabel->visit(renderer, transform, flags);
    }
    
    renderTexture->end();
    
    // 保留 renderTexture，防止纹理被释放
    renderTexture->retain();
    Texture2D* texture = renderTexture->getSprite()->getTexture();
        texture->retain();
    
    return texture;
}

Texture2D* ResourceManager::createCardBackTexture()
{
    // 创建卡牌背面纹理
    Size cardSize = _cardResConfig->getDisplaySize();
    RenderTexture* renderTexture = RenderTexture::create(cardSize.width, cardSize.height);
    
    if (!renderTexture)
    {
        CCLOGERROR("Failed to create render texture for card back");
        return nullptr;
    }
    
    renderTexture->begin();
    
    // 绘制卡牌背景
    DrawNode* background = DrawNode::create();
    background->drawSolidRect(Vec2::ZERO, Vec2(cardSize.width, cardSize.height), 
        _cardResConfig->getCardBackColor());
    background->visit();
    
    // 绘制卡牌边框
    DrawNode* border = DrawNode::create();
    border->drawRect(Vec2::ZERO, Vec2(cardSize.width, cardSize.height), 
        _cardResConfig->getCardBorderColor());
    border->visit();
    
    // 绘制中央图案
    DrawNode* pattern = DrawNode::create();
    Vec2 center = Vec2(cardSize.width / 2, cardSize.height / 2);
    float radius = std::min(cardSize.width, cardSize.height) / 4;
    
    // 绘制简单的几何图案
    for (int i = 0; i < 8; ++i)
    {
        float angle = i * M_PI / 4;
        Vec2 point = Vec2(center.x + radius * cos(angle), center.y + radius * sin(angle));
        pattern->drawSolidCircle(point, 5, 0, 10, Color4F::WHITE);
    }
    
    pattern->visit();
    
    renderTexture->end();
    
    // 保留 renderTexture，防止纹理被释放
    renderTexture->retain();
    Texture2D* texture = renderTexture->getSprite()->getTexture();
        texture->retain();
    
    return texture;
}

void ResourceManager::cleanup()
{
    clearResourceCache();
    
    CC_SAFE_RELEASE(_cardResConfig);
    
    _isInitialized = false;
}

bool ResourceManager::validateResourcePath(const std::string& resourcePath)
{
    if (resourcePath.empty()) return false;
    
    // 检查路径是否包含非法字符
    if (resourcePath.find("..") != std::string::npos) return false;
    
    return true;
}

std::string ResourceManager::getCacheKey(const std::string& resourcePath)
{
    // 将路径转换为缓存键
    std::string key = resourcePath;
    std::replace(key.begin(), key.end(), '/', '_');
    std::replace(key.begin(), key.end(), '\\', '_');
    return key;
}
