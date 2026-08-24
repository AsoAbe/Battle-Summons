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
    // 初期値
    static constexpr float INITIAL_VALUE = 0.0f;

    // 無効なハンドル
    static constexpr int INVALID_HANDLE = -1;

    // 落下判定の限界高さ
    static constexpr float FALL_LIMIT_Y = -1000.0f;

    // ステージ中心座標
    static constexpr float STAGE_CENTER_X = 0.0f;
    static constexpr float STAGE_CENTER_Y = 0.0f;
    static constexpr float STAGE_CENTER_Z = 0.0f;

    // 落下後にステージ中心方向へ戻す距離
    static constexpr float RETURN_DISTANCE = 100.0f;

    // 落下後に設定する高さ
    static constexpr float RESET_HEIGHT_Y = 5.0f;

    // 重力方向への判定距離
    static constexpr float GRAVITY_CHECK_HEIGHT = 100.0f;

    // 地面との衝突判定を行う距離
    static constexpr float GRAVITY_CHECK_DISTANCE = 10.0f;

    // レイ開始位置に追加する距離の倍率
    static constexpr float RAY_EXTRA_DISTANCE = 2.0f;

    // カプセル衝突時の押し出し距離
    static constexpr float CAPSULE_PUSH_DISTANCE = 1.0f;

    // カプセル衝突判定の最大試行回数
    static constexpr int COLLISION_RETRY_COUNT = 10;
    
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

    int debugHitNum_ = 0;
};
