#pragma once
#include <string>
#include "../Manager/CardManager.h"
class Player;
class EnemyGolem;

class CardBase
{
public:

	enum class CARD_RARITY
	{
		NONE,
		BRONZE,
		SILVER,
		GOLD
	};

	//コンストラクタ
	CardBase(void);
	//デストラクタ
	~CardBase(void);

	//初期化
	virtual bool Init(void);
	//更新
	virtual void Update(void);
	//描画
	virtual void Draw(void);
	//解放
	virtual bool Release(void);

	virtual void SetParam();

	// 操作：発動
	virtual void ProcessCard(void);
	// 自機のカードを発動
	virtual void CreateCard(void);

	bool IsAlive()const;

	// レア度を設定
	void SetRarity(CARD_RARITY rarity);

	// CardManagerとの整合性を取るための補助関数
	CARD_RARITY GetRarity() const { return rarity_; }

	void SetEnemy(std::weak_ptr<EnemyGolem> enemy);
protected:

	// プレイヤーとの当たり判定のために保持
	std::weak_ptr<Player> player_;
	std::weak_ptr<EnemyGolem> enemy_;

	// モデル制御の基本情報
	Transform transform_;
	// 進行方向
	VECTOR dir_;

	//レア度
	CARD_RARITY rarity_;

	//カードの共通効果
	int Damage_;
	int Heal_;
	float speed_;

	bool isAlive_;

	int iconImg_ = -1;
};