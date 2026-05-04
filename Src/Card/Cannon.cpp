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
}

Cannon::~Cannon(void)
{
}

bool Cannon::Init(void)
{
	switch (rarity_)
	{
	case CARD_RARITY::BRONZE: DamageRate = 1.0f; break;
	case CARD_RARITY::SILVER: DamageRate = 1.5f; break;
	case CARD_RARITY::GOLD:   DamageRate = 2.0f; break;
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
		float offsetDist = 120.0f;

		//à íu = ÉvÉåÉCÉÑÅ[ÇÃà íu + Forward Å~ ãóó£
		standPos_ = VAdd(
			player_->GetPos(),
			VScale(forward, offsetDist)
		);
	}
	standRot_ = { 0.0f,0.0f,0.0f };
	//standPos_ = { 0.0f,10.0f,-200.0f };


#pragma endregion
#pragma region ñCêg
	barrelModelId_ =
		rem.LoadModelDuplicate(ResourceManager::SRC::CANNON_BARREL);

	//ñCêgÇÃëÂÇ´Ç≥
	barrelScl_ = { size_,size_,size_ };
	//ñCêgÇÃäpìx
	barrelRot_ = { -0.5f,0.0f,0.0f };
	barrelRot_.y = player_->GetRot().y;
	//barrelRot_ = standRot_;
	//ñCêgÇÃç¿ïW
	//barrelPos_ = { 0.0f,110.0f,-200.0f };
	barrelLocalPos_ = { 0.0f,100.0f *DamageRate,0.0f };
	/*barrelPos_.x = standPos_.x + barrelLocalPos_.x;
	barrelPos_.y = standPos_.y + barrelLocalPos_.y;
	barrelPos_.z = standPos_.z + barrelLocalPos_.z;*/

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

	//// --- íeÇÃçXêV ---
	//for (auto& shot : shots_)
	//{
	//	if (shot->IsAlive())
	//	{
	//		shot->Update();
	//	}
	//}
	for (auto it = shots_.begin(); it != shots_.end(); )
	{
		(*it)->Update();

		if (!(*it)->IsAlive())
		{
			delete* it;                 // Åö ïKê{
			it = shots_.erase(it);
		}
		else
		{
			++it;
		}
	}

	// ê∂ë∂éûä‘ÇÉJÉEÉìÉg ---
	stepAlive_ += 1.0f / DEFAULT_FPS; // ñàÉtÉåÅ[ÉÄâ¡éZÅi60FPSëzíËÅj

	if (stepAlive_ >= timeAlive_)
	{
		isAlive_ = false; // CardManagerÇ…îjä¸ÇµÇƒÇ‡ÇÁÇ§
	}
#pragma region ñCë‰
	// ÇRÇcÉÇÉfÉãÇÃëÂÇ´Ç≥Çê›íË(à¯êîÇÕÅAx, y, zÇÃî{ó¶)
	MV1SetScale(standModelId_, standScl_);

	// ÇRÇcÉÇÉfÉãÇÃå¸Ç´(à¯êîÇÕÅAx, y, zÇÃâÒì]ó ÅBíPà ÇÕÉâÉWÉAÉìÅB)
	MV1SetRotationXYZ(standModelId_, standRot_);

	// ÇRÇcÉÇÉfÉãÇÃà íu(à¯êîÇÕÅAÇRÇcç¿ïW)
	MV1SetPosition(standModelId_, standPos_);
#pragma endregion
#pragma region ñCêg
	// ÇRÇcÉÇÉfÉãÇÃëÂÇ´Ç≥Çê›íË(à¯êîÇÕÅAx, y, zÇÃî{ó¶)
	MV1SetScale(barrelModelId_, barrelScl_);

	// ÇRÇcÉÇÉfÉãÇÃå¸Ç´(à¯êîÇÕÅAx, y, zÇÃâÒì]ó ÅBíPà ÇÕÉâÉWÉAÉìÅB)
	MV1SetRotationXYZ(barrelModelId_, barrelRot_);

	// ÇRÇcÉÇÉfÉãÇÃà íu(à¯êîÇÕÅAÇRÇcç¿ïW)
	MV1SetPosition(barrelModelId_, barrelPos_);

	ProcessRot();
#pragma endregion
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
	CannonBase::Release();

	MV1DeleteModel(standModelId_);
	MV1DeleteModel(barrelModelId_);
	MV1DeleteModel(shotModelId_);

	shots_.clear();   // Update ë§Ç≈ delete çœÇ›

	return true;
}

std::vector<CannonBase*> Cannon::GetShots(void)
{
	return shots_;
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

	////â°âÒì]
	//if (ins.IsNew(KEY_INPUT_RIGHT))
	//{
	//	standRot_.y += rotPowRad;
	//	barrelRot_.y = standRot_.y;
	//}
	//if (ins.IsNew(KEY_INPUT_LEFT))
	//{
	//	standRot_.y -= rotPowRad;
	//	barrelRot_.y = standRot_.y;
	//}

	////ècâÒì]
	//if (ins.IsNew(KEY_INPUT_UP))
	//{
	//	if (MAX_ANGLE_X > barrelRot_.x)
	//	{
	//		barrelRot_.x += rotPowRad;
	//	}
	//}

	//if (ins.IsNew(KEY_INPUT_DOWN))
	//{
	//	if (MIN_ANGLE_X < barrelRot_.x)
	//	{
	//		barrelRot_.x -= rotPowRad;
	//	}
	//}
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
		VECTOR localPosRot = VTransform({ 0.0f, 25.0f * DamageRate, 30.0f }, matRot);
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
		Fired_ = true;   // Åö åÇÇ¡ÇΩèuä‘ÇæÇØ true
	}

	if (stepShotDelay_ > 0.0f)
	{
		stepShotDelay_ -= 1.0f / DEFAULT_FPS;
	}
}

CannonBase* Cannon::GetValidShot(void)
{
	size_t size = shots_.size();
	for (int i = 0; i < size; i++)
	{
		if (!shots_[i]->IsAlive())
		{
			return shots_[i];
		}
	}
	CannonBase* shot = new CannonBase(shotModelId_, blastEffect_);

	shot->SetPlayer(player_);
	shot->SetEnemy(enemy_);

	shot->SetRarity(rarity_);

	shots_.push_back(shot);

	return shot;
}
