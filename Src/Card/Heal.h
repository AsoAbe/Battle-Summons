#pragma once
#include "CardBase.h"
class Player;

class Heal :
    public CardBase
{
public:
	Heal(std::shared_ptr<Player> player); // ← Playerを受け取る
	virtual ~Heal();

	//初期化
	bool Init(void) override;
	//更新
	void Update(void) override;
	//描画
	void Draw(void) override;
	//解放
	bool Release(void) override;

	//共通機能
	void SetParam() override;

	//効果の発動
	void ProcessCard() override;
private:
	//回復対象プレイヤー
	std::shared_ptr<Player> player_;

	bool used_;
};

