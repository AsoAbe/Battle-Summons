#include <string>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/CardManager.h"
#include "../Manager/Camera.h"
#include "../Scene/GameScene.h"
#include "../Card/Shot.h"
#include "../Card/CardBase.h"
#include "Common/AnimationController.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "Planet.h"
#include "Player.h"


Player::Player(void)
{

	animationController_ = nullptr;
	sceneGame_ = nullptr;
	card_ = nullptr;
	state_ = STATE::NONE;

	speed_ = INITIAL_FLOAT;
	moveDir_ = AsoUtility::VECTOR_ZERO;
	movePow_ = AsoUtility::VECTOR_ZERO;
	movedPos_ = AsoUtility::VECTOR_ZERO;
	LastPos_ = AsoUtility::VECTOR_ZERO;

	playerRotY_ = Quaternion();
	goalQuaRot_ = Quaternion();
	stepRotTime_ = INITIAL_FLOAT;

	jumpPow_ = AsoUtility::VECTOR_ZERO;
	isJump_ = false;
	stepJump_ = INITIAL_FLOAT;
	MaxHp_ = INITIAL_INT;
	Hp_ = INITIAL_INT;
	invincibleTimer_ = INITIAL_INT;
	alive_ = true;
	
	regenTimer_ = INITIAL_FLOAT;
	regenInterval_ = INITIAL_FLOAT;   //1秒ごと
	regenAmount_ = INITIAL_INT;        //1回の回復量

	capsuleOffsetY = INITIAL_FLOAT;
	footOffsetY = INITIAL_FLOAT;

	imgShadow_ = INVALID_HANDLE;

	//エフェクト関連
	HealEffectHandle_ = INVALID_HANDLE;
	HealEffectPlayId_ = INVALID_HANDLE;
	isHealEffectPlaying_ = false;

	capsule_ = nullptr;

	isAttacking_ = false;
	attackTimer_ = INITIAL_FLOAT;

	deleyShot_ = INITIAL_FLOAT;
	TIME_DELAY_SHOT = INITIAL_FLOAT;

	damaged_ = false;
	preDamaged_ = false;

	//状態管理
	stateChanges_.emplace(STATE::NONE, std::bind(&Player::ChangeStateNone, this));
	stateChanges_.emplace(STATE::PLAY, std::bind(&Player::ChangeStatePlay, this));
	stateChanges_.emplace(STATE::DEAD, std::bind(&Player::ChangeStateDead, this));
	
}

Player::~Player(void)
{
}

void Player::Init(void)
{

	//モデルの基本設定
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::PLAYER));
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.pos = { INITIAL_FLOAT, -START_POS_Y, INITIAL_FLOAT };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ INITIAL_FLOAT, AsoUtility::Deg2RadF(MODEL_ROT_Y), INITIAL_FLOAT });
	transform_.Update();

	//アニメーションの設定
	InitAnimation();

	//カプセルコライダ
	capsule_ = std::make_unique<Capsule>(transform_);
	capsuleOffsetY = CAPSULE_OFFSET_Y;
	
	//魔法使い
	capsule_->SetLocalPosTop({ INITIAL_FLOAT, CAPSULE_TOP_Y, INITIAL_FLOAT });
	capsule_->SetLocalPosDown({ INITIAL_FLOAT, CAPSULE_BOTTOM_Y, INITIAL_FLOAT });
	capsule_->SetRadius(CAPSULE_RADIUS);
	transform_.modelOffset = { INITIAL_FLOAT, MODEL_OFFSET_Y, INITIAL_FLOAT };

	//丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;


	HealEffectHandle_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::HEAL).handleId_;

	animationController_->Play(static_cast<int>(ANIM_TYPE::RUN));

	//初期状態
	ChangeState(STATE::PLAY);

	MaxHp_ = MAX_HP;
	Hp_ = MAX_HP;
	//生存判定初期化(保険　タイトルでもしている)
	SceneManager::GetInstance().SetPlayerAlive(true);

	regenInterval_ = REGEN_INTERVAL;
	regenAmount_ = REGEN_AMOUNT;
}

void Player::Update(void)
{
	//無敵時間カウントダウン
	float delta = SceneManager::GetInstance().GetDeltaTime();

	UpdateInvincible(delta);

	//持続回復
	UpdateRegen(delta);
	
	//単発エフェクトの終了確認
	if (isHealEffectPlaying_)
	{
		if (IsEffekseer3DEffectPlaying(HealEffectPlayId_) != 0)
		{
			isHealEffectPlaying_ = false;
			HealEffectPlayId_ = INVALID_HANDLE;
		}
	}


	//攻撃中ならアニメーションだけ固定
	if (isAttacking_)
	{
		attackTimer_ += delta;
		animationController_->Update();

		//攻撃終了判定
		if (animationController_->IsEnd() || attackTimer_ >= ATTACK_DURATION)
		{
			isAttacking_ = false;
			animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), ANIM_LOOP);
		}
	}
	else
	{
		//攻撃中でないときだけ、通常アニメや移動制御を更新
		stateUpdate_();
	}

	//通常更新
	transform_.Update();
	animationController_->Update();

	//弾の更新・描画
	for (auto& shot : shots_) {
		shot->Update();
	}


	UpdateEffekseer3D();
}

void Player::Draw(void)
{

	//モデルの描画
	MV1DrawModel(transform_.modelId);

	//丸影描画
	DrawShadow();

#ifdef _DEBUG
	//capsule_->Draw();
#endif
	for (auto& shot : shots_) {
		shot->Draw();
	}
	DrawEffekseer3D();
}

bool Player::Release()
{
	return false;
}

const Capsule& Player::GetCapsule(void) const
{
	return *capsule_;
}

void Player::InitAnimation(void)
{

	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", ANIM_SPEED_NORMAL);
	animationController_->Add((int)ANIM_TYPE::RUN, path + "Run.mv1", ANIM_SPEED_NORMAL);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN, path + "FastRun.mv1", ANIM_SPEED_NORMAL);
	animationController_->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", ANIM_SPEED_ACTION);
	animationController_->Add((int)ANIM_TYPE::ATTACK, path + "Attack2.mv1", ANIM_SPEED_ACTION);
	animationController_->Add((int)ANIM_TYPE::WARP_PAUSE, path + "WarpPose.mv1", ANIM_SPEED_ACTION);
	animationController_->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", ANIM_SPEED_ACTION);
	animationController_->Add((int)ANIM_TYPE::FALLING, path + "Falling.mv1", ANIM_SPEED_FALL);
	animationController_->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", ANIM_SPEED_ACTION);
	animationController_->Add((int)ANIM_TYPE::DEAD, path + "Down2.mv1", ANIM_SPEED_ACTION);

	animationController_->Play((int)ANIM_TYPE::IDLE);

}

void Player::ChangeState(STATE state)
{

	//状態変更
	state_ = state;

	//各状態遷移の初期処理
	stateChanges_[state_]();

}

void Player::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&Player::UpdateNone, this);
}

void Player::ChangeStatePlay(void)
{
	transform_.modelOffset.y = MODEL_OFFSET_Y;
	stateUpdate_ = std::bind(&Player::UpdatePlay, this);
}

void Player::ChangeStateDead()
{
	transform_.modelOffset.y = MODEL_OFFSET_DEAD_Y;
	animationController_->Play(
		(int)ANIM_TYPE::DEAD,
		ANIM_NO_LOOP,			 //ループしない
		ANIM_START_FRAME,
		ANIM_END_FRAME_AUTO,     //最後まで
		false,
		false					 //止めない（Golemと同じ）
	);
	stateUpdate_ = std::bind(&Player::UpdateDead, this);
}

void Player::UpdateNone(void)
{
}

void Player::UpdatePlay(void)
{

	//移動処理
	ProcessMove();

	//ジャンプ処理
	ProcessJump();

	//移動方向に応じた回転
	Rotate();

	//衝突判定&重力処理
	Collision();

	//回転させる
	transform_.quaRot = playerRotY_;

}

void Player::UpdateRegen(float deltaTime)
{
	if (!alive_) return;

	if (Hp_ >= MaxHp_)
	{
		StopHealEffect();   //満タンで止める
		return;
	}

	regenTimer_ += deltaTime;

	if (regenTimer_ >= regenInterval_)
	{
		regenTimer_ -= regenInterval_;

		Hp_ += regenAmount_;
		if (Hp_ > MaxHp_)
		{
			Hp_ = MaxHp_;
		}

		PlayHealEffect();   //ここで再生
	}
}

void Player::UpdateDead()
{
	float delta = SceneManager::GetInstance().GetDeltaTime();
	deadTimer_ += delta;

	if (gameOverReserved_ && deadTimer_ >= deadDelay_)
	{
		gameOverReserved_ = false;
		SceneManager::GetInstance().SetPlayerAlive(false);
		sceneGame_->GameOver();
	}
}

void Player::PlayHealEffect(void)
{
	// もし前回の再生がまだ生きてたら再生しない（保険）
	if (isHealEffectPlaying_)
	{
		if (IsEffekseer3DEffectPlaying(HealEffectPlayId_) == 0)
		{
			return;
		}
	}

	// 再生
	HealEffectPlayId_ = PlayEffekseer3DEffect(HealEffectHandle_);

	// エフェクト位置設定
	SetPosPlayingEffekseer3DEffect(
		HealEffectPlayId_,
		transform_.pos.x,
		transform_.pos.y + HEAL_EFFECT_OFFSET_Y,
		transform_.pos.z
	);

	float size = HEAL_EFFECT_SCALE;
	//大きさ
	SetScalePlayingEffekseer3DEffect(
		HealEffectPlayId_,
		size,
		size,
		size
	);

	isHealEffectPlaying_ = true;
}

void Player::StopHealEffect(void)
{
	if (!isHealEffectPlaying_) return;

	StopEffekseer3DEffect(HealEffectPlayId_);
	HealEffectPlayId_ = INVALID_HANDLE;
	isHealEffectPlaying_ = false;
}

void Player::UpdateInvincible(float deltaTime)
{
	if (invincibleTimer_ > INITIAL_FLOAT)
	{
		invincibleTimer_ -= deltaTime;
		if (invincibleTimer_ < INITIAL_FLOAT)
			invincibleTimer_ = INITIAL_FLOAT;
	}
}


void Player::DrawShadow(void)
{
	if (state_ == STATE::DEAD || !alive_) return;

	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[SHADOW_VERTEX_NUM] = { VERTEX3D(), VERTEX3D(), VERTEX3D() };
	VECTOR SlideVec;
	int ModelHandle;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// 影を落とすモデルの数だけ繰り返し
	for (const auto& c : colliders_)
	{

		// チェックするモデルは、jが0の時はステージモデル、1以上の場合はコリジョンモデル
		ModelHandle = c.lock()->modelId_;

		// プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(
			ModelHandle, -1,
			transform_.pos, VAdd(transform_.pos, { INITIAL_FLOAT, -SHADOW_HEIGHT, INITIAL_FLOAT }), SHADOW_SIZE);

		// 頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(COLOR_MAX, COLOR_MAX, COLOR_MAX, COLOR_MAX);
		Vertex[0].spc = GetColorU8(COLOR_MIN, COLOR_MIN, COLOR_MIN, COLOR_MIN);
		Vertex[0].su = INITIAL_FLOAT;
		Vertex[0].sv = INITIAL_FLOAT;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		// 球の直下に存在するポリゴンの数だけ繰り返し
		HitRes = HitResDim.Dim;
		for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			// ポリゴンの座標は地面ポリゴンの座標
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			// ちょっと持ち上げて重ならないようにする
			SlideVec = VScale(HitRes->Normal, SHADOW_SLIDE_LENGTH);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			//影の透明度設定
			Vertex[0].dif.a = GetShadowAlpha(HitRes->Position[0].y);
			Vertex[1].dif.a = GetShadowAlpha(HitRes->Position[1].y);
			Vertex[2].dif.a = GetShadowAlpha(HitRes->Position[2].y);

			// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (SHADOW_SIZE * SHADOW_DIAMETER_SCALE) +SHADOW_UV_CENTER;
			Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (SHADOW_SIZE * SHADOW_DIAMETER_SCALE) + SHADOW_UV_CENTER;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (SHADOW_SIZE * SHADOW_DIAMETER_SCALE) + SHADOW_UV_CENTER;
			Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (SHADOW_SIZE * SHADOW_DIAMETER_SCALE) + SHADOW_UV_CENTER;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (SHADOW_SIZE * SHADOW_DIAMETER_SCALE) + SHADOW_UV_CENTER;
			Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (SHADOW_SIZE * SHADOW_DIAMETER_SCALE) + SHADOW_UV_CENTER;

			// 影ポリゴンを描画
			DrawPolygon3D(Vertex, SHADOW_POLYGON_NUM, imgShadow_, TRUE);
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);

}

int Player::GetShadowAlpha(float posY) const
{
	//影の範囲外
	if (posY <= transform_.pos.y - SHADOW_HEIGHT)
	{
		return COLOR_MIN;
	}

	float heightRate =
		SHADOW_ALPHA_RATE_BASE - fabs(posY - transform_.pos.y) / SHADOW_HEIGHT;

	return static_cast<int>(
		roundf(SHADOW_ALPHA_MAX * heightRate)
		);
}

void Player::ProcessMove(void)
{
	if (state_ == STATE::DEAD) return;

	auto& ins = InputManager::GetInstance();

	//ATTACKで下げた分を戻す
	transform_.modelOffset.y = MODEL_OFFSET_Y;

	// 移動量をゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// X軸回転を除いた、重力方向に垂直なカメラ角度(XZ平面)を取得
	Quaternion cameraRot = mainCamera.GetQuaRotOutX();

	// 回転したい角度
	double rotRad = MOVE_ROT_FRONT;

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	// ゲームパッドが接続数で処理を分ける
	if (GetJoypadNum() == 0)
	{
		// WASDで移動する
		if (ins.IsNew(KEY_MOVE_FRONT)) {
			rotRad = AsoUtility::Deg2RadD(MOVE_ROT_FRONT);//前
			dir = VAdd(dir, cameraRot.GetForward());
		}
		if (ins.IsNew(KEY_MOVE_LEFT)) {
			rotRad = AsoUtility::Deg2RadD(MOVE_ROT_LEFT);//左
			dir = VAdd(dir, cameraRot.GetLeft());

		}
		if (ins.IsNew(KEY_MOVE_BACK)) {
			rotRad = AsoUtility::Deg2RadD(MOVE_ROT_BACK);//後
			dir = VAdd(dir, cameraRot.GetBack());
		}
		if (ins.IsNew(KEY_MOVE_RIGHT)) {
			rotRad = AsoUtility::Deg2RadD(MOVE_ROT_RIGHT);//右
			dir = VAdd(dir, cameraRot.GetRight());
		}
	}
	else
	{
		// 接続されているゲームパッド１の情報を取得
		InputManager::JOYPAD_IN_STATE padState =
			ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
		// アナログキーの入力値から方向を取得
		dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

		dir = cameraRot.PosAxis(dir);

	}

	if (!AsoUtility::EqualsVZero(dir) && (isJump_ || IsEndLanding())) {

		
		speed_ = SPEED_RUN;
		dir = VNorm(dir);
		moveDir_ = dir;
		movePow_ = VScale(dir, speed_);

		// 回転処理

		if (!isJump_ && IsEndLanding())
		{
			animationController_->Play((int)ANIM_TYPE::FAST_RUN);
		}
	}
	else
	{
		if (!isJump_ && IsEndLanding())
		{
			animationController_->Play((int)ANIM_TYPE::IDLE);
		}
	}

	//デバッグ用-------------------------
	//if (ins.IsNew(KEY_HEAL))
	//{	
	//	Hp_ += 1;
	//	if (Hp_ >= MaxHp_)
	//	{
	//		Hp_ = MaxHp_;
	//	}
	//
	//}
	//if (ins.IsNew(KEY_DAMAGE))
	//{
	//	if(Hp_ > 0)
	//	{
	//		Hp_ -= 1;
	//	}
	//}
}

void Player::ProcessJump(void)
{
	if (state_ == STATE::DEAD) return;

	bool isHit = CheckHitKey(KEY_JUMP);


	// ジャンプ
	if (isHit && (isJump_ || IsEndLanding()))
	{

		if (!isJump_)
		{

			SoundManager::GetInstance().PlaySE(
				SoundManager::SOUND_ID::JUMP,
				true,
				SoundManager::VOLUME_STANDARD
			);

			// 無理やりアニメーション
			animationController_->Play((int)ANIM_TYPE::JUMP,
				ANIM_LOOP,
				JUMP_ANIM_START,
				JUMP_ANIM_END
			);

			animationController_->SetEndLoop(
				JUMP_LOOP_START,
				JUMP_LOOP_END,
				LANDING_BLEND_TIME
			);

			jumpPow_.y = POW_JUMP;
		}

		isJump_ = true;

		// ジャンプの入力受付時間をヘラス
		stepJump_ += scnMng_.GetDeltaTime();
		if (stepJump_ < TIME_JUMP_IN)
		{
			jumpPow_ = VScale(AsoUtility::DIR_U, POW_JUMP);
		}
	}

	// ボタンを離したらジャンプ力に加算しない
	if (!isHit)
	{
		stepJump_ = TIME_JUMP_IN;
	}

}

void Player::SetGoalRotate(double rotRad)
{

	VECTOR cameraRot = mainCamera.GetAngles();
	Quaternion axis = Quaternion::AngleAxis((double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);

	// 現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	// しきい値
	if (angleDiff > ROTATE_LERP_SPEED)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;

}

void Player::Rotate(void)
{

	playerRotY_ = Quaternion::Slerp(
	transform_.quaRot,
		Quaternion::LookRotation(moveDir_),
		ROTATE_LERP_SPEED
	);

}

void Player::OnLanding(const MV1_COLL_RESULT_POLY& hit)
{

	// 衝突地点から、少し上に移動
	movedPos_.y = hit.HitPosition.y + LANDING_OFFSET_Y;

	// ジャンプリセット
	jumpPow_ = AsoUtility::VECTOR_ZERO;
	//stepJump_ = INITIAL_FLOAT;  Playerで管理
	stepJump_ = INITIAL_FLOAT;
	if (isJump_)
	{
		animationController_->Play(
			(int)ANIM_TYPE::JUMP,
			ANIM_NO_LOOP, LANDING_ANIM_START, LANDING_ANIM_END, false, true);
	}

	isJump_ = false;
	
}

bool Player::IsEndLanding(void)
{

	bool ret = true;

	// アニメーションがジャンプではない
	if (animationController_->GetPlayType() != (int)ANIM_TYPE::JUMP)
	{
		return ret;
	}

	// アニメーションが終了しているか
	if (animationController_->IsEnd())
	{
		return ret;
	}

	return false;

}

void Player::Heal(int Value)
{
	Hp_ += Value;
	if (Hp_ > MaxHp_) 
	{
		Hp_ = MaxHp_;
	}
}

bool Player::Damage(int damage)
{
	// 無敵中はダメージ無効
	if (invincibleTimer_ > INITIAL_FLOAT || isDead_) return false;

	SoundManager::GetInstance().PlaySE(
		SoundManager::SOUND_ID::HIT,
		true,
		SoundManager::VOLUME_STANDARD
	);

	Hp_ -= damage;

	if (Hp_ <= 0)
	{
		Hp_ = 0;
		alive_ = false;
		isDead_ = true;

		ChangeState(STATE::DEAD);
		// ディレイ開始
		deadTimer_ = INITIAL_FLOAT;
		gameOverReserved_ = true;

		return true;
	}

	invincibleTimer_ = INVINCIBLE_TIME;
	return true;
}

void Player::Died(float delta)
{
}

float Player::GetMaxHp() const
{
	return static_cast<float>(MaxHp_);
}

float Player::GetHp() const
{
	return static_cast<float>(Hp_);
}

void Player::ProcessShot(bool byCard)
{

	deleyShot_ -= SceneManager::GetInstance().GetDeltaTime();
	if (deleyShot_ < INITIAL_FLOAT)
	{
		deleyShot_ = INITIAL_FLOAT;
	}

	auto& ins = InputManager::GetInstance();

}

void Player::CreateShot(void)
{
	CardManager::GetInstance();
}

void Player::CreateBronzeShot(void)
{
	
	//プレイヤーの位置
	VECTOR birthPos = transform_.pos;

	//発射位置を少し上にずらす
	birthPos.y += SHOT_SPAWN_OFFSET_Y;
	
	// 弾の生成フラグ
	bool isCreate = false;

	SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::SHOT_S);
	for (auto& v : shots_)
	{
		if (v->GetState() == ShotBase::STATE::END)
		{
			v->SetRarity(CardBase::CARD_RARITY::BRONZE);
			// 以前に生成したインスタンスを使い回し
			v->Create(birthPos, transform_.GetForward());
			isCreate = true;
			break;
		}
	}

	if (!isCreate)
	{
		//// 自機の前方方向
		auto dir = transform_.GetForward();
		
		auto shot = std::make_unique<Shot>(shared_from_this(), enemy_);
		shot->SetEnemy(enemy_);
		shot->SetRarity(CardBase::CARD_RARITY::BRONZE);
		shot->Create(birthPos, transform_.GetForward());
		// 弾の管理配列に追加
		shots_.push_back(std::move(shot));
	}
}

void Player::CreateSilverShot(void)
{

	// プレイヤーの位置
	VECTOR birthPos = transform_.pos;

	// 発射位置を少し上にずらす
	birthPos.y += SHOT_SPAWN_OFFSET_Y;

	// 自機の前方方向を取得
	VECTOR baseDir = transform_.GetForward();

	// 角度オフセット設定（±30度）
	const int num = SILVER_SHOT_NUM;
	const float angleOffset = SILVER_SHOT_ANGLE;
	float baseAngle = atan2f(baseDir.x, baseDir.z);

	for (int i = 0; i < num; ++i)
	{
		// 弾の生成フラグ
		bool isCreate = false;

		// 発射角を計算
		float offsetIndex = static_cast<float>(i) - (num - SHOT_CENTER_ADJUST) / SHOT_CENTER_OFFSET; // -1, 0, +1
		float angle = baseAngle + offsetIndex * angleOffset;

		// 向きベクトル算出
		VECTOR dir = VGet(sinf(angle), baseDir.y, cosf(angle));
		dir = VNorm(dir);

		// 既存弾の再利用チェック
		for (auto& v : shots_)
		{
			if (v->GetState() == ShotBase::STATE::END)
			{
				v->SetRarity(CardBase::CARD_RARITY::SILVER);
				v->Create(birthPos, dir);
				isCreate = true;
				break;
			}
		}

		if (!isCreate)
		{
			// 新しい弾を生成して登録
			auto shot = std::make_unique<Shot>(shared_from_this(), enemy_);
			shot->SetEnemy(enemy_);
			shot->SetRarity(CardBase::CARD_RARITY::SILVER);
			shot->Create(birthPos, dir);
			shots_.push_back(std::move(shot));
		}
	}
}

void Player::CreateGoldShot(void)
{

	//プレイヤーの位置
	VECTOR birthPos = transform_.pos;

	//発射位置を少し上にずらす
	birthPos.y += SHOT_SPAWN_OFFSET_Y;

	//自機の前方方向を取得
	VECTOR baseDir = transform_.GetForward();
	baseDir = VNorm(baseDir);

	//拡散設定
	const int numH = GOLD_SHOT_NUM_H;             // 横方向の弾数
	const int numV = GOLD_SHOT_NUM_V;             // 縦方向の弾段数
	const float angleOffsetH = GOLD_SHOT_ANGLE_H; // 横方向 ±10°
	const float angleOffsetV = GOLD_SHOT_ANGLE_V; // 縦方向 ±5°

	float baseYaw = atan2f(baseDir.x, baseDir.z); // 水平角
	float basePitch = asinf(baseDir.y);           // 垂直角

	for (int v = 0; v < numV; ++v)
	{
		float offsetPitch = (v - (numV - SHOT_CENTER_ADJUST) / SHOT_CENTER_OFFSET) * angleOffsetV; // 上下の角度変化

		for (int h = 0; h < numH; ++h)
		{
			// 弾の生成フラグ
			bool isCreate = false;
			float offsetYaw = (h - (numH - SHOT_CENTER_ADJUST) / SHOT_CENTER_OFFSET) * angleOffsetH; // 左右の角度変化

			// 弾の角度計算（yaw：左右、pitch：上下）
			float yaw = baseYaw + offsetYaw;
			float pitch = basePitch + offsetPitch;

			// 向きベクトル算出（角度→方向ベクトル）
			VECTOR dir = AsoUtility::VECTOR_ZERO;
			dir.x = sinf(yaw) * cosf(pitch);
			dir.y = sinf(pitch);
			dir.z = cosf(yaw) * cosf(pitch);
			dir = VNorm(dir);

			// 既存弾の再利用チェック
			for (auto& v : shots_)
			{
				if (v->GetState() == ShotBase::STATE::END)
				{
					// 以前に生成したインスタンスを使い回し
					v->SetRarity(CardBase::CARD_RARITY::GOLD);
					v->Create(birthPos, dir);
					isCreate = true;
					break;
				}
			}

			if (!isCreate)
			{
				// 新しい弾を生成して登録
				auto shot = std::make_unique<Shot>(shared_from_this(), enemy_);
				shot->SetEnemy(enemy_);
				shot->SetRarity(CardBase::CARD_RARITY::GOLD);
				shot->Create(birthPos, dir);
				shots_.push_back(std::move(shot));
			}
		}
	}
}

void Player::PlayAnimation(ANIM_TYPE animType, bool loop, float startFrame, float endFrame)
{
    if (animationController_)
    {
        animationController_->Play(
            static_cast<int>(animType),
            loop,         // ループ再生かどうか
            startFrame,   // 開始フレーム
            endFrame,     // 終了フレーム
            false,        // ブレンドなし
            true          // 再生完了で停止
        );
    }
}

void Player::StartAttack()
{
	if (state_ == STATE::DEAD) return;
	if (isAttacking_) return;  // 攻撃中なら無視

	transform_.modelOffset.y = ATTACK_MODEL_OFFSET_Y;

	isAttacking_ = true;
	attackTimer_ = INITIAL_FLOAT;

	// 攻撃アニメ再生（ブレンド無し、ループなし）
	PlayAnimation(ANIM_TYPE::ATTACK, false, ANIM_START_FRAME, ANIM_SPEED_ACTION);
}

void Player::EndAttack()
{
	isAttacking_ = false;
}

VECTOR Player::GetPos()
{
	return transform_.pos;
}

VECTOR Player::GetRot()
{
	return playerRotY_.ToEuler();
}

bool Player::IsAlive() const
{
	return alive_;
}

VECTOR Player::GetForward() const
{
	return transform_.GetForward();
}

void Player::SetEnemy(std::shared_ptr<EnemyGolem> enemy)
{
	enemy_ = enemy;
}