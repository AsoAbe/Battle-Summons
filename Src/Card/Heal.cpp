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
	case CARD_RARITY::BRONZE: healRate = BRONZE_RATE; break;
	case CARD_RARITY::SILVER: healRate = SILVER_RATE; break;
	case CARD_RARITY::GOLD:   healRate = GOLD_RATE; break;
	}
	
	int healAmount = static_cast<int>(Heal_ * healRate);

	player_->Heal(healAmount);
	used_ = true;
	//printfDx("Heal::ProcessCard() called! Heal_=%d\n", healAmount);
}