#pragma once
#include <vector>
#include <memory>
#include "CannonBase.h"

class Player;
class EnemyGolem;

class Cannon :
	public CannonBase
{
public:

	//プレイヤーから前に出す距離
	static constexpr float OFFSET_DIST = 120.0f;

	//砲身の高さ
	static constexpr float BARREL_HEIGHT = 100.0f;

	//弾の発射位置（ローカルY）
	static constexpr float SHOT_OFFSET_Y = 25.0f;

	//弾の発射位置（ローカルZ）
	static constexpr float SHOT_OFFSET_Z = 30.0f;

	// 砲身のX回転制限(最大)
	static constexpr float MAX_ANGLE_X
		= 15.0f * DX_PI_F / 180.0f;

	// 砲身のX回転制限(最小)
	static constexpr float MIN_ANGLE_X
		= -30.0f * DX_PI_F / 180.0f;

	// 弾発射後の硬直時間
	static constexpr float SHOT_DELAY = 1.0f;

	// 爆発のサイズ
	static constexpr int BLAST_SIZE_X = 32;
	static constexpr int BLAST_SIZE_Y = 32;
	// 爆発のアニメーション数
	static constexpr int BLAST_ANIM_NUM = 16;

	// コンストラクタ
	Cannon(std::shared_ptr<Player> player, std::weak_ptr<EnemyGolem> enemy);
	// デストラクタ
	~Cannon(void);

	bool Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool Release(void) override;

	//効果の発動
	void ProcessCard() override;
private:
#pragma region 砲台
	//砲台のモデルID
	int standModelId_;

	//砲台の大きさ
	VECTOR standScl_;
	//砲台の角度
	VECTOR standRot_;
	//砲台の座標
	VECTOR standPos_;
#pragma endregion

#pragma region 砲身
	//砲身のモデルID
	int barrelModelId_;

	//砲身の大きさ
	VECTOR barrelScl_;
	//砲身の角度
	VECTOR barrelRot_;
	//砲身の座標
	VECTOR barrelPos_;

	// 砲台からの相対座標
	VECTOR barrelLocalPos_;
#pragma endregion

	// ショット(ポインタ)
	std::vector<std::unique_ptr<CannonBase>> shots_;

	//プレイヤー
	std::shared_ptr<Player> player_;
	std::weak_ptr<EnemyGolem> enemy_;

	// 弾のモデルID
	int shotModelId_;

	// 弾発射後の硬直時間計算用
	float stepShotDelay_;

	//回転操作
	void ProcessRot(void);

	// 発射操作
	void ProcessShot(void);

	//弾更新
	//void UpdateShots();  //SceneManagerにある
	//生存時間
	void UpdateLifeTime();
	//モデル更新
	void UpdateTransform();

	// 有効な弾を取得する
	CannonBase* GetValidShot(void);

	// 爆発の画像(本来は外部リソース用の管理クラスを作るべき。弾モデルも。)
	//int blastImgs_[BLAST_ANIM_NUM];
	int blastEffect_ = -1;

	// 生存時間（秒）
	static constexpr float timeAlive_ = 2.0f; // 例：10秒
	float stepAlive_ = 0.0f;
};
