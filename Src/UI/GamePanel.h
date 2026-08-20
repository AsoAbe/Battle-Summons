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


// プレイヤーHPとスコアを描画

class GamePanel
{
public:
	// 無効なハンドル
	static constexpr int INVALID_HANDLE = -1;

	// 初期値
	static constexpr int INITIAL_COUNT = 0;

	// HP割合の下限
	static constexpr float HP_RATE_MIN = 0.0f;

	// HP割合の上限
	static constexpr float HP_RATE_MAX = 1.0f;

	// HP色補間の倍率
	static constexpr float HP_RATE_MULTIPLIER = 2.0f;

	// 色の初期値
	static constexpr int COLOR_INITIAL = 0;

	// 白色
	static constexpr unsigned int COLOR_WHITE = 0xFFFFFF;

	// 敵HPバーのX座標
	static constexpr int ENEMY_BAR_POS_X = 0;

	static constexpr int FRAME_WIDTH_BAR = DrawUtility::FRAME_WIDTH_BAR;

	// 一時スクリーンのサイズ	
	static constexpr int BAR_SCREEN_SIZE = 256;

	// hpバー演出の時間(60FPSのフレーム数)	
	static constexpr int BAR_FLASH_TIME = 30;

	// hpバー演出の時間(60FPSのフレーム数)	
	static constexpr int BAR_ANIM_TIME = 12;

	// デフォルトの長さ	
	static constexpr int BAR_SIZE = 100;

	// デフォルトの幅	
	static constexpr int BAR_WIDTH = 16;

	// HPバー揺れ演出用乱数範囲	
	static constexpr int HP_ANIM_RAND = 3;

	// プレイヤーHPバーY座標	
	static constexpr int PLAYER_BAR_Y = 320;

	// プレイヤー名表示オフセット	
	static constexpr int PLAYER_NAME_OFFSET_Y = 3;

	// 敵HPバーY座標	
	static constexpr int ENEMY_BAR_POS_Y = 30;

	// 敵HPバー高さ	
	static constexpr int ENEMY_BAR_HEIGHT = 30;

	// 敵HPバー横幅補正	
	static constexpr int ENEMY_BAR_WIDTH_OFFSET = 280;

	// ボス名表示座標	
	static constexpr int ENEMY_NAME_X = 30;
	static constexpr int ENEMY_NAME_Y = 10;

	// HP色計算用	
	static constexpr int HP_COLOR_MAX = 255;
	static constexpr int HP_COLOR_RED_LOW = 70;
	static constexpr float HP_RATE_BORDER = 0.5f;
	static constexpr float HP_COLOR_SCALE = 225.0f;

	// HPバーの大きさ
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
	std::shared_ptr<Player> player_;

	//敵
	std::shared_ptr<EnemyGolem> enemy_;

	//フレーム数
	int counter_;

	// 背景
	int bgImg_;

	int barScreen_;

	int color_;
	
	// デストラクタによって呼ばれる
	void Release();
};