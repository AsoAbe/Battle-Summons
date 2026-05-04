#include "../Manager/CardManager.h"
#include "../Object/Player.h"
#include "CardBase.h"

CardBase::CardBase(void)
{
	Damage_ = 0;
	Heal_ = 0;
	speed_ = 0.0f;
	isAlive_ = true;
}

CardBase::~CardBase(void)
{
}

bool CardBase::Init(void)
{
	SetParam();
    return true;
}

void CardBase::Update(void)
{
}

void CardBase::Draw(void)
{
}

bool CardBase::Release(void)
{
    return true;
}

void CardBase::SetParam()
{
}

void CardBase::ProcessCard(void)
{
}

void CardBase::CreateCard(void)
{
}

bool CardBase::IsAlive() const
{
	return isAlive_;
}

void CardBase::SetRarity(CARD_RARITY rarity)
{
	 rarity_ = rarity; 
}

void CardBase::SetEnemy(std::weak_ptr<EnemyGolem> enemy)
{
	enemy_ = enemy;   // weak_ptr ‚É‘ã“ü
}