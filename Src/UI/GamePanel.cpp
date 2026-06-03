#include "../Application.h"
#include "../Common/DrawUtility.h"
#include "../Scene/GameScene.h"
#include"../Common/Vector2.h"
#include"../Common/Trans.h"
#include "../Object/Player.h"
#include "../Object/EnemyGolem.h"
#include "../Manager/SceneManager.h"
#include "GamePanel.h"

using IntRGB = DrawUtility::IntRGB;

namespace
{
	constexpr int FRAME_WIDTH_BAR = DrawUtility::FRAME_WIDTH_BAR;

	// 一時スクリーンのサイズ	
	constexpr int BAR_SCREEN_SIZE = 256;
	
	// hpバー演出の時間(60FPSのフレーム数)	
	constexpr int BAR_FLASH_TIME = 30;

	// hpバー演出の時間(60FPSのフレーム数)	
	constexpr int BAR_ANIM_TIME = 12;

	// デフォルトの長さ	
	static constexpr int BAR_SIZE = 100;

	// デフォルトの幅	
	static constexpr int BAR_WIDTH = 16;
	
	// HPバー揺れ演出用乱数範囲	
	constexpr int HP_ANIM_RAND = 3;
	
	// プレイヤーHPバーY座標	
	constexpr int PLAYER_BAR_Y = 320;
	
	// プレイヤー名表示オフセット	
	constexpr int PLAYER_NAME_OFFSET_Y = 3;

	// 敵HPバーY座標	
	constexpr int ENEMY_BAR_POS_Y = 30;
	
	// 敵HPバー高さ	
	constexpr int ENEMY_BAR_HEIGHT = 30;

	// 敵HPバー横幅補正	
	constexpr int ENEMY_BAR_WIDTH_OFFSET = 280;
	
	// ボス名表示座標	
	constexpr int ENEMY_NAME_X = 30;
	constexpr int ENEMY_NAME_Y = 10;
	
	// HP色計算用	
	constexpr int HP_COLOR_MAX = 255;
	constexpr int HP_COLOR_RED_LOW = 70;
	constexpr float HP_RATE_BORDER = 0.5f;
	constexpr float HP_COLOR_SCALE = 225.0f;
}

GamePanel::GamePanel(GameScene& scene,
	std::shared_ptr<Player> player,
	std::shared_ptr<EnemyGolem> enemy)
	: scene_(scene),
	player_(player),
	enemy_(enemy)
{
	trans_ = nullptr;
	color_ = 0;

	counter_ = 0;
	bgImg_ = -1;
	barScreen_ = -1;
}

GamePanel::~GamePanel()
{
	//解放処理
	//Release();
}

void GamePanel::Init()
{
}

void GamePanel::Update()
{
	counter_++;
}

void GamePanel::Draw()
{
	//メインスクリーン
	int mainScreen = SceneManager::GetInstance().GetMainScreen();

	//バーのy始点
	int barY = PLAYER_BAR_Y;    //プレイヤー用スコアバーの高さ

	constexpr int PANEL_R_POS_X = Application::MAINGAME_POS_X;
#pragma region PlayerHp

	//テキスト描画
	DrawString(PANEL_R_POS_X, barY - DrawUtility::DEFAULT_TEXT_SIZE * PLAYER_NAME_OFFSET_Y, "YOU", 0xffffff);

	float hpRate = static_cast<float>(player_->GetHp()) / player_->GetMaxHp();
	if (hpRate < 0.0f) hpRate = 0.0f;
	if (hpRate > 1.0f) hpRate = 1.0f;

	//プレイヤー体力----------------

	// 線形式を使って、色を滑らかに補間
	int r = 0; // 赤は常に強め
	int g = 0; // 緑は緩やかに減少
	int b = 0;
	// 0.0～1.0 の hpRate に対して自然に色が変化する
	if (hpRate > 0.5f)
	{
		// 0.5～1.0 → 緑(0,255,0)から黄(255,255,0)
		float t = ((hpRate - 0.5f) * 2.0f) * HP_COLOR_MAX; // 0～1
		r = static_cast<int>(HP_COLOR_MAX - t);
		g = HP_COLOR_MAX;
	}
	else
	{
		// 0.0～0.5 → 黄(255,255,0)から赤(255,0,0)
		float t = (1.0f- hpRate) * HP_COLOR_MAX; // 0～1
		r = HP_COLOR_MAX;
		g = static_cast<int>(HP_COLOR_MAX - t);
	}
	//体力(HP)
	DrawUtility::DrawBarGlossy({ PANEL_R_POS_X ,barY },
		PANEL_R_POS_X + BAR_SIZE, BAR_WIDTH + 5,
		{ r, g, b }, player_->GetHp(), player_->GetMaxHp());
	
	DrawFormatString(PANEL_R_POS_X
		, barY - DrawUtility::DEFAULT_TEXT_SIZE
		, 0xFFFFFF
		, "HP&MP:%.1f"
		, player_->GetHp()
	);
	//---------------------------------------


	// 敵の体力バー  --------------------------
	int barWidth = Application::MAINGAME_SIZE_X + ENEMY_BAR_WIDTH_OFFSET;   // 画面いっぱい
	int barHeight = ENEMY_BAR_HEIGHT;   // かなり大きめ
	int x = 0;
	int y = ENEMY_BAR_POS_Y;            // 画面上部ギリギリ

	// HP割合
	float hpErate = (float)enemy_->GetHp() / enemy_->GetMaxHp();
	if (hpErate < 0.0f) hpErate = 0.0f;
	if (hpErate > 1.0f) hpErate = 1.0f;

	//  赤いHPバー
	Vector2 startPos = { x, y };
	IntRGB redColor = {HP_COLOR_MAX,HP_COLOR_RED_LOW,HP_COLOR_RED_LOW};

	// DrawBarGlossy(start位置, バーの右端X, バーの高さ, 色, 現在値, 最大値)
	DrawUtility::DrawBarGlossy(startPos, x + barWidth, barHeight, redColor, enemy_->GetHp(), enemy_->GetMaxHp());
	
	DrawFormatString(ENEMY_NAME_X
		, ENEMY_NAME_Y
		, 0xFFFFFF
		, "BOSS ゴーレム"
	);
	
}

void GamePanel::Release()
{
}