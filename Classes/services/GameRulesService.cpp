#include "GameRulesService.h"
#include <random>
#include <algorithm>

bool GameRulesService::canCardsMatch(CardModel* card1, CardModel* card2)
{
    if (!card1 || !card2) return false;
    
    int rank1 = static_cast<int>(card1->_rank);
    int rank2 = static_cast<int>(card2->_rank);
    
    int diff = abs(rank1 - rank2);
    
    return (diff == 1);
}

bool GameRulesService::executeCardMatch(GameModel* gameModel, int playfieldCardId, int handCardId)
{
    if (!gameModel) return false;
    
    CardModel* playfieldCard = gameModel->getCardById(playfieldCardId);
    CardModel* handCard = gameModel->getCardById(handCardId);
    
    if (!playfieldCard || !handCard) return false;
    if (!canCardsMatch(playfieldCard, handCard)) return false;
    
    auto& playfieldCards = gameModel->_playfieldCards;
    auto playfieldIt = std::find(playfieldCards.begin(), playfieldCards.end(), playfieldCard);
    if (playfieldIt != playfieldCards.end())
    {
        playfieldCards.erase(playfieldIt);
    }
    
    playfieldCard->_cardArea = "hand";
    playfieldCard->_isFaceUp = true;
    gameModel->_handCards.push_back(playfieldCard);
    
    gameModel->_movesCount++;
    
    // 每消除主牌堆一张扑克，分数增加100
    gameModel->_score += 100;
    
    return true;
}

void GameRulesService::shuffleCards(std::vector<CardModel*>& cards)
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}

bool GameRulesService::dealCards(GameModel* gameModel, int count, const std::string& toArea)
{
    if (!gameModel) return false;
    
    // 发牌逻辑由 GameModelGenerator 在初始化时完成
    return true;
}

bool GameRulesService::flipStackCardToHand(GameModel* gameModel)
{
    if (!gameModel) return false;
    if (gameModel->_stackCards.empty()) return false;
    
    CardModel* topStackCard = gameModel->_stackCards.back();
    gameModel->_stackCards.pop_back();
    
    topStackCard->_cardArea = "hand";
    topStackCard->_isFaceUp = true;
    gameModel->_handCards.push_back(topStackCard);
    
    return true;
}

bool GameRulesService::checkGameWon(const GameModel* gameModel)
{
    if (!gameModel) return false;
    
    // TriPeaks胜利条件：主牌区所有卡牌都被清除
    return gameModel->_playfieldCards.empty();
}

bool GameRulesService::checkGameLost(const GameModel* gameModel)
{
    if (!gameModel) return false;
    
    // 失败条件：备用牌堆空了，且没有可匹配的牌
    if (!gameModel->_stackCards.empty())
    {
        return false; // 还有备用牌，游戏继续
    }
    
    // 检查是否还有可移动的卡牌
    return !hasAvailableMoves(gameModel);
}

bool GameRulesService::hasAvailableMoves(const GameModel* gameModel)
{
    if (!gameModel) return false;
    
    // 获取当前手牌顶部卡牌
    if (gameModel->_handCards.empty()) return false;
    
    CardModel* handCard = gameModel->_handCards.back();
    
    // 检查主牌区是否有可以匹配的卡牌
    for (CardModel* playfieldCard : gameModel->_playfieldCards)
    {
        if (playfieldCard->_isFaceUp && canCardsMatch(playfieldCard, handCard))
        {
            return true;
        }
    }
    
    // 如果还有备用牌堆，也算有可移动的
    return !gameModel->_stackCards.empty();
}

std::vector<std::pair<int, int>> GameRulesService::findMatchingPairs(const GameModel* gameModel)
{
    std::vector<std::pair<int, int>> pairs;
    
    if (!gameModel || gameModel->_handCards.empty()) return pairs;
    
    CardModel* handCard = gameModel->_handCards.back();
    
    for (CardModel* playfieldCard : gameModel->_playfieldCards)
    {
        if (playfieldCard->_isFaceUp && canCardsMatch(playfieldCard, handCard))
        {
            pairs.push_back(std::make_pair(playfieldCard->_cardId, handCard->_cardId));
        }
    }
    
    return pairs;
}

int GameRulesService::calculateHandScore(const std::vector<CardModel*>& hand)
{
    int score = 0;
    int aceCount = 0;
    
    for (auto card : hand)
    {
        int rank = static_cast<int>(card->_rank);
        if (rank == 0) // ACE
        {
            aceCount++;
            score += 11;
        }
        else if (rank >= 10) // J, Q, K
        {
            score += 10;
        }
        else
        {
            score += (rank + 1);
        }
    }
    
    // 处理ACE为1的情况
    while (score > 21 && aceCount > 0)
    {
        score -= 10;
        aceCount--;
    }
    
    return score;
}

bool GameRulesService::isBusted(const std::vector<CardModel*>& hand)
{
    return calculateHandScore(hand) > 21;
}

