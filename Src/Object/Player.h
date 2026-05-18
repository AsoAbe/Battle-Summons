#pragma once
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <DxLib.h>
#include "CharacterBase.h"
class AnimationController;
class Collider;
class Capsule;
class GameScene;
class Shot;
class CardManager;
class EnemyGolem;

class Player : public CharacterBase, public std::enable_shared_from_this<Player>
{

public:

	// スピード
	static constexpr float SPEED_MOVE = 5.0f;
	static constexpr float SPEED_RUN = 10.0f;

	// 回転完了までの時間
	static constexpr float TIME_ROT = 1.0f;

	// ジャンプ力
	static constexpr float POW_JUMP = 10.0f;

	// ジャンプ受付時間
	static constexpr float TIME_JUMP_IN = 0.5f;

	//最大体力
	static constexpr float MAX_HP = 100.0f;

	//ダメージ受けた時の無敵時間
	static constexpr float INVINCIBLE_TIME = 0.2f;

	// 状態
	enum class STATE
	{
		NONE,
		PLAY,
		ATTACK,
		WARP_RESERVE,
		WARP_MOVE,
		DEAD,
		VICTORY,
		END
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		JUMP,
		ATTACK,
		WARP_PAUSE,
		FLY,
		FALLING,
		VICTORY,
		DEAD
	};

	// コンストラクタ
	Player(void);

	// デストラクタ
	~Player(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool Release() override;

	// 衝突用カプセルの取得
	const Capsule& GetCapsule(void) const;

	//体力参照
	float GetMaxHp();
	float GetHp();

	void Heal(int Value);
	bool Damage(int Value);

	/// 死亡時の処理
/// </summary>
	void Died(float delta);

	// 無敵時間更新
	void UpdateInvincible(float deltaTime);

	// 生存判定
	bool IsAlive() const;

	// 操作：弾発射
	void ProcessShot(bool byCard = false);
	// 自機の弾を発射
	void CreateShot(void);
	void CreateBronzeShot(void);
	void CreateSilverShot(void);
	void CreateGoldShot(void);

	VECTOR GetPos();
	VECTOR GetRot();
	
	VECTOR GetForward() const;

	void PlayAnimation(ANIM_TYPE animType, bool loop = false, float startFrame = 0.0f, float endFrame = 40.0f);
	void StartAttack();  // ← 攻撃開始
	void EndAttack();    //攻撃終了
	bool IsAttacking() const { return isAttacking_; }

	void SetEnemy(std::shared_ptr<EnemyGolem> enemy);
private:

	// アニメーション
	std::unique_ptr<AnimationController> animationController_;
	
	GameScene* sceneGame_;
	CardManager* card_;
	// 状態管理
	STATE state_;

	std::shared_ptr<EnemyGolem> enemy_;
	// 状態管理(状態遷移時初期処理)
	std::map<STATE, std::function<void(void)>> stateChanges_;
	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;

	// 移動スピード
	float speed_;

	// 移動方向
	VECTOR moveDir_;

	//// 移動量
	//VECTOR movePow_;  　　//CharacterBaseにある

	//// 移動後の座標
	//VECTOR movedPos_;　　//CharacterBaseにある

	// 回転
	Quaternion playerRotY_;
	Quaternion goalQuaRot_;
	float stepRotTime_;

	// ジャンプ量
	//VECTOR jumpPow_;

//体力関係---------
	// 
	//最大体力
	int MaxHp_;
	//残り体力
	int Hp_;
	//そのフレームでダメージを受けた
	bool damaged_;
	//直前のダメージ判定
	bool preDamaged_;
	//無敵時間
	float invincibleTimer_;
	bool alive_;
	//持続回復
	void UpdateRegen(float delta);

	void UpdateDead();

	float regenTimer_ = 0.0f;
	float regenInterval_ = 1.0f;   // 1秒ごと
	int   regenAmount_ = 5;        // 1回の回復量

	// エフェクト関連
	int HealEffectHandle_ = -1;      // リソースハンドル
	int HealEffectPlayId_ = -1;      // 再生中のID
	bool isHealEffectPlaying_ = false;  // 再生フラグ
	void PlayHealEffect(void);
	void StopHealEffect(void);
//--------------------------------------

	//死亡関連
	float deadTimer_ = 0.0f;
	float deadDelay_ = 2.5f;     // ゴーレムと同じ
	bool gameOverReserved_ = false;
	bool isDead_ = false;

	// ジャンプ判定
	bool isJump_;

	// ジャンプの入力受付時間
	float stepJump_;

	//キャラごとの位置調整
	float capsuleOffsetY;
	float footOffsetY;

	//ショット関係
	 // 発射間隔の残り時間
	float deleyShot_;       
	// 連射間隔（固定秒数）
	float TIME_DELAY_SHOT;  
	// 弾リスト
	std::vector<std::unique_ptr<Shot>> shots_;

	//// 衝突判定に用いられるコライダ
	//std::vector<std::weak_ptr<Collider>> colliders_;  //CharacterBaseにあるので使わない
	// 
	//std::unique_ptr<Capsule> capsule_;　　　　　　　　//CharacterBaseにある

	// 衝突チェック
	/*VECTOR gravHitPosDown_;		//CharacterBaseにある
	VECTOR gravHitPosUp_;*/			//CharacterBaseにある

	// 丸影
	int imgShadow_;

	void InitAnimation(void);

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStatePlay(void);
	void ChangeStateDead();

	// 更新ステップ
	void UpdateNone(void);
	void UpdatePlay(void);
	
	// 描画系
	void DrawShadow(void);

	// 操作
	void ProcessMove(void);
	void ProcessJump(void);

	// 回転
	void SetGoalRotate(double rotRad);
	void Rotate(void);

	//// 衝突判定
	//void Collision(void);          |
	//void CollisionGravity(void);   |
	//void CollisionCapsule(void);   |
	//                               |   //CharacterBaseにある
	//// 移動量の計算                | 
	//void CalcGravityPow(void);     |

	//地面に接地した場合のジャンプ処理
	void OnLanding(const MV1_COLL_RESULT_POLY& hit);

	// 着地モーション終了
	bool IsEndLanding(void);

	//攻撃中か判別
	bool isAttacking_;
	float attackTimer_ = 0.0f;
	const float attackDuration_ = 0.6f;
};

