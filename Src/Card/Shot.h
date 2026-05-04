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

	bool hasDealtDamage_ = false;

	//ダメージ処理
	void CheckDamage();
};

