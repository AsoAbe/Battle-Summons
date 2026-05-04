#pragma once
#include"../Common/DrawUtility.h"
#include"PanelBase.h"

class GameScene;
class Player;
class EnemyGolem;
class CharacterBase;
class Trans;

class PixelShaderMaterial;
class PixelShaderRenderer;

/// <summary>
/// プレイヤーHPとスコアを描画
/// </summary>
class GamePanel
{
public:
	static constexpr int LIFE_IMG_SIZE = 16;
	static constexpr int LIFE_IMG_HALF = LIFE_IMG_SIZE / 2;

	GamePanel(GameScene& scene,
		std::shared_ptr<Player> player,
		std::shared_ptr<EnemyGolem> enemy);
	~GamePanel();

	void Init();
	void Update();
	void Draw();

private:

	GameScene& scene_;

	Trans* trans_;

	// プレイヤー
	//Player& player_;
	std::shared_ptr<Player> player_;

	//敵
	//EnemyGolem& enemy_;
	std::shared_ptr<EnemyGolem> enemy_;

	//フレーム数
	int counter_;

	/// <summary>
	/// 背景
	/// </summary>
	int bgImg_;

	int barScreen_;

	int color_;
	/// <summary>
	/// デストラクタによって呼ばれる
	/// </summary>
	void Release();
};