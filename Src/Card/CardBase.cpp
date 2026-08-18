#include "../Manager/CardManager.h"
#include "../Object/Player.h"
#include "CardBase.h"

CardBase::CardBase(void)
{
	rarity_ = CARD_RARITY::NONE;

	dir_ = AsoUtility::VECTOR_ZERO;
	Damage_ = NONE;
	Heal_ = NONE;
	speed_ = NONE;
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

void CardBase::Activate(CARD_RARITY rarity)
{
	SetRarity(rarity);
	Init();
	ProcessCard();
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