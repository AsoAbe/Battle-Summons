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

	speed_ = 0.0f;
	moveDir_ = AsoUtility::VECTOR_ZERO;
	movePow_ = AsoUtility::VECTOR_ZERO;
	movedPos_ = AsoUtility::VECTOR_ZERO;
	LastPos_ = AsoUtility::VECTOR_ZERO;

	playerRotY_ = Quaternion();
	goalQuaRot_ = Quaternion();
	stepRotTime_ = 0.0f;

	jumpPow_ = AsoUtility::VECTOR_ZERO;
	isJump_ = false;
	stepJump_ = 0.0f;
	MaxHp_ = 0;
	Hp_ = 0;
	invincibleTimer_ = 0;
	alive_ = true;
	
	regenTimer_ = 0.0f;
	regenInterval_ = 0.0f;   // 1秒ごと
	regenAmount_ = 0;        // 1回の回復量

	capsuleOffsetY = 0.0f;
	footOffsetY = 0.0f;

	//// 衝突チェック
	//gravHitPosDown_ = AsoUtility::VECTOR_ZERO;
	//gravHitPosUp_ = AsoUtility::VECTOR_ZERO;

	imgShadow_ = -1;

	// エフェクト関連
	HealEffectHandle_ = -1;
	HealEffectPlayId_ = -1;
	isHealEffectPlaying_ = false;

	capsule_ = nullptr;

	isAttacking_ = false;

	// 状態管理
	stateChanges_.emplace(STATE::NONE, std::bind(&Player::ChangeStateNone, this));
	stateChanges_.emplace(STATE::PLAY, std::bind(&Player::ChangeStatePlay, this));
	stateChanges_.emplace(STATE::DEAD, std::bind(&Player::ChangeStateDead, this));
	
}

Player::~Player(void)
{
}

void Player::Init(void)
{

	// モデルの基本設定
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::PLAYER));
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.pos = { 0.0f, -30.0f, 0.0f };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_.Update();

	// アニメーションの設定
	InitAnimation();

	// カプセルコライダ
	capsule_ = std::make_unique<Capsule>(transform_);
	capsuleOffsetY = 10.0f;
	//footOffsetY = 10.0f;
	//少女
	//capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	//capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	//capsule_->SetRadius(20.0f);	
	//魔法使い
	capsule_->SetLocalPosTop({ 0.0f, 140.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(30.0f);
	transform_.modelOffset = { 0.0f, 45.0f, 0.0f };

	// 丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;


	HealEffectHandle_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::HEAL).handleId_;

	animationController_->Play(static_cast<int>(ANIM_TYPE::RUN));

	// 初期状態
	ChangeState(STATE::PLAY);

	MaxHp_ = MAX_HP;
	Hp_ = MAX_HP;
	//生存判定初期化(保険　タイトルでもしている)
	SceneManager::GetInstance().SetPlayerAlive(true);

	regenInterval_ = 1.0f;
	regenAmount_ = 5;  
}

void Player::Update(void)
{
	// 無敵時間カウントダウン
	float delta = SceneManager::GetInstance().GetDeltaTime();

	// 死亡中
	/*if (isDead_)
	{
		Died(delta);
		return;
	}*/

	UpdateInvincible(delta);

	//持続回復
	UpdateRegen(delta);
	
	// 単発エフェクトの終了確認
	if (isHealEffectPlaying_)
	{
		if (IsEffekseer3DEffectPlaying(HealEffectPlayId_) != 0)
		{
			isHealEffectPlaying_ = false;
			HealEffectPlayId_ = -1;
		}
	}

	//// 更新ステップ
	//stateUpdate_();

	//// モデル制御更新
	//transform_.Update();

	//// アニメーション再生
	//animationController_->Update();

// 攻撃中ならアニメーションだけ固定
	if (isAttacking_)
	{
		attackTimer_ += delta;
		animationController_->Update();

		// 攻撃終了判定
		if (animationController_->IsEnd() || attackTimer_ >= attackDuration_)
		{
			isAttacking_ = false;
			animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
		}
	}
	else
	{
		// 攻撃中でないときだけ、通常アニメや移動制御を更新
		stateUpdate_();
	}

	// 通常更新
	transform_.Update();
	animationController_->Update();

	// 弾の更新・描画
	for (auto& shot : shots_) {
		shot->Update();
	}


	UpdateEffekseer3D();
}

void Player::Draw(void)
{

	// モデルの描画
	MV1DrawModel(transform_.modelId);

	// 丸影描画
	DrawShadow();

#ifdef _DEBUG
	//capsule_->Draw();
#endif
	for (auto& shot : shots_) {
		shot->Draw();
	}
	//DrawFormatString(0, 190, GetColor(0, 255, 0), "Player Pos: %.0f, %.0f, %.0f", GetPos().x, GetPos().y, GetPos().z);
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
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::RUN, path + "Run.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN, path + "FastRun.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::ATTACK, path + "Attack2.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::WARP_PAUSE, path + "WarpPose.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FALLING, path + "Falling.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::DEAD, path + "Down2.mv1", 60.0f);

	animationController_->Play((int)ANIM_TYPE::IDLE);

}

void Player::ChangeState(STATE state)
{

	// 状態変更
	state_ = state;

	// 各状態遷移の初期処理
	stateChanges_[state_]();

}

void Player::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&Player::UpdateNone, this);
}

void Player::ChangeStatePlay(void)
{
	transform_.modelOffset.y = 45.0f;
	stateUpdate_ = std::bind(&Player::UpdatePlay, this);
}

void Player::ChangeStateDead()
{
	transform_.modelOffset.y = 0.0f;
	/*PlayAnimation(ANIM_TYPE::DEAD, false);*/
	animationController_->Play(
		(int)ANIM_TYPE::DEAD,
		false,     // ループしない
		0.0f,
		-1.0f,     // 最後まで
		false,
		false     // ★止めない（Golemと同じ）
	);
	stateUpdate_ = std::bind(&Player::UpdateDead, this);
}

void Player::UpdateNone(void)
{
}

void Player::UpdatePlay(void)
{

	// 移動処理
	ProcessMove();

	// ジャンプ処理
	ProcessJump();

	//ショット
	//ProcessShot();

	// 移動方向に応じた回転
	Rotate();

	// 衝突判定&重力処理
	Collision();

	//UpdatePhysics();

	// 回転させる
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

		PlayHealEffect();   // ★ここで再生
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

	// 位置設定（必須）
	SetPosPlayingEffekseer3DEffect(
		HealEffectPlayId_,
		transform_.pos.x,
		transform_.pos.y + 130.0f,
		transform_.pos.z
	);

	float size = 10.0f;
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
	HealEffectPlayId_ = -1;
	isHealEffectPlaying_ = false;
}

void Player::UpdateInvincible(float deltaTime)
{
	if (invincibleTimer_ > 0.0f)
	{
		invincibleTimer_ -= deltaTime;
		if (invincibleTimer_ < 0.0f)
			invincibleTimer_ = 0.0f;
	}
}


void Player::DrawShadow(void)
{
	if (state_ == STATE::DEAD || !alive_) return;
	float PLAYER_SHADOW_HEIGHT = 300.0f;
	float PLAYER_SHADOW_SIZE = 30.0f;

	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3] = { VERTEX3D(), VERTEX3D(), VERTEX3D() };
	VECTOR SlideVec;
	int ModelHandle;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// 影を落とすモデルの数だけ繰り返し
	for (const auto c : colliders_)
	{

		// チェックするモデルは、jが0の時はステージモデル、1以上の場合はコリジョンモデル
		ModelHandle = c.lock()->modelId_;

		// プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(
			ModelHandle, -1,
			transform_.pos, VAdd(transform_.pos, { 0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f }), PLAYER_SHADOW_SIZE);

		// 頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
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
			SlideVec = VScale(HitRes->Normal, 0.5f);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			// ポリゴンの不透明度を設定する
			Vertex[0].dif.a = 0;
			Vertex[1].dif.a = 0;
			Vertex[2].dif.a = 0;
			if (HitRes->Position[0].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[0].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[0].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[1].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[1].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[1].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[2].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[2].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[2].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			// 影ポリゴンを描画
			DrawPolygon3D(Vertex, 1, imgShadow_, TRUE);
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);

}

void Player::ProcessMove(void)
{
	if (state_ == STATE::DEAD) return;

	auto& ins = InputManager::GetInstance();

	//ATTACKで下げた分を戻す
	transform_.modelOffset.y = 45.0f;

	// 移動量をゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// X軸回転を除いた、重力方向に垂直なカメラ角度(XZ平面)を取得
	Quaternion cameraRot = mainCamera.GetQuaRotOutX();

	// 回転したい角度
	double rotRad = 0;

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	// ゲームパッドが接続数で処理を分ける
	if (GetJoypadNum() == 0)
	{
		// WASDで移動する
		if (ins.IsNew(KEY_INPUT_W)) {
			rotRad = AsoUtility::Deg2RadD(0.0);
			//dir = cameraRot.GetForward();
			dir = VAdd(dir, cameraRot.GetForward());
		}
		if (ins.IsNew(KEY_INPUT_A)) {
			rotRad = AsoUtility::Deg2RadD(270.0);
			//dir = cameraRot.GetLeft();
			dir = VAdd(dir, cameraRot.GetLeft());

		}
		if (ins.IsNew(KEY_INPUT_S)) {
			rotRad = AsoUtility::Deg2RadD(180.0);
			//dir = cameraRot.GetBack();
			dir = VAdd(dir, cameraRot.GetBack());
		}
		if (ins.IsNew(KEY_INPUT_D)) {
			rotRad = AsoUtility::Deg2RadD(90.0);
			//dir = cameraRot.GetRight();
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

		// 移動処理
		//speed_ = SPEED_MOVE;
		//if (ins.IsNew(KEY_INPUT_RSHIFT))
		//{
		//	speed_ = SPEED_RUN;
		//}
		speed_ = SPEED_RUN;
		dir = VNorm(dir);
		moveDir_ = dir;
		movePow_ = VScale(dir, speed_);

		// 回転処理
		//SetGoalRotate(rotRad);

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

	if (ins.IsNew(KEY_INPUT_Z))
	{	
		Hp_ += 1;
		/*CardManager::GetInstance().UseCard(CardManager::EFFECT::HEAL);*/
		if (Hp_ >= MaxHp_)
		{
			Hp_ = MaxHp_;
		}
	
	}
	if (ins.IsNew(KEY_INPUT_C))
	{
		if(Hp_ > 0)
		{
			Hp_ -= 1;
		}
	}
}

void Player::ProcessJump(void)
{
	if (state_ == STATE::DEAD) return;

	bool isHit = CheckHitKey(KEY_INPUT_SPACE);

	SoundManager::GetInstance().PlaySE(
		SoundManager::SOUND_ID::JUMP,
		true,
		SoundManager::VOLUME_STANDARD
	);

	// ジャンプ
	if (isHit && (isJump_ || IsEndLanding()))
	{

		if (!isJump_)
		{

			// 無理やりアニメーション
			animationController_->Play((int)ANIM_TYPE::JUMP, true, 13.0f, 25.0f);
			animationController_->SetEndLoop(23.0f, 25.0f, 5.0f);

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
	if (angleDiff > 0.1)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;

}

void Player::Rotate(void)
{

	playerRotY_ = Quaternion::Slerp(
	transform_.quaRot, Quaternion::LookRotation(moveDir_), 0.1f);

}

//
//void Player::Collision(void)
//{
//
//	// 現在座標を起点に移動後座標を決める
//	movedPos_ = VAdd(transform_.pos, movePow_);
//
//	// 衝突(カプセル)
//	CollisionCapsule();
//
//	// 衝突(重力)
//	CollisionGravity();
//
//
//	// 移動が確定した座標を保存
//	LastPos_ = movedPos_;
//
//	// ---------------------------------------------
//	//  落下判定（高さが -1000 以下 = ステージ外）
//	// ---------------------------------------------
//	if (movedPos_.y < -1000.0f)
//	{
//		// ステージ中心（例として (0,0,0)）方向へ少し寄せる
//		VECTOR stageCenter = VGet(0.0f, 0.0f, 0.0f);
//
//		// LastPos_ → 中心への方向
//		VECTOR dir = VSub(stageCenter, LastPos_);
//		dir = VNorm(dir);
//
//		// ★ 戻す位置＝直前位置 ＋ 中心方向へ 50 だけ寄せる
//		movedPos_ = VAdd(LastPos_, VScale(dir, 100.0f));
//
//		// ★ 少しだけ浮かせて衝突が安定するように
//		movedPos_.y = 5.0f;
//
//		// 重力リセット
//		jumpPow_ = VGet(0, 0, 0);
//	}
//
//
//	// 移動
//	transform_.pos = movedPos_;
//
//}
//
//void Player::CollisionGravity(void)
//{
//
//	// ジャンプ量を加算
//	movedPos_ = VAdd(movedPos_, jumpPow_);
//
//	// 重力方向
//	VECTOR dirGravity = AsoUtility::DIR_D;
//
//	// 重力方向の反対
//	VECTOR dirUpGravity = AsoUtility::DIR_U;
//
//	// 重力の強さ
//	float gravityPow = 100.0f;
//	float checkPow = 10.0f;
//
//	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
//	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
//	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));
//	for (const auto c : colliders_)
//	{
//
//		// 地面との衝突
//		auto hit = MV1CollCheck_Line(
//			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);
//
//		// 最初は上の行のように実装して、木の上に登ってしまうことを確認する
//		//着地判定(高さ)
//		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.0f)
//		{
//
//			// 衝突地点から、少し上に移動
//			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));
//			/*movedPos_.y = -capsuleOffsetY;*/
//
//			// ジャンプリセット
//			jumpPow_ = AsoUtility::VECTOR_ZERO;
//			stepJump_ = 0.0f;
//
//			if (isJump_)
//			{
//				// 着地モーション
//				animationController_->Play(
//					(int)ANIM_TYPE::JUMP, false, 29.0f, 45.0f, false, true);
//			}
//			isJump_ = false;
//		}
//	}
//}
//
//void Player::CollisionCapsule(void)
//{
//
//	// カプセルを移動させる
//	Transform trans = Transform(transform_);
//	trans.pos = movedPos_;
//	trans.Update();
//	Capsule cap = Capsule(*capsule_, trans);
//
//	// カプセルとの衝突判定
//	for (const auto c : colliders_)
//	{
//
//		auto hits = MV1CollCheck_Capsule(
//			c.lock()->modelId_, -1,
//			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());
//
//		for (int i = 0; i < hits.HitNum; i++)
//		{
//
//			auto hit = hits.Dim[i];
//
//			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
//			{
//
//				int pHit = HitCheck_Capsule_Triangle(
//					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
//					hit.Position[0], hit.Position[1], hit.Position[2]);
//
//				if (pHit)
//				{
//					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
//					// カプセルを移動させる
//					trans.pos = movedPos_;
//					trans.Update();
//					continue;
//				}
//				break;
//			}
//		}
//
//		// 検出した地面ポリゴン情報の後始末
//		MV1CollResultPolyDimTerminate(hits);
//	}
//}
//
//void Player::CalcGravityPow(void)
//{
//
//	// 重力方向
//	VECTOR dirGravity = AsoUtility::DIR_D;
//
//	// 重力の強さ
//	float gravityPow = Planet::DEFAULT_GRAVITY_POW;
//
//	// 重力
//	VECTOR gravity = VScale(dirGravity, gravityPow);
//	jumpPow_ = VAdd(jumpPow_, gravity);
//
//	// 最初は実装しない。地面と突き抜けることを確認する。
//	// 内積
//	//float dot = VDot(dirGravity, jumpPow_);
//	//if (dot >= 0.0f)
//	//{
//	//	// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
//	//	jumpPow_ = gravity;
//	//}
//
//}

void Player::OnLanding(const MV1_COLL_RESULT_POLY& hit)
{

	// 衝突地点から、少し上に移動
	//movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));
	movedPos_.y = hit.HitPosition.y + 2.0f;

	// ジャンプリセット
	jumpPow_ = AsoUtility::VECTOR_ZERO;
	//stepJump_ = 0.0f;  Playerで管理
	stepJump_ = 0.0f;
	if (isJump_)
	{
		animationController_->Play(
			(int)ANIM_TYPE::JUMP,
			false, 29.0f, 45.0f, false, true);
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
	//printfDx("Healed! HP = %d / %d\n", (int)Hp_, (int)MaxHp_);
}

bool Player::Damage(int damage)
{
	// 無敵中はダメージ無効
	if (invincibleTimer_ > 0.0f || isDead_) return false;

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
		deadTimer_ = 0.0f;
		gameOverReserved_ = true;

		return true;
	}

	invincibleTimer_ = INVINCIBLE_TIME;
	return true;
}

void Player::Died(float delta)
{
	////判定
	//if (Hp_ <= 0)
	//{
	//	SceneManager::GetInstance().SetPlayerAlive(false);
	//	sceneGame_->GameOver();
	//}
	//deadTimer_ += delta;

	//// 死亡アニメ進行
	//animationController_->Update();
	//transform_.Update();

	//// 死亡アニメ
	//animationController_->Play((int)ANIM_TYPE::DEAD, true,0.0f,60.0f);

	//// ディレイ後にゲームオーバー
	//if (gameOverReserved_ && deadTimer_ >= deadDelay_)
	//{
	//	gameOverReserved_ = false;
	//	SceneManager::GetInstance().SetPlayerAlive(false);
	//	sceneGame_->GameOver();
	//}
}

float Player::GetMaxHp()
{
	return MaxHp_;
}

float Player::GetHp()
{
	return Hp_;
}

void Player::ProcessShot(bool byCard)
{

	deleyShot_ -= SceneManager::GetInstance().GetDeltaTime();
	if (deleyShot_ < 0.0f)
	{
		deleyShot_ = 0.0f;
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
	birthPos.y += 150.f;
	
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
		//// 新しいインスタンスを生成
		//ShotBase* newShot = new ShotBase();
		//newShot->Create(transform_.pos, transform_.GetForward());
		
		auto shot = std::make_unique<Shot>(shared_from_this(),enemy_);
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
	birthPos.y += 150.f;

	// 自機の前方方向を取得
	VECTOR baseDir = transform_.GetForward();

	// 角度オフセット設定（±30度）
	const int num = 5;
	const float angleOffset = DX_PI_F / 18.0f;
	float baseAngle = atan2f(baseDir.x, baseDir.z);

	for (int i = 0; i < num; ++i)
	{
		// 弾の生成フラグ
		bool isCreate = false;

		// 発射角を計算
		float offsetIndex = static_cast<float>(i) - (num - 1) / 2.0f; // -1, 0, +1
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

	// プレイヤーの位置
	VECTOR birthPos = transform_.pos;

	// 発射位置を少し上にずらす
	birthPos.y += 150.f;

	// 自機の前方方向を取得
	VECTOR baseDir = transform_.GetForward();
	baseDir = VNorm(baseDir);

	// 拡散設定
	const int numH = 5;                     // 横方向の弾数
	const int numV = 3;                     // 縦方向の弾段数
	const float angleOffsetH = DX_PI_F / 18.0f; // 横方向 ±10°
	const float angleOffsetV = DX_PI_F / 36.0f; // 縦方向 ±5°

	float baseYaw = atan2f(baseDir.x, baseDir.z); // 水平角
	float basePitch = asinf(baseDir.y);            // 垂直角

	for (int v = 0; v < numV; ++v)
	{
		float offsetPitch = (v - (numV - 1) / 2.0f) * angleOffsetV; // 上下の角度変化

		for (int h = 0; h < numH; ++h)
		{
			// 弾の生成フラグ
			bool isCreate = false;
			float offsetYaw = (h - (numH - 1) / 2.0f) * angleOffsetH; // 左右の角度変化

			// 弾の角度計算（yaw：左右、pitch：上下）
			float yaw = baseYaw + offsetYaw;
			float pitch = basePitch + offsetPitch;

			// 向きベクトル算出（角度→方向ベクトル）
			VECTOR dir;
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

	transform_.modelOffset.y = 10.0f;

	isAttacking_ = true;
	attackTimer_ = 0.0f;

	// 攻撃アニメ再生（ブレンド無し、ループなし）
	PlayAnimation(ANIM_TYPE::ATTACK, false, 0.0f, 60.0f);
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