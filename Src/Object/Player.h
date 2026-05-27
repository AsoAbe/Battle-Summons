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
	//初期座標
	static constexpr float START_POS_Y = 30.0f;

	//モデル回転
	static constexpr float MODEL_ROT_Y = 180.0f;

	//カプセル関連
	static constexpr float CAPSULE_OFFSET_Y = 10.0f;
	static constexpr float CAPSULE_TOP_Y = 140.0f;
	static constexpr float CAPSULE_BOTTOM_Y = 30.0f;
	static constexpr float CAPSULE_RADIUS = 30.0f;

	//スピード
	static constexpr float SPEED_MOVE = 5.0f;
	static constexpr float SPEED_RUN = 10.0f;

	//回転完了までの時間
	static constexpr float TIME_ROT = 1.0f;

	//最大体力
	static constexpr float MAX_HP = 100.0f;

	//ダメージ受けた時の無敵時間
	static constexpr float INVINCIBLE_TIME = 0.2f;

	//色の値
	static constexpr int COLOR_MAX = 255; //色の最大値
	static constexpr int COLOR_MIN = 0;   //色の最小値
	
	//回転補間速度
	static constexpr float ROTATE_LERP_SPEED = 0.1f;
	
	//描画ポリゴン数
	static constexpr int SHADOW_POLYGON_NUM = 1; 

	//攻撃関連---------------------------
	static constexpr float ATTACK_DURATION = 0.6f;       //攻撃継続時間
	
	//モデル表示位置補正-------------------------------------
	static constexpr float MODEL_OFFSET_Y = 45.0f;
	static constexpr float ATTACK_MODEL_OFFSET_Y = 10.0f; //攻撃時モデル補正
	static constexpr float MODEL_OFFSET_DEAD_Y = 0.0f;    //死亡時

	//移動方向角度---------------------------
	static constexpr double MOVE_ROT_FRONT = 0.0;    //前
	static constexpr double MOVE_ROT_RIGHT = 90.0;   //右
	static constexpr double MOVE_ROT_BACK = 180.0;   //後
	static constexpr double MOVE_ROT_LEFT = 270.0;   //左

	//ジャンプ---------------------------
	static constexpr float JUMP_ANIM_START = 13.0f;     //開始フレーム
	static constexpr float JUMP_ANIM_END = 25.0f;       //上昇終了フレーム
	static constexpr float JUMP_LOOP_START = 23.0f;     //ループ開始
	static constexpr float JUMP_LOOP_END = 25.0f;       //ループ終了
	static constexpr float LANDING_ANIM_START = 29.0f;  //着地開始
	static constexpr float LANDING_ANIM_END = 45.0f;    //着地終了
	static constexpr float LANDING_BLEND_TIME = 5.0f;   //着地補間時間
	static constexpr float POW_JUMP = 10.0f;		    //ジャンプ力
	static constexpr float TIME_JUMP_IN = 0.5f;		    //ジャンプ受付時間
	static constexpr float LANDING_OFFSET_Y = 2.0f;		//着地時のめり込み防止オフセット

	//回復---------------------------
	static constexpr float HEAL_EFFECT_OFFSET_Y = 130.0f; //エフェクト高さ
	static constexpr float REGEN_INTERVAL = 1.0f;		  //インターバル
	static constexpr int REGEN_AMOUNT = 5;                //回復量
	static constexpr float HEAL_EFFECT_SCALE = 10.0f;	  //回復エフェクトサイズ

	//影描画----------------------------------------
	static constexpr float SHADOW_HEIGHT = 300.0f;		// 影を落とす高さ
	static constexpr float SHADOW_SIZE = 30.0f;			// 影サイズ
	static constexpr float SHADOW_SLIDE_LENGTH = 0.5f;  // ポリゴンから浮かせる量(Z-fighting防止)
	static constexpr float SHADOW_ALPHA_MAX = 128.0f;   // 影の最大透明度
	static constexpr float SHADOW_DIAMETER_SCALE = 2.0f;//半径→直径変換
	static constexpr float SHADOW_UV_CENTER = 0.5f;		// UV中心補正
	static constexpr int SHADOW_VERTEX_NUM = 3; //影ポリゴン頂点数
	
	//アニメ共通---------------------------
	static constexpr float ANIM_START_FRAME = 0.0f; //開始フレーム
	static constexpr float ANIM_END_FRAME_AUTO = -1.0f; //最後まで再生

	//アニメーション再生速度------------------------------
	static constexpr float ANIM_SPEED_NORMAL = 20.0f;//通常
	static constexpr float ANIM_SPEED_ACTION = 60.0f;//ジャンプ・攻撃系
	static constexpr float ANIM_SPEED_FALL = 80.0f;  //落下
	
	//アニメループ設定---------------------------
	static constexpr bool ANIM_LOOP = true;
	static constexpr bool ANIM_NO_LOOP = false;

	//ショット関連---------------------------
	static constexpr float SHOT_SPAWN_OFFSET_Y = 150.0f;		//弾生成位置高さ
	static constexpr int SILVER_SHOT_NUM = 5;					//Silver弾数
	static constexpr int GOLD_SHOT_NUM_H = 5;					//Gold横弾数
	static constexpr int GOLD_SHOT_NUM_V = 3;					//Gold縦弾数
	static constexpr float SILVER_SHOT_ANGLE = DX_PI_F / 18.0f; //Silver拡散角
	static constexpr float GOLD_SHOT_ANGLE_H = DX_PI_F / 18.0f; //Gold横拡散角
	static constexpr float GOLD_SHOT_ANGLE_V = DX_PI_F / 36.0f; //Gold縦拡散角
	static constexpr float SHOT_CENTER_OFFSET = 2.0f;			//中央基準補正

	//キー入力---------------------------
	static constexpr int KEY_MOVE_FRONT = KEY_INPUT_W; //前
	static constexpr int KEY_MOVE_LEFT = KEY_INPUT_A;  //左
	static constexpr int KEY_MOVE_BACK = KEY_INPUT_S;  //後
	static constexpr int KEY_MOVE_RIGHT = KEY_INPUT_D; //右
	static constexpr int KEY_JUMP = KEY_INPUT_SPACE;   //ジャンプ
	static constexpr int KEY_HEAL = KEY_INPUT_Z;       //回復確認
	static constexpr int KEY_DAMAGE = KEY_INPUT_C;     //ダメージ確認
	
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

	float regenTimer_;
	float regenInterval_;	// 1秒ごと
	int   regenAmount_;     // 1回の回復量

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

	// 丸影
	int imgShadow_;
	//影アルファ値取得
	int GetShadowAlpha(float posY) const;

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

	//地面に接地した場合のジャンプ処理
	void OnLanding(const MV1_COLL_RESULT_POLY& hit);

	// 着地モーション終了
	bool IsEndLanding(void);

	//攻撃中か判別
	bool isAttacking_;
	float attackTimer_;

};

