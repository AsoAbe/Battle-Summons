#include "../Utility/AsoUtility.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
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
	if (movedPos_.y < -1000.0f)
	{
		// ステージ中心（例として (0,0,0)）方向へ少し寄せる
		VECTOR stageCenter = VGet(0.0f, 0.0f, 0.0f);

		// LastPos_ → 中心への方向
		VECTOR dir = VSub(stageCenter, LastPos_);
		dir = VNorm(dir);

		// ★ 戻す位置＝直前位置 ＋ 中心方向へ 50 だけ寄せる
		movedPos_ = VAdd(LastPos_, VScale(dir, 100.0f));

		// ★ 少しだけ浮かせて衝突が安定するように
		movedPos_.y = 5.0f;

		// 重力リセット
		jumpPow_ = VGet(0, 0, 0);
	}

	// 移動
	transform_.pos = movedPos_;

}

void CharacterBase::CollisionGravity(void)
{

	// ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	// 重力の強さ
	float gravityPow = 100.0f;
	float checkPow = 10.0f;

	/*gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));*/

	VECTOR rayStart =
		VAdd(movedPos_, VScale(dirUpGravity, gravityPow + checkPow * 2.0f));

	VECTOR rayEnd =
		VAdd(movedPos_, VScale(dirGravity, checkPow));
	for (const auto c : colliders_)
	{

		// 地面との衝突
		/*auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);*/
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, rayStart, rayEnd);

		// 最初は上の行のように実装して、木の上に登ってしまうことを確認する
		//着地判定(高さ)
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.0f)
		{

			//// 衝突地点から、少し上に移動
			//movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));
			///*movedPos_.y = -capsuleOffsetY;*/

			//// ジャンプリセット
			//jumpPow_ = AsoUtility::VECTOR_ZERO;
			////stepJump_ = 0.0f;  Playerで管理

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
	for (const auto c : colliders_)
	{

		auto hits = MV1CollCheck_Capsule(
			c.lock()->modelId_, -1,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());

		for (int i = 0; i < hits.HitNum; i++)
		{

			auto hit = hits.Dim[i];

			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
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

	// 最初は実装しない。地面と突き抜けることを確認する。
	// 内積
	//float dot = VDot(dirGravity, jumpPow_);
	//if (dot >= 0.0f)
	//{
	//	// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
	//	jumpPow_ = gravity;
	//}

}

void CharacterBase::OnCapsuleHit(const MV1_COLL_RESULT_POLY& hit, const Capsule& cap)
{
	// デフォルト（Player互換）
	VECTOR n = hit.Normal;

	// 今まで通りの処理
	movedPos_ = VAdd(movedPos_, VScale(n, 1.0f));
}

VECTOR CharacterBase::GetCapsuleTop(void) const
{
    return capsule_ ? capsule_->GetPosTop() : VECTOR{ 0,0,0 };
}

VECTOR CharacterBase::GetCapsuleBottom(void) const
{
    return capsule_ ? capsule_->GetPosDown() : VECTOR{ 0,0,0 };
}

float CharacterBase::GetRadius(void) const
{
    return capsule_ ? capsule_->GetRadius() : 0.0f;
}

void CharacterBase::AddCollider(std::weak_ptr<Collider> collider)
{
    colliders_.push_back(collider);
}

void CharacterBase::ClearCollider(void)
{
    colliders_.clear();
}
