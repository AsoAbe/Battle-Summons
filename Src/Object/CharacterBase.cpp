#include "../Utility/AsoUtility.h"
#include "../Manager/ResourceManager.h"
#include "Common/Transform.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "Planet.h"
#include "CharacterBase.h"

//実装していない変数がある

CharacterBase::CharacterBase()
    : resMng_(ResourceManager::GetInstance())
    , scnMng_(SceneManager::GetInstance())
{
	isGround_ = false;

	LastPos_ = AsoUtility::VECTOR_ZERO;
	movePow_ = AsoUtility::VECTOR_ZERO;
	movedPos_ = AsoUtility::VECTOR_ZERO;
	jumpPow_ = AsoUtility::VECTOR_ZERO;
}

CharacterBase::~CharacterBase() 
{
}

void CharacterBase::Collision(void)
{

	//重力による移動量
	CalcGravityPow();

	// 現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	// 衝突(カプセル)
	CollisionCapsule();

	// 衝突(重力)
	CollisionGravity();

	// 移動が確定した座標を保存
	LastPos_ = movedPos_;

	// ---------------------------------------------
	//  落下判定（高さが -1000 以下 = ステージ外）
	// ---------------------------------------------
	if (movedPos_.y < FALL_LIMIT_Y)
	{
		// ステージ中心へ戻す
		VECTOR stageCenter =
			VGet(STAGE_CENTER_X, STAGE_CENTER_Y, STAGE_CENTER_Z);

		// 現在位置からステージ中心への方向
		VECTOR dir = VSub(stageCenter, LastPos_);
		dir = VNorm(dir);

		// ステージ中心方向へ移動
		movedPos_ = VAdd(
			LastPos_,
			VScale(dir, RETURN_DISTANCE)
		);

		// 少し浮かせて衝突を安定させる
		movedPos_.y = RESET_HEIGHT_Y;

		// 重力・ジャンプ量をリセット
		jumpPow_ = AsoUtility::VECTOR_ZERO;
	}

	// 移動
	transform_.pos = movedPos_;

}

void CharacterBase::CollisionGravity(void)
{
	isGround_ = false;
	// ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	// 重力の強さ
	float gravityPow = GRAVITY_CHECK_HEIGHT;
	float checkPow = GRAVITY_CHECK_DISTANCE;


	VECTOR rayStart =
		VAdd(movedPos_, VScale(dirUpGravity, gravityPow + checkPow * RAY_EXTRA_DISTANCE));

	VECTOR rayEnd =
		VAdd(movedPos_, VScale(dirGravity, checkPow));
	for (const auto& c : colliders_)
	{
		auto collider = c.lock();

		if (!collider)
		{
			continue;
		}

		auto hit = MV1CollCheck_Line(
			collider->modelId_, INVALID_HANDLE, rayStart, rayEnd);
		// 最初は上の行のように実装して、木の上に登ってしまうことを確認する
		//着地判定(高さ)
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.0f)
		{

			//子クラスで上書き↓
			OnLanding(hit);
		}
	}
}

void CharacterBase::CollisionCapsule(void)
{

	// カプセルを移動させる
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);

	// カプセルとの衝突判定
	for (const auto& c : colliders_)
	{
		auto collider = c.lock();

		if (!collider)
		{
			continue;
		}
		auto hits = MV1CollCheck_Capsule(
			collider->modelId_, INVALID_HANDLE,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());

		for (int i = 0; i < hits.HitNum; i++)
		{

			auto hit = hits.Dim[i];

			for (int tryCnt = 0; tryCnt < COLLISION_RETRY_COUNT; tryCnt++)
			{

				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);

				if (pHit)
				{
					OnCapsuleHit(hit, cap);
					//movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
					
					// カプセルを移動させる
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}
				break;
			}
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}

void CharacterBase::CalcGravityPow(void)
{

	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	// 重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

}

void CharacterBase::OnCapsuleHit(const MV1_COLL_RESULT_POLY& hit, const Capsule& cap)
{
	// デフォルト（Player互換）
	VECTOR n = hit.Normal;

	// 今まで通りの処理
	movedPos_ = VAdd(movedPos_, VScale(n, CAPSULE_PUSH_DISTANCE));
}

VECTOR CharacterBase::GetCapsuleTop(void) const
{
    return capsule_ ? capsule_->GetPosTop() : AsoUtility::VECTOR_ZERO;
}

VECTOR CharacterBase::GetCapsuleBottom(void) const
{
    return capsule_ ? capsule_->GetPosDown() : AsoUtility::VECTOR_ZERO;
}

float CharacterBase::GetRadius(void) const
{
    return capsule_ ? capsule_->GetRadius() : INITIAL_VALUE;
}

void CharacterBase::AddCollider(std::weak_ptr<Collider> collider)
{
    colliders_.push_back(collider);
}

void CharacterBase::ClearCollider(void)
{
    colliders_.clear();
}
