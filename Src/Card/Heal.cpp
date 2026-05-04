#include "../Object/Player.h"
#include "Heal.h"

Heal::Heal(std::shared_ptr<Player> player)
	:player_(player),used_(false)
{
}

Heal::~Heal()
{
}

bool Heal::Init(void)
{
	SetParam();
	return true;
}

void Heal::Update(void)
{
}

void Heal::Draw(void)
{
}

bool Heal::Release(void)
{
	return false;
}

void Heal::SetParam()
{
	Heal_ = 20;
}

void Heal::ProcessCard()
{
	if (!player_) return;

	// ÉåÉAìxÇ≤Ç∆ÇÃî{ó¶ê›íË
	float healRate = 1.0f;
	switch (rarity_)
	{
	case CARD_RARITY::BRONZE: healRate = 1.0f; break;
	case CARD_RARITY::SILVER: healRate = 1.5f; break;
	case CARD_RARITY::GOLD:   healRate = 2.0f; break;
	}

	int healAmount = static_cast<int>(Heal_ * healRate);

	player_->Heal(healAmount);
	used_ = true;
	//printfDx("Heal::ProcessCard() called! Heal_=%d\n", healAmount);
}