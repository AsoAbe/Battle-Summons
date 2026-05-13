#pragma once
#include <memory>
#include "CardBase.h"
#include "../Object/Player.h"
#include "../Object/EnemyGolem.h"
#include "ShotBase.h"
class EnemyGolem;

class Shot :
	public ShotBase
{
public:
	//弾の基本速度
	static constexpr float DEFAULT_SPEED = 13.0f;

	//弾の生存時間
	static constexpr float DEFAULT_ALIVE_TIME = 3.0f;

	//弾の基本サイズ
	static constexpr float DEFAULT_SCALE = 0.2f;

	//基本ダメージ
	static constexpr int DEFAULT_DAMAGE = 15;

	//プレイヤー反動ダメージ倍率
	static constexpr float PLAYER_DAMAGE_RATE = 0.5f;

	//レア度倍率(CardBaseのものとは数値が違う)
	static constexpr float BRONZE_RATE = 1.0f;
	static constexpr float SILVER_RATE = 2.0f;
	static constexpr float GOLD_RATE = 3.0f;

	// コンストラクタ
	Shot(std::shared_ptr<Player> player, std::shared_ptr<EnemyGolem> enemy);

	// デストラクタ
	~Shot(void);

	void Update(void) override;

	//共通機能
	void SetParam() override;
	//効果の発動
	void ProcessCard() override;

	void SetEnemy(std::shared_ptr<EnemyGolem> enemy);
private:
	// 弾を発射するプレイヤー
	std::shared_ptr<Player> player_;

	std::shared_ptr<EnemyGolem> enemy_;

	//ダメージ処理
	void CheckDamage();
};

