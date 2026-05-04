#include <chrono>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Common/Fader.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "../Object/Player.h"
#include "../Object/Stage.h"
#include "../Object/Planet.h"
#include "../Card/CardBase.h"
#include "../Card/Heal.h"
#include "../Card/Shot.h"
#include "../Card/Cannon.h"
#include "../Card/Monster.h"
#include "Camera.h"
#include "CardManager.h"

//カードのアイコンを作ったが位置が合わない

CardManager::CardManager()
{
    player_ = nullptr;
    enemy_ = nullptr;
    base_ = nullptr;
    cardImgBr_ = cardImgSl_ = cardImgGo_ = -1;

    cardUsedThisFrame_ = false;
}

CardManager::~CardManager()
{
    Release();
}

void CardManager::SetPlayer(Player* player)
{
    player_ = std::shared_ptr<Player>(player, [](Player*) {});
}

void CardManager::SetEnemy(std::shared_ptr<EnemyGolem> enemy)
{
    enemy_ = enemy;
}

void CardManager::SetRarity(CARD rarity)
{
    rarity_ = rarity;
}

bool CardManager::Init()
{
    //アイコン
    LoadIcons();
    return true;
}

void CardManager::Update()
{
    InputManager& ins = InputManager::GetInstance();
  
    // --- レア度切り替え -------------------

    if (ins.IsTrgDown(KEY_INPUT_V))
    {
        int val = static_cast<int>(rarity_);
        val++;
        if (val > static_cast<int>(CARD::GOLD)) val = static_cast<int>(CARD::BRONZE);
        rarity_ = static_cast<CARD>(val);
        //printfDx("Rarity changed: %d\n", val);
    }

    // 押した瞬間だけ発動
    if (ins.IsTrgDown(KEY_INPUT_X))
    {
        //printfDx("Heal card used!\n");
        DoCard(EFFECT::HEAL);
    }
    if (ins.IsTrgDown(KEY_INPUT_Q))
    {
        //printfDx("Shot card used!\n");
        DoCard(EFFECT::SHOT);
    } 
    if (ins.IsTrgDown(KEY_INPUT_E))
    {
        //printfDx("Cannon card used!\n");
        //すでにCannonが存在している場合は再生成しない
        if (!base_ || dynamic_cast<Cannon*>(base_.get()) == nullptr)
        {
            DoCard(EFFECT::CANNON);
        }
        else
        {
            // 既に存在するCannonならProcessCard()だけ呼ぶ
            base_->ProcessCard();
        }
    }
    if (base_)
    {
        base_->Update();

        if (!base_->IsAlive())
        {
            base_->Release();  // ←明示停止
            base_.reset();
            //printfDx("Cannon expired!\n");
        }
    }
}

void CardManager::Draw()
{
    if (base_)
        base_->Draw();

  //  // UI表示
  ////=== カード枠描画 ===
  //  int cx = 100;
  //  int cy = 500;

  //  int cardImg = -1;
  //  switch (rarity_)
  //  {
  //  case CARD::BRONZE:
  //      cardImg = cardImgBr_;
  //      break;
  //  case CARD::SILVER:
  //      cardImg = cardImgSl_;
  //      break;
  //  case CARD::GOLD:
  //      cardImg = cardImgGo_;
  //      break;
  //  }

  //  DrawRotaGraph(cx, cy, 0.12f, 0.0, cardImg, true);

    //アイコン描画
    DrawCardIcon();

}

bool CardManager::Release()
{
    if (cardImgBr_ != -1) DeleteGraph(cardImgBr_);
    if (cardImgSl_ != -1) DeleteGraph(cardImgSl_);
    if (cardImgGo_ != -1) DeleteGraph(cardImgGo_);

    if (iconShot_ != -1) DeleteGraph(iconShot_);
    if (iconCannon_ != -1) DeleteGraph(iconCannon_);

    return true;
}

//呼び出し用
void CardManager::UseCard(EFFECT effectId)
{
    DoCard(effectId);
}

//起動用
void CardManager::DoCard(EFFECT effectId)
{
    if (player_ && player_->IsAttacking())
        return;

    std::shared_ptr<CardBase> card;
    switch (effectId)
    {
    case EFFECT::HEAL:
        base_ = std::make_unique<Heal>(player_);
        break;
    case EFFECT::SHOT:
        base_ = std::make_shared<Shot>(player_,enemy_);
        break;
    case EFFECT::CANNON:
        base_ = std::make_shared<Cannon>(player_,enemy_);
        break;
    default:
        base_ = std::make_unique<CardBase>();
        break;
    }
 
    // 効果発動
    if (base_)
    {
         // 現在のレア度をカードに渡す
        base_->SetRarity(static_cast<CardBase::CARD_RARITY>(rarity_));

        base_->Init();
        base_->ProcessCard();
       
        if (player_ && !player_->IsAttacking())
        {
            player_->StartAttack();
        }
    }
}

std::vector<std::weak_ptr<Collider>> CardManager::GetCurrentStageColliders()
{
    std::vector<std::weak_ptr<Collider>> cols;

    auto stage = SceneManager::GetInstance().GetStage();
    if (stage)
    {
        if (auto planet = stage->GetActivePlanet().lock())
        {
            cols.push_back(planet->GetTransform().collider);
        }
    }
    return cols;
}

void CardManager::LoadIcons()
{
    // カード枠
    cardImgBr_ = LoadGraph("Data/Image/素材/銅カード2.png");
    cardImgSl_ = LoadGraph("Data/Image/素材/銀カード3.png");
    cardImgGo_ = LoadGraph("Data/Image/素材/金カード2.png");

    // 効果アイコン
    //iconHeal_ = LoadGraph("Data/Image/素材/heal.png");
    iconShot_ = LoadGraph("Data/Image/素材/Shot.png");
    iconCannon_ = LoadGraph("Data/Image/素材/Cannon.png");
    //iconMonster_ = LoadGraph("Data/Image/素材/monster.png");

    int baseX = 20;
    int baseY = 450;
    int interval = 150;

    // ★ すべて「カード中心(100,500)からのオフセット」で管理
    cardList_.clear();
    //cardList_.push_back({ EFFECT::HEAL,    baseX + interval * 0, baseY });
    cardList_.push_back({ EFFECT::SHOT,iconShot_,  baseX + interval * 1, baseY });
    cardList_.push_back({ EFFECT::CANNON, iconCannon_, baseX + interval * 2, baseY });
    //cardList_.push_back({ EFFECT::MONSTER,  baseX + interval * 3, baseY });
}

void CardManager::DrawCardIcon()
{
    int cardImg = -1;

    // ★ レア度によって使用する枠画像を変更
    switch (rarity_)
    {
    case CARD::BRONZE: cardImg = cardImgBr_; break;
    case CARD::SILVER: cardImg = cardImgSl_; break;
    case CARD::GOLD:   cardImg = cardImgGo_; break;
    }

    for (auto& c : cardList_)
    {
        // レア度に応じた枠を描画
        DrawRotaGraph(c.x, c.y, 0.12f, 0.0, cardImg, TRUE);

        // アイコン（固定）
        DrawRotaGraph(c.x, c.y , 0.10f, 0.0, c.iconImg, TRUE);
    }
}
