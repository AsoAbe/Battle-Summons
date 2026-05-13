#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/SoundManager.h"
#include "../Object/EnemyGolem.h"
#include "ShotBase.h"
#include "Shot.h"

//ダメージ処理実装　当たり判定が機能しないCannonはできたのに

Shot::Shot(std::shared_ptr<Player> player, std::shared_ptr<EnemyGolem> enemy)
	: player_(player)
	, enemy_(enemy)
{
}

Shot::~Shot()
{
}

void Shot::Update()
{
	// ShotBase の基礎処理
	ShotBase::Update();

	// 状態が飛んでいたら終わり
	if (state_ != STATE::SHOT) return;

	//ダメージチェック
	CheckDamage();
}

void Shot::SetParam()
{
	// --- レア度ごとの補正 ---
	float rate = 1.0f;
	switch (rarity_)
	{
	case CARD_RARITY::BRONZE:
		rate = BRONZE_RATE;
		break;
	case CARD_RARITY::SILVER:
		rate = SILVER_RATE;
		break;
	case CARD_RARITY::GOLD:
		rate = GOLD_RATE;
		break;
	default:
		rate = BRONZE_RATE;
		break;
	}

	// 弾の速度
	speed_ = DEFAULT_SPEED * rate;
	// 生存時間
	timeAlive_ = DEFAULT_ALIVE_TIME;
	//衝突用球体半径
	scale = DEFAULT_SCALE * rate;
	Damage_ = DEFAULT_DAMAGE / rate;

	if (player_)
	{
		player_->Damage(static_cast<int>(Damage_ * rate) * PLAYER_DAMAGE_RATE);
	}

}

void Shot::ProcessCard()
{
	if (!player_) return;

	SoundManager::SOUND_ID se = SoundManager::SOUND_ID::SHOT_S;

	// レア度ごとの倍率
	switch (rarity_)
	{
	case CARD_RARITY::BRONZE:
		player_->CreateBronzeShot();
		se = SoundManager::SOUND_ID::SHOT_S;
		//printfDx("Bronze shot fired!\n");
		break;
	case CARD_RARITY::SILVER:
		player_->CreateSilverShot();
		se = SoundManager::SOUND_ID::SHOT_M;
		//printfDx("Silver shot fired!\n");
		break;
	case CARD_RARITY::GOLD:
		player_->CreateGoldShot();
		se = SoundManager::SOUND_ID::SHOT_L;
		//printfDx("Gold shot fired!\n");
		break;
	default:
		player_->CreateBronzeShot();
		break;
	}

	SoundManager::GetInstance().PlaySE(
		se,
		true,
		SoundManager::VOLUME_STANDARD
	);
}

void Shot::CheckDamage()
{
	if (hasDealtDamage_) return;
	if (!enemy_) return;

	// 弾情報
	VECTOR p0 = prevPos_;
	VECTOR p1 = GetPos();
	float  shotR = GetCollisionRadius();

	// 敵カプセル情報
	VECTOR eTop = enemy_->GetCapsuleTop();
	VECTOR eDown = enemy_->GetCapsuleBottom();
	float  eR = enemy_->GetRadius();

	// ★ カプセル vs 球 の衝突判定
	if (AsoUtility::IsHitSphereCapsule(
		p0, shotR,
		eTop, eDown, eR
	))
	{
		enemy_->Damage(Damage_);
		hasDealtDamage_ = true;

		// ★ ヒット効果音
		SoundManager::GetInstance().PlaySE(
			SoundManager::SOUND_ID::BLAST_S,
			true,
			SoundManager::VOLUME_STANDARD
		);

		// 爆発
		Blast();
	}
}

void Shot::SetEnemy(std::shared_ptr<EnemyGolem> enemy)
{
	enemy_ = enemy;
}