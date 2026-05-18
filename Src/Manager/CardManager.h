#pragma once
#include <string>
#include <vector>
#include "../Object/Common/Transform.h"
#include "../Object/Player.h"

class CardBase;
class Cannon;
class Shot;

class CardManager
{
public:
	// UI座標
	static constexpr int CARD_BASE_X = 10;
	static constexpr int CARD_BASE_Y = 550;
	static constexpr int CARD_INTERVAL = 150;

	static constexpr int CARD_STRING_X = 178;
	static constexpr int CARD_STRING_Y = 510;


	// キー表示位置
	static constexpr int KEY_TEXT_Y = 510;

	// カード描画倍率
	static constexpr float CARD_DRAW_SCALE = 0.12f;

	// アイコン描画倍率
	static constexpr float ICON_DRAW_SCALE = 0.10f;
	//モデルのファイル名
	std::string modelFileName_;

	enum class CARD
	{
		NONE,
		BRONZE, //銅レア
		SILVER, //銀レア
		GOLD,   //金レア
	};

	enum class EFFECT
	{
		NONE,
		SHOT,
		CANNON,
		MONSTER
	};

	static CardManager& GetInstance()
	{
		static CardManager instance; // ← 自動で破棄される（delete不要）
		return instance;
	}

	//コンストラクタ
	CardManager(void);
	//デストラクタ
	~CardManager(void);

	//初期化
	bool virtual Init(void);
	//更新
	void virtual Update(void);
	//描画
	void virtual Draw(void);
	//解放
	bool  virtual Release(void);

	void SetPlayer(Player* player);
	void SetEnemy(std::shared_ptr<EnemyGolem> enemy);

	void virtual SetRarity(CARD rarity);

	//呼び出し用
	void UseCard(EFFECT effectId);

protected:
	
	// 静的インスタンス
	static CardManager* instance_;
	// 単発カード（Healなど）
	std::shared_ptr<CardBase> base_;

	// 射撃（弾を管理）
	std::shared_ptr<Shot> shot_;

	// 砲撃
	std::shared_ptr<Cannon> cannon_;

	//プレイヤー
	std::shared_ptr<Player> player_;
	
	std::shared_ptr<EnemyGolem> enemy_;

	int cardImgBr_;
	int cardImgSl_;
	int cardImgGo_;

	// モデル制御の基本情報
	Transform transform_;

	// 進行方向
	VECTOR dir_;

	//カードの共通効果
	int Damage_;
	float speed_;

	bool cardUsedThisFrame_;

	//レア度
	CARD rarity_ = CARD::BRONZE;

	//起動用
	void DoCard(EFFECT effectId);

	std::vector<std::weak_ptr<Collider>> GetCurrentStageColliders();

	// 選択中の効果
	EFFECT currentEffect_ = EFFECT::SHOT;

	// 効果アイコン
	int iconShot_ = -1;
	int iconCannon_ = -1;
	int iconMonster_ = -1;

	struct CardUI {
		EFFECT effect;
		int iconImg;
		int x;
		int y;
	};

	std::vector<CardUI> cardList_;
	void LoadIcons(); 
	void DrawCardIcon();
};

