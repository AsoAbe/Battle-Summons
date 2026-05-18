#pragma once
#include "CharacterBase.h"
#include <DxLib.h>
#include <memory>
#include <unordered_map>
#include <functional>

class GameScene;
class AnimationController;
class Capsule;
class Collider;
class Player;
class CannonBase;

//---------------------------------------------
// 敵：ゴーレム
//---------------------------------------------
class EnemyGolem : public CharacterBase, public std::enable_shared_from_this<EnemyGolem>
{
public:

    // スピード
    static constexpr float SPEED_MOVE = 5.0f;
    static constexpr float SPEED_RUN = 10.0f;

    //最大体力
    static constexpr float MAX_HP = 300.0f;
    
    static constexpr float POW_GRAVITY = 10.0f;

    static constexpr float DEFAULT_FPS = 60.0f;

    // 状態定義
    enum class STATE
    {
        NONE,
        IDLE,
        MOVE,
        ATTACK,
        TACKLE,
        REST,
        DEAD,
    };

    // アニメーションタイプ
    enum class ANIM_TYPE
    {
        IDLE,
        RUN,
        ATTACK,
        TACKLE,
        REST,
        DEAD,
    };

    // コンストラクタ・デストラクタ
    EnemyGolem(GameScene& scene);
    ~EnemyGolem();

    // 基本処理
    void Init() override;
    void Update() override;
    void Draw() override;
    bool Release() override;
   
    // ====== HP 関連 ======
    //体力参照
    float GetMaxHp();
    float GetHp();

    // ダメージ処理
    bool Damage(int value);
    bool IsAlive() const { return alive_; }

    // プレイヤー取得
    std::shared_ptr<Player> GetPlayer() const;

    //// 衝突判定に用いられるコライダ制御
    //void AddCollider(std::weak_ptr<Collider> collider);
    //void ClearCollider(void);
    
	// 衝突用カプセルの取得
	const Capsule& GetCapsule(void) const;

    //プレイヤーの取得
    void SetPlayer(std::shared_ptr<Player> player);
    void SetCannon(std::shared_ptr<CannonBase> cannon);
    std::weak_ptr<CannonBase> GetCannonWeakPtr() const;


    //CharacterBaseにある-----------------
    //VECTOR GetPos() const;
    ////VECTOR GetRot();
    ////カプセル取得用
    //VECTOR GetCapsuleTop() const;
    //VECTOR GetCapsuleBottom() const;
    //float GetRadius()const;

    //VECTOR GetColPos() const;

private:
    // シーン参照
    GameScene& scene_;  
    // アニメーション管理
    std::unique_ptr<AnimationController> animationController_;

    //プレイヤー
    std::weak_ptr<Player> player_;

    //Cannon爆発
    std::weak_ptr<CannonBase> cannon_;

    STATE state_;                                            // 現在の状態
    std::unordered_map<STATE, std::function<void()>> stateChanges_; // 状態遷移表
    //std::unordered_map<TACKLE, std::function<void()>> TacklestateChanges_; // 状態遷移表
    std::function<void()> stateUpdate_;                      // 現在の更新関数
                                                 
    //// 衝突判定に用いられるコライダ
    //std::vector<std::weak_ptr<Collider>> colliders_;
    //std::unique_ptr<Capsule> capsule_;

    float moveSpeed_;        // 移動速度
    float attackRange_;      // 攻撃範囲
    float attackTimer_;      // 攻撃クールタイム
    VECTOR attackPos;

    //後隙用
    float EndTime_;

    // -----------------------------
// タックル関連
// -----------------------------
    bool isTackling_;          // タックル中か
    bool isTackleCharging_;    // チャージ中か
    bool isTackleRunning_;     // 突進中か
    bool tackleHitDone_;       // 判定が有効かどうか、タックル用

    float prevTime_;
    float tackleTimer_;         // 状態遷移用タイマー
    float chargeTime_;          // チャージ時間
    float runTime_ ;            // 突進時間
    float tackleSpeed_;         // 突進速度
    float tackleHitRadius_;
    VECTOR tackleStartPos_;
    VECTOR tackleTargetPos_;

    VECTOR tackleDir_ = VGet(0, 0, 0);     // 突進方向
    VECTOR tackleGoal_ = VGet(0, 0, 0);    // 突進終了地点

    // エフェクト関連
    int chargeEffectHandle_ = -1;         // リソースハンドル
    int chargeEffectPlayId_ = -1;         // 再生中のID
    bool isChargeEffectPlaying_ = false;  // 再生フラグ
   
    //体力関連
    bool alive_;             // 生存フラグ
    float Hp_;               // 現在HP
    float MaxHp_;            // 最大HP
    bool damaged_ = false;
    bool preDamaged_ = false;

    float invincibleTimer_ = 0.0f;

    //死亡関連
    float deadTimer_ = 0.0f;
    float deadDelay_ = 1.5f;   // ← 好きな待ち時間（秒）
    bool gameOverReserved_ = false;

    // 移動量
    //VECTOR movePow_;   //CharacterBaseにある
    VECTOR moveDir_;    
   
    //キャラごとの位置調整
    float capsuleOffsetY;
    float footOffsetY;
    VECTOR prevPos_;

    // ジャンプ判定
    //bool isJump_;            //CharacterBaseにある
    // ジャンプの入力受付時間
    //float stepJump_;

    // 衝突チェック
    //VECTOR gravHitPosDown_;  //CharacterBaseにある
    //VECTOR gravHitPosUp_;

    //地面に接地した場合のジャンプ処理
    void OnLanding(const MV1_COLL_RESULT_POLY& hit);
    
    void OnCapsuleHit(
        const MV1_COLL_RESULT_POLY& hit,
        const Capsule& cap) override;

    // 回転関連
    Quaternion playerRotY_;    // 現在の回転
    Quaternion goalQuaRot_;    // 目標回転
    float stepRotTime_;        // 移動方向

    bool attackHitDone_;        // 判定が有効かどうか、通常攻撃用
    //bool isAttackHitActive_;    // 判定が有効かどうか,汎用
    float attackHitStartTime_;  // 攻撃判定開始時間
    float attackHitEndTime_;    // 攻撃判定終了時間
    float chargeStartTime_;     // チャージ時間

    bool EnemyStart_ = false;


    //重力関係
    //VECTOR jumpPow_;        // 重力・落下ベクトル&移動後の座標 |
    //VECTOR movedPos_;       // 衝突後の補正済み座標            | //CharacterBaseにある
    //float gravity_;         // 重力加速度                      |
    //bool isGround_;         // 地面についているか              |

    // 内部更新
    void ChangeState(STATE state);
    void ChangeStateNone();
    void ChangeStateIdle();
    void ChangeStateMove();
    void ChangeStateAttack();
    void ChangeStateTackle();
    void ChangeStateRest();
    void ChangeStateDead();

    void UpdateNone();
    void UpdateIdle();
    void UpdateMove();
    void UpdateAttack();
    void UpdateRest();
    void DoRest(float value);

    //タックル用
    void UpdateTackle();    // TACKLE 更新処理
    void ChargeTackle();
    void EffectTackle();

    void UpdateDead();

    // モデルロード
    void InitAnimation();

    //プレイヤーの方を向く
    void LookAtPlayer(float rotSpeed = 0.15f);
    void MoveTowardPlayer(float deltaTime);
    void SetGoalRotate(double rotRad);
    void Rotate(float deltaTime);

    //攻撃
    bool CheckAttackHit(float forwardOffset  //前方距離
        , float heightOffset                 //高さ
        , float radius                       //半径
    );    
    void DeactivateAttackHit();
    VECTOR debugAttackPos_;
    //void CheckTackleStart();

    // 衝突判定
    //void Collision(void);        //|
    //void CollisionGravity(void); //|
    //void CollisionCapsule(void); //| //CharacterBaseにある
    //                             //|
    //// 移動量の計算              //|
    //void CalcGravityPow(void);   //|

    VECTOR capsuleTopLocal_;      // ローカル座標の上端
    VECTOR capsuleBottomLocal_;   // ローカル座標の下端
    float  capsuleRadius_;

    //影
    void DrawShadow(void);
    int imgShadow_;
};