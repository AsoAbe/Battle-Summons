#pragma once
#include "CardBase.h"
class Player;
class EnemyGolem;
class Collider;
class Capsure;

class CannonBase :
	public CardBase
{
	public:

	//衝突判定用の球体半径
	static constexpr float COL_RADIUS = 10.0f;
	
	static constexpr float DEFAULT_FPS = 60.0f;
	// 重力
	static constexpr float GRAVITY = 9.81f;
	// 弾の状態
	enum class STATE
	{
		NONE,
		SHOT,
		BLAST,
		END
	};

	// ダメージヒット情報
	struct DamageHit {
		void* target; // Player* か EnemyGolem*
		int damage;
	};


	// コンストラクタ(元となるモデルのハンドルID)
	CannonBase(int baseModelId, int blastEffect);
	// デストラクタ
	virtual ~CannonBase(void);
	// 弾の生成(表示開始座標、弾の進行方向)
	void CreateShot(VECTOR pos, VECTOR dir);
	// 更新ステップ
	void Update(void) override;
	void UpdateShot(void);
	void UpdateBlast(void);
	void UpdateEnd(void);
	void UpdateCapsulePos();
	// 描画
	void Draw(void) override;
	void DrawShot();
	void DrawBlast();
	void DrawEnd();
	// 解放処理
	bool Release(void) override;
	// 生存判定
	bool IsAlive(void);

	// 弾判定
	bool IsShot(void);

	VECTOR GetPos(void);
	float GetRadius(void) const;
	void SetColliders(const std::vector<std::weak_ptr<Collider>>& cols);
	void SetPlayer(std::shared_ptr<Player> player);
	void SetEnemy(std::weak_ptr<EnemyGolem> enemy);

	//弾の爆発
	void Blast(void);
	void CheckBlastDamage(void);
	void CreateExplosion(const VECTOR& pos);

	//渡す用
	bool IsExploding() const;
	VECTOR GetExplosionPos() const;
	float GetExplosionRadius() const;
	int GetExplosionDamage() const;
	bool HasDealtDamage() const;
	void MarkDamageDealt();
	
	//強制ストップ
	void StopEffect();
protected:

	// 衝突判定に用いられるコライダ
	std::vector<std::weak_ptr<Collider>> colliders_;
	std::unique_ptr<Capsule> capsule_;
	
	// 弾の状態
	STATE state_;

	// 元となる弾のモデルID
	int baseModelId_;
	// 弾のモデルID
	int modelId_;

	// 方向
	VECTOR dir_;
	// 弾の大きさ
	VECTOR scl_;
	// 弾の角度
	VECTOR rot_;
	// 弾の座標
	VECTOR pos_;
	// 弾の移動速度
	float speed_;
	//当たり判定半径
	float radius_;
	//バレル、スタンド大きさ
	float size_;
	// 重力
	float gravityPow_;

	//爆発関係
	int blastEffectHandle_; 
	int blastEffectPlayId_;
	float blastTimer_ = 0.0f;      // 爆発経過時間
	const float blastDuration_;
	float blastRadius_;             // 爆風の有効範囲
	int Damage_;
	bool hasDealtDamage_;           // ダメージを一度しか与えない制御

	float DamageRate;

	void PlayBlastEffect();

	// 状態遷移
	void ChangeState(STATE state);

	void Colliders(void);
	void CheckHitSphere();

	bool Fired_;
	
	// 追加：前フレーム位置（高速弾のすり抜け対策で使用）
	VECTOR prevPos_;
	float bulletRadius_;    // 弾の半径
};