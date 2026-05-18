#pragma once
#include "Common/Transform.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "../Manager/SceneManager.h"
#include "../Utility/AsoUtility.h"
#include <memory>
#include <vector>

class ResourceManager;
class SceneManager;

class CharacterBase
{
public:
    CharacterBase();
    virtual ~CharacterBase();

    virtual void Init(void) = 0;
    virtual void Update(void) = 0;
    virtual void Draw(void) = 0;
    virtual bool Release(void) = 0;

    const Transform& GetTransform(void) const { return transform_; }
    const VECTOR& GetPos(void) const { return transform_.pos; }

    // Capsule 共通取得
    VECTOR GetCapsuleTop(void) const;
    VECTOR GetCapsuleBottom(void) const;
    float  GetRadius(void) const;

    // Collider 管理
    void AddCollider(std::weak_ptr<Collider> collider);
    void ClearCollider(void);

protected:
    // 各種マネージャー
    ResourceManager& resMng_;
    SceneManager& scnMng_;

    // モデル制御
    Transform transform_;
    std::unique_ptr<Capsule> capsule_;

    // コリジョン
    std::vector<std::weak_ptr<Collider>> colliders_;

    // 衝突判定
    void Collision(void);
    void CollisionGravity(void);
    void CollisionCapsule(void);

    // 移動量の計算
    void CalcGravityPow(void);
    
    //着地による処理(ポリモフィズム)
    virtual void OnLanding(const MV1_COLL_RESULT_POLY& hit) {}   // 何もしない（デフォルト）
    
    virtual void OnCapsuleHit(const MV1_COLL_RESULT_POLY& hit,const Capsule& cap);

    VECTOR LastPos_;     //最後にいた座標

    VECTOR movePow_;     // 移動量
    VECTOR movedPos_;    // 衝突後の確定候補
    VECTOR jumpPow_;     // 重力・ジャンプ

    bool  isJump_ = false;
    float stepJump_ = 0.0f;

    bool isGround_;      // 接地判定
};
