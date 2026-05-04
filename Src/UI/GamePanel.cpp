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

	/// <summary>
	/// 一時スクリーンのサイズ
	/// </summary>
	constexpr int BAR_SCREEN_SIZE = 256;

	/// <summary>
	/// hpバー演出の時間(60FPSのフレーム数)
	/// </summary>
	constexpr int BAR_FLASH_TIME = 30;
	/// <summary>
	/// hpバー演出の時間(60FPSのフレーム数)
	/// </summary>
	constexpr int BAR_ANIM_TIME = 12;
	/// <summary>
	/// デフォルトの長さ
	/// </summary>
	static constexpr int BAR_SIZE = 100;
	/// <summary>
	/// デフォルトの幅
	/// </summary>
	static constexpr int BAR_WIDTH = 16;

	constexpr int HP_ANIM_RAND = 3;
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
	int ENbarY = 88;  //エネミー用スコアバーの高さ
	int barY = 320;    //プレイヤー用スコアバーの高さ

	constexpr int PANEL_R_POS_X = Application::MAINGAME_POS_X;
#pragma region PlayerHp

	//テキスト描画
	DrawString(PANEL_R_POS_X, barY - DrawUtility::DEFAULT_TEXT_SIZE*3, "YOU", 0xffffff);

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
		float t = ((hpRate - 0.5f) * 2.0f) * 225; // 0～1
		r = static_cast<int>(255 - t);
		g = 255;
	}
	else
	{
		// 0.0～0.5 → 黄(255,255,0)から赤(255,0,0)
		float t = (1.0f- hpRate) * 225; // 0～1
		r = 255;
		g = static_cast<int>(255 - t);
	}
	//体力(HP)
	DrawUtility::DrawBarGlossy({ PANEL_R_POS_X ,barY },
		PANEL_R_POS_X + BAR_SIZE, BAR_WIDTH + 5,
		{ r, g, b }, player_->GetHp(), player_->GetMaxHp());
	/*barY += BAR_WIDTH + 4 + LIFE_IMG_HALF;*/
	
	DrawFormatString(PANEL_R_POS_X
		, barY - DrawUtility::DEFAULT_TEXT_SIZE
		, 0xFFFFFF
		, "HP&MP:%.1f"
		, player_->GetHp()
	);
	//---------------------------------------


	// 敵の体力バー  --------------------------
	int barWidth = Application::MAINGAME_SIZE_X + 150;   // 画面いっぱい
	int barHeight = 40;                             // かなり大きめ
	int x = 0;
	int y = 30;                                     // 画面上部ギリギリ

	// HP割合
	float hpErate = (float)enemy_->GetHp() / enemy_->GetMaxHp();
	if (hpErate < 0.0f) hpErate = 0.0f;
	if (hpErate > 1.0f) hpErate = 1.0f;

	//  赤いHPバー
	Vector2 startPos = { x, y };
	IntRGB redColor = { 255, 70, 70 };

	// DrawBarGlossy(start位置, バーの右端X, バーの高さ, 色, 現在値, 最大値)
	DrawUtility::DrawBarGlossy(startPos, x + barWidth, barHeight, redColor, enemy_->GetHp(), enemy_->GetMaxHp());
	
	DrawFormatString(30
		, 10
		, 0xFFFFFF
		, "BOSS ゴーレム"
	);
	//-----------------------------------


	//DrawFormatString(PANEL_R_POS_X
	//	, barY - DrawUtility::DEFAULT_TEXT_SIZE * 2
	//	, 0xFFFFFF
	//	, "色%d"
	//	, r
	//);
}

void GamePanel::Release()
{
}