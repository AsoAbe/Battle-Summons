#include"../Application.h"
#include"../Manager/InputManager.h"
#include"../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Object/Player.h"
#include "../Object/EnemyGolem.h"
#include "../Object/Planet.h"
#include"CannonBase.h"
#include "Cannon.h"
#pragma message(">>> Included Stage.h from: " __FILE__)
Cannon::Cannon(std::shared_ptr<Player> player, std::weak_ptr<EnemyGolem> enemy)
	: CannonBase(0, 0),player_(player),enemy_(enemy)
{
	size_ = 0.8f;

	standModelId_ = -1;
	standScl_ = { 0.0f,0.0f,0.0f };
	standRot_ = { 0.0f,0.0f,0.0f };
	standPos_ = { 0.0f,0.0f,0.0f };

	barrelModelId_ = -1;
	barrelScl_ = { 0.0f,0.0f,0.0f };
	barrelRot_ = { 0.0f,0.0f,0.0f };
	barrelPos_ = { 0.0f,0.0f,0.0f };
	barrelLocalPos_ = { 0.0f,0.0f,0.0f };

	shotModelId_ = -1;

	stepShotDelay_ = 0.0f;

	blastEffect_ = -1;

	stepAlive_ = 0.0f;
}

Cannon::~Cannon(void)
{
}

bool Cannon::Init(void)
{
	switch (rarity_)
	{
	case CARD_RARITY::BRONZE: DamageRate = BRONZE_RATE;
		break;
	case CARD_RARITY::SILVER: DamageRate = SILVER_RATE;
		break;
	case CARD_RARITY::GOLD:   DamageRate = GOLD_RATE;
		break;
	}

	size_ *= DamageRate;
#pragma region ñCë‰
	ResourceManager& rem = ResourceManager::GetInstance();
	standModelId_ = rem.LoadModelDuplicate(ResourceManager::SRC::CANNON_STAND);

	//ñCë‰ÇÃëÂÇ´Ç≥
	//ñCë‰ÇÃëÂÇ´Ç≥
	standScl_ = { size_,size_,size_ };


	if (player_)
	{
		//ÉvÉåÉCÉÑÅ[ÇÃëOï˚ï˚å¸ÅiForwardÅjÇéÊìæ
		VECTOR forward = player_->GetTransform().GetForward();

		//ÉvÉåÉCÉÑÅ[ÇÃà íuÇäÓèÄÇ…ÅAëÂñCÇëOÇ…Ç∏ÇÁÇ∑ãóó£Åií≤êÆâ¬Åj
		float offsetDist = OFFSET_DIST;

		//à íu = ÉvÉåÉCÉÑÅ[ÇÃà íu + Forward Å~ ãóó£
		standPos_ = VAdd(
			player_->GetPos(),
			VScale(forward, offsetDist)
		);
	}
	standRot_ = { 0.0f,0.0f,0.0f };


#pragma endregion
#pragma region ñCêg
	barrelModelId_ =
		rem.LoadModelDuplicate(ResourceManager::SRC::CANNON_BARREL);

	//ñCêgÇÃëÂÇ´Ç≥
	barrelScl_ = { size_,size_,size_ };
	//ñCêgÇÃäpìx
	barrelRot_ = { -0.5f,0.0f,0.0f };
	barrelRot_.y = player_->GetRot().y;
	//ñCêgÇÃç¿ïW
	barrelLocalPos_ = { 0.0f,BARREL_HEIGHT *DamageRate,0.0f };

	// VECTORìØémÇÃâ¡éZ VAddä÷êî
	barrelPos_ = VAdd(standPos_, barrelLocalPos_);

#pragma endregion

	// íeÇÃÉÇÉfÉã
	shotModelId_ =
		rem.LoadModelDuplicate(ResourceManager::SRC::CANNON_SHOT);

	// íeî≠éÀÇÃçdíºéûä‘
	stepShotDelay_ = 0.0f;

	// îöî≠ÉGÉtÉFÉNÉgì«Ç›çûÇ›
	blastEffect_ = ResourceManager::GetInstance().Load(
			ResourceManager::SRC::CANNON_BLAST).handleId_;

	//èâä˙ê›íËÇÉÇÉfÉãÇ…îΩâf
	Update();
	return true;
}

void Cannon::Update(void)
{

	ProcessRot();
	ProcessShot();

	//UpdateShots();   //SceneManagerÇ…Ç†ÇÈ
	UpdateLifeTime();
	UpdateTransform();

	for (auto it = shots_.begin(); it != shots_.end(); )
	{
		(*it)->Update();

		if (!(*it)->IsAlive())
		{
			//delete* it;           
			it = shots_.erase(it);
		}
		else
		{
			++it;
		}
	}

}

void Cannon::Draw(void)
{
	
	// ñCë‰ÇÃÉÇÉfÉãï`âÊ
	MV1DrawModel(standModelId_);

	MV1DrawModel(barrelModelId_);
	// --- íeÇÃçXêV ---
	for (auto& shot : shots_)
	{
		if (shot->IsAlive())
		{
			shot->Draw();
		}
	}
}

bool Cannon::Release(void)
{
	
	shots_.clear();   // Update ë§Ç≈ delete çœÇ›

	return true;
}


void Cannon::ProcessCard()
{
	ProcessShot();
}

void Cannon::ProcessRot(void)
{
	auto& ins = InputManager::GetInstance();

	//âÒì]ó 
	float rotPowRad = 2.0f * DX_PI_F / 180.0f;

}

void Cannon::ProcessShot(void)
{
	auto& ins = InputManager::GetInstance();

	// Ç∑Ç≈Ç…åÇÇ¡ÇƒÇ¢ÇΩÇÁÇ‡Ç§åÇÇƒÇ»Ç¢
	if (Fired_)
		return;

	if (ins.IsNew(KEY_INPUT_E) && stepShotDelay_ <= 0.0f)
	{
		CannonBase* shot = GetValidShot();

		MATRIX matRot = MGetIdent();
		matRot = MMult(matRot, MGetRotX(barrelRot_.x));
		matRot = MMult(matRot, MGetRotY(barrelRot_.y));
		matRot = MMult(matRot, MGetRotZ(barrelRot_.z));

		VECTOR dir = VNorm(VTransform({ 0.0f, 0.0f, 1.0f }, matRot));
		VECTOR localPosRot = VTransform({ 0.0f, SHOT_OFFSET_Y * DamageRate, SHOT_OFFSET_Z }, matRot);
		VECTOR pos = VAdd(barrelPos_, localPosRot);

		shot->CreateShot(pos, dir);

		// ñCåÇâπ
		SoundManager::GetInstance().PlaySE(
			SoundManager::SOUND_ID::CAANON,
			true,   // Å© load Ç true
			SoundManager::VOLUME_STANDARD
		);

		auto& sceneMng = SceneManager::GetInstance();
		auto stage = sceneMng.GetStage();
		if (stage)
		{
			auto planet = stage->GetActivePlanet().lock();
			if (planet)
			{
				auto collider = planet->GetTransform().collider;
				if (collider)
				{
					std::vector<std::weak_ptr<Collider>> cols = { collider };
					shot->SetColliders(cols);
				}
			}
		}

		stepShotDelay_ = SHOT_DELAY;
	    //åÇÇ¡ÇΩèuä‘ÇæÇØ true
		Fired_ = true;
	}

	if (stepShotDelay_ > 0.0f)
	{
		stepShotDelay_ -= 1.0f / DEFAULT_FPS;
	}
	else
	{
		Fired_ = false;
	}
}

void Cannon::UpdateLifeTime()
{
	stepAlive_ += 1.0f / DEFAULT_FPS;

	if (stepAlive_ >= timeAlive_)
	{
		isAlive_ = false;
	}
}

void Cannon::UpdateTransform()
{
    // ñCë‰
    MV1SetScale(standModelId_, standScl_);
    MV1SetRotationXYZ(standModelId_, standRot_);
    MV1SetPosition(standModelId_, standPos_);

    // ñCêg
    MV1SetScale(barrelModelId_, barrelScl_);
    MV1SetRotationXYZ(barrelModelId_, barrelRot_);
    MV1SetPosition(barrelModelId_, barrelPos_);
}

CannonBase* Cannon::GetValidShot(void)
{
	for (auto& shot : shots_)
	{
		if (!shot->IsAlive())
		{
			return shot.get();
		}
	}

	auto shot = std::make_unique<CannonBase>(shotModelId_, blastEffect_);
	shot->SetPlayer(player_);
	shot->SetEnemy(enemy_);
	shot->SetRarity(rarity_);

	shots_.push_back(std::move(shot));
	return shots_.back().get();
}
