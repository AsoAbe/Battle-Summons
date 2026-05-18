#define NOMINMAX
#include <algorithm>
#include <EffekseerForDXLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/Player.h"
#include "../Object/EnemyGolem.h"
#include "CannonBase.h"

//爆発の判定を相手の半径を考慮、試していない

CannonBase::CannonBase(int baseModelId, int blastEffect)
	: blastDuration_(BLAST_DURATION)
{
	baseModelId_ = baseModelId;

	modelId_ = -1;

	dir_ = { 0.0f,0.0f,0.0f };
	scl_ = { 1.0f,1.0f,1.0f };
	rot_ = { 0.0f,0.0f,0.0f };
	pos_ = { 0.0f,0.0f,0.0f };

	speed_ = 0.0f;
	radius_ = 0.0f;
	size_ = 1.0f;
	gravityPow_ = 0.0f;

	blastEffectHandle_ = blastEffect;
	blastEffectPlayId_ = -1;

	blastTimer_ = 0.0f;
	blastRadius_ = DEFAULT_BLAST_RADIUS;

	Damage_ = DEFAULT_DAMAGE;

	hasDealtDamage_ = false;

	DamageRate = 1.0f;

	state_ = STATE::NONE;

	Fired_ = false;

	prevPos_ = { 0.0f,0.0f,0.0f };

	bulletRadius_ = DEFAULT_BULLET_RADIUS;

	transform_ = Transform{};
}

CannonBase::~CannonBase(void)
{
	if (blastEffectPlayId_ >= 0)
	{
		StopEffekseer3DEffect(blastEffectPlayId_);
		blastEffectPlayId_ = -1;
	}

	if (modelId_ >= 0)
	{
		MV1DeleteModel(modelId_);
		modelId_ = -1;
	}
}

void CannonBase::CreateShot(VECTOR pos, VECTOR dir)
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
	// 使用メモリ容量と読み込み時間の削減のため
	// モデルデータをいくつもメモリ上に存在させない
	modelId_ = MV1DuplicateModel(baseModelId_);

	// 弾の大きさを設定
	scl_ = { MODEL_SCALE * DamageRate, MODEL_SCALE * DamageRate, MODEL_SCALE * DamageRate };

	// 弾の角度を設定
	rot_ = { 0.0f, 0.0f, 0.0f };

	// 弾の発射位置を設定
	pos_ = pos;

	// 弾の発射方向の設定
	dir_ = dir;

	// 弾の速度
	speed_ = DEFAULT_SPEED;

	// 弾の生存判定
	isAlive_ = true;

	// 重力
	gravityPow_ = 0.0f;

	//当たり判定半径
	bulletRadius_ = DEFAULT_BULLET_RADIUS;

	// カプセルコライダー生成
	capsule_ = std::make_unique<Capsule>(Transform()); // 仮のTransform
	capsule_->SetLocalPosTop(VAdd(pos_, { 0.0f, CAPSULE_HALF_HEIGHT, 0.0f }));
	capsule_->SetLocalPosDown(VAdd(pos_, { 0.0f, -CAPSULE_HALF_HEIGHT, 0.0f }));
	capsule_->SetRadius(radius_);

	// カプセルの位置を弾の座標に反映
	UpdateCapsulePos();

	//// 爆発のアニメーション用カウンタ
	//blastCntAnim_ = 0;

	//// 爆発のアニメーション速度
	//blastSpeedAnim_ = 0.3f;
	auto player = player_.lock();
	if (player)
	{
		player->Damage(static_cast<int>(Damage_ * DamageRate * PLAYER_DAMAGE_RATE));
	}
	// 状態遷移
	ChangeState(STATE::SHOT);
}

void CannonBase::Update(void)
{

	// 毎フレームEffekseer更新
	//UpdateEffekseer3D();

	switch (state_)
	{
	case CannonBase::STATE::NONE:
		break;
	case CannonBase::STATE::SHOT:
		UpdateShot();
		break;
	case CannonBase::STATE::BLAST:
		UpdateBlast();
		break;
	case CannonBase::STATE::END:
		UpdateEnd();
		break;
	default:
		break;
	}

		prevPos_ = pos_;

}

void CannonBase::UpdateShot(void)
{
	if (!IsAlive())
	{
		// 生存していなければ処理中断
		return;
	}

	// 弾を移動させる
	//--------------------------------------
	// 移動量の計算(方向×スピード)
	VECTOR movePow;

	movePow = VScale(dir_, speed_);

	// 移動前に前フレーム位置を保存
	prevPos_ = pos_;

	pos_ = VAdd(pos_, movePow);


	// 更に加速度的に重力を加える
	gravityPow_ += GRAVITY / DEFAULT_FPS;

	//pos_.y -= gravityPow_;
	pos_ = VAdd(pos_, VScale({ 0.0f, -1.0f, 0.0f }, gravityPow_));

	// 大きさの設定
	MV1SetScale(modelId_, scl_);

	// 角度の設定
	MV1SetRotationXYZ(modelId_, rot_);

	//位置の設定
	MV1SetPosition(modelId_, pos_);
	
	// カプセルを更新
	UpdateCapsulePos();

	// すり抜け防止（球 vs 線分判定）
	CheckHitSphere();

	//当たり判定チェック
	Colliders();
}

void CannonBase::UpdateBlast(void)
{

	// 爆発時間を加算
	blastTimer_ += SceneManager::GetInstance().GetDeltaTime();
	
	// 1回だけ爆風ダメージ判定
	if (!hasDealtDamage_)
	{
		CheckBlastDamage();
		hasDealtDamage_ = true;
	}

	// Effekseerの再生が終わった、または一定時間経過したら終了
// 再生終了 or 時間経過で爆風終了
	if ((blastEffectPlayId_ >= 0 && IsEffekseer3DEffectPlaying(blastEffectPlayId_) == -1) ||
		blastTimer_ > blastDuration_)
	{
		StopEffekseer3DEffect(blastEffectPlayId_);
		blastEffectPlayId_ = -1;
		ChangeState(STATE::END);
	}
}

void CannonBase::UpdateEnd(void)
{
	isAlive_ = false;
}

void CannonBase::UpdateCapsulePos()
{
	if (!capsule_) return;

	// ローカル座標 + 弾の座標
	capsule_->SetLocalPosTop(VAdd({ 0.0f, CAPSULE_HALF_HEIGHT, 0.0f }, pos_));
	capsule_->SetLocalPosDown(VAdd({ 0.0f, -CAPSULE_HALF_HEIGHT, 0.0f }, pos_));
}

void CannonBase::CheckBlastDamage()
{
	switch (rarity_)
	{
	case CARD_RARITY::BRONZE: DamageRate = 1.0f; break;
	case CARD_RARITY::SILVER: DamageRate = 1.5f; break;
	case CARD_RARITY::GOLD:   DamageRate = 2.0f; break;
	}	
	// Player ダメージ
	if (!player_.expired())
	{
		auto player = player_.lock();
		VECTOR playerPos = player->GetPos();
		float dist = VSize(VSub(playerPos, pos_));
		if (dist <= blastRadius_ * DamageRate)
		{
			player->Damage(static_cast<int>(Damage_ * DamageRate));
		}
	}	
	if (!enemy_.expired())
	{
		auto enemy = enemy_.lock();

		VECTOR enemyPos = enemy->GetPos();

		float blastR = blastRadius_ * DamageRate;
		float enemyR = enemy->GetRadius();   // ゴーレムの半径

		float dist = VSize(VSub(enemyPos, pos_));

		// ★ 爆発球 vs 敵の球　の衝突判定
		if (dist <= blastR + enemyR)
		{
			enemy->Damage(static_cast<int>(Damage_ * DamageRate));
		}
	}
}

void CannonBase::CreateExplosion(const VECTOR& pos)
{
}

void CannonBase::Draw()
{
	switch (state_)
	{
	case CannonBase::STATE::NONE:
		break;
	case CannonBase::STATE::SHOT:
		DrawShot();
		break;
	case CannonBase::STATE::BLAST:
		DrawBlast();
		break;
	case CannonBase::STATE::END:
		DrawEnd();
		break;
	default:
		break;
	}
	  DrawEffekseer3D();

}

void CannonBase::DrawShot()
{
	MV1DrawModel(modelId_);
}

void CannonBase::DrawBlast()
{
}

void CannonBase::DrawEnd()
{
}

bool CannonBase::Release(void)
{
	//MV1DeleteModel(modelId_);
	if (blastEffectPlayId_ >= 0)
	{
		StopEffekseer3DEffect(blastEffectPlayId_);
		blastEffectPlayId_ = -1;
	}
	MV1DeleteModel(modelId_);
	modelId_ = -1;

	return true;
}

bool CannonBase::IsAlive(void)
{
	return isAlive_;
}

bool CannonBase::IsShot(void)
{
	return state_ == STATE::SHOT;
}

VECTOR CannonBase::GetPos(void)
{
	return pos_;
}

float CannonBase::GetRadius(void) const
{
	return radius_;
}

void CannonBase::SetColliders(const std::vector<std::weak_ptr<Collider>>& cols)
{
	colliders_ = cols;
}

void CannonBase::Blast(void)
{
	// 状態遷移
	ChangeState(STATE::BLAST);
}

void CannonBase::ChangeState(STATE state)
{
	if (state_ == state) return;

	state_ = state;

	switch (state_)
	{
	case CannonBase::STATE::NONE:
		break;
	case CannonBase::STATE::SHOT:
		break;
	case CannonBase::STATE::BLAST:
		PlayBlastEffect();
		break;
	case CannonBase::STATE::END:
		break;
	default:
		break;
	}
}

void CannonBase::Colliders(void)
{
	dir_ = VNorm(dir_);
	// ステージとの当たり判定
	for (const auto& c : colliders_)
	{
		if (c.expired()) continue;
		auto collider = c.lock();

		// カプセルとステージの衝突判定
		auto hits = MV1CollCheck_Capsule(
			collider->modelId_,
			-1,
			capsule_->GetLocalPosTop(),
			capsule_->GetLocalPosDown(),
			capsule_->GetRadius()
		);

		// 衝突時
		if (hits.HitNum > 0)
		{
			auto hit = hits.Dim[0];
			pos_ = VAdd(pos_, VScale(hit.Normal, HIT_PUSH_BACK));  // 埋まらないよう少しずらす
			Blast();
			MV1CollResultPolyDimTerminate(hits);
			return;
		}

		MV1CollResultPolyDimTerminate(hits);
	}

	// エネミーに直接命中したか？
	if (!enemy_.expired())
	{
		auto enemy = enemy_.lock();

		// --- 弾（球）
		VECTOR bulletPos = pos_;
		float bulletR = bulletRadius_;

		// --- 敵（カプセル）
		VECTOR eTop = enemy->GetCapsuleTop();
		VECTOR eDown = enemy->GetCapsuleBottom();
		float  eR = enemy->GetRadius();

		// ★ 球（弾） vs カプセル（敵） 判定
		if (AsoUtility::IsHitSphereCapsule(
			bulletPos, bulletR,
			eTop, eDown, eR))
		{
			Blast();
			return;
		}
	}
}

void CannonBase::PlayBlastEffect()
{
	switch (rarity_)
    {
    case CARD_RARITY::BRONZE:
        SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::BLAST_S,
			true,   // ← load を true
			SoundManager::VOLUME_STANDARD
		);
        break;
    case CARD_RARITY::SILVER:
        SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::BLAST_M,
			true,   // ← load を true
			SoundManager::VOLUME_STANDARD
		);
        break;
    case CARD_RARITY::GOLD:
        SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::BLAST_L,
			true,   // ← load を true
			SoundManager::VOLUME_STANDARD
		);
        break;
    }

	//エフェクト再生
	blastEffectPlayId_ = PlayEffekseer3DEffect(blastEffectHandle_);
	float ShotSize = DEFAULT_EFFECT_SCALE * DamageRate;

	//スケール
	SetScalePlayingEffekseer3DEffect(
		blastEffectPlayId_, ShotSize, ShotSize, ShotSize);

	//回転
	SetRotationPlayingEffekseer3DEffect(blastEffectPlayId_, 0.0f, 0.0f, 0.0f);

	//位置
	//VECTOR pos = transform_.pos;
	SetPosPlayingEffekseer3DEffect(blastEffectPlayId_, pos_.x, pos_.y + BLAST_OFFSET_Y, pos_.z);

	// タイマー初期化
	blastTimer_ = 0.0f;
	// ダメージ判定リセット
	hasDealtDamage_ = false;
}

void CannonBase::CheckHitSphere()
{
	// エネミーに直接命中したか？
	if (!enemy_.expired())
	{
		auto enemy = enemy_.lock();

		VECTOR enemyPos = enemy->GetPos();
		float enemyR = enemy->GetRadius();       // 敵の半径
		float bulletR = bulletRadius_;           // 弾の半径 ← すでにあなたが持っている

		float dist = VSize(VSub(enemyPos, pos_));

		// ★ 弾の球 vs 敵の球 の判定
		if (dist <= bulletR + enemyR)
		{
			Blast();
			return;
		}
	}
}

void CannonBase::StopEffect()
{
	if (blastEffectPlayId_ >= 0)
	{
		StopEffekseer3DEffect(blastEffectPlayId_);
		blastEffectPlayId_ = -1;
	}

	isAlive_ = false;
}

void CannonBase::SetPlayer(std::shared_ptr<Player> player)
{
	player_ = player;
}

void CannonBase::SetEnemy(std::weak_ptr<EnemyGolem> enemy)
{
	enemy_ = enemy;
}

bool CannonBase::IsExploding() const
{
	return state_ == STATE::BLAST;
}

VECTOR CannonBase::GetExplosionPos() const
{
	return pos_;
}

float CannonBase::GetExplosionRadius() const
{
	return blastRadius_ * DamageRate;
}

int CannonBase::GetExplosionDamage() const
{
	return static_cast<int>(Damage_ * DamageRate);
}

bool CannonBase::HasDealtDamage() const
{
	return hasDealtDamage_;
}

void CannonBase::MarkDamageDealt()
{
	hasDealtDamage_ = true;
}
