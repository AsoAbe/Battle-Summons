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

GamePanel::GamePanel(GameScene& scene,
	std::shared_ptr<Player> player,
	std::shared_ptr<EnemyGolem> enemy)
	: scene_(scene),
	player_(player),
	enemy_(enemy)
{
	trans_ = nullptr;
	color_ = COLOR_INITIAL;

	counter_ = 0;
	bgImg_ = INVALID_HANDLE;
	barScreen_ = INVALID_HANDLE;
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
	DrawString(PANEL_R_POS_X, barY - DrawUtility::DEFAULT_TEXT_SIZE * PLAYER_NAME_OFFSET_Y, "YOU", COLOR_WHITE);

	float hpRate = static_cast<float>(player_->GetHp()) / player_->GetMaxHp();
	if (hpRate < HP_RATE_MIN) hpRate = HP_RATE_MIN;
	if (hpRate > HP_RATE_MAX) hpRate = HP_RATE_MAX;

	//プレイヤー体力----------------

	// 線形式を使って、色を滑らかに補間
	int r = COLOR_INITIAL; // 赤は常に強め
	int g = COLOR_INITIAL; // 緑は緩やかに減少
	int b = COLOR_INITIAL;
	// 0.0～1.0 の hpRate に対して自然に色が変化する
	if (hpRate > HP_RATE_BORDER)
	{
		// 0.5～1.0 → 緑(0,255,0)から黄(255,255,0)
		float t = ((hpRate - HP_RATE_BORDER) * HP_RATE_MULTIPLIER) * HP_COLOR_MAX; // 0～1
		r = static_cast<int>(HP_COLOR_MAX - t);
		g = HP_COLOR_MAX;
	}
	else
	{
		// 0.0～0.5 → 黄(255,255,0)から赤(255,0,0)
		float t = (HP_RATE_MAX - hpRate) * HP_COLOR_MAX; // 0～1
		r = HP_COLOR_MAX;
		g = static_cast<int>(HP_COLOR_MAX - t);
	}
	//体力(HP)
	DrawUtility::DrawBarGlossy({ PANEL_R_POS_X ,barY },
		PANEL_R_POS_X + BAR_SIZE, BAR_WIDTH + 5,
		{ r, g, b }, player_->GetHp(), player_->GetMaxHp());
	
	DrawFormatString(PANEL_R_POS_X
		, barY - DrawUtility::DEFAULT_TEXT_SIZE
		, COLOR_WHITE
		, "HP&MP:%.1f"
		, player_->GetHp()
	);
	//---------------------------------------


	// 敵の体力バー  --------------------------
	int barWidth = Application::MAINGAME_SIZE_X + ENEMY_BAR_WIDTH_OFFSET;   // 画面いっぱい
	int barHeight = ENEMY_BAR_HEIGHT;   // かなり大きめ
	int x = ENEMY_BAR_POS_X;
	int y = ENEMY_BAR_POS_Y;            // 画面上部ギリギリ

	// HP割合
	float hpErate = (float)enemy_->GetHp() / enemy_->GetMaxHp();
	if (hpRate < HP_RATE_MIN) hpRate = HP_RATE_MIN;
	if (hpRate > HP_RATE_MAX) hpRate = HP_RATE_MAX;

	//  赤いHPバー
	Vector2 startPos = { x, y };
	IntRGB redColor = {HP_COLOR_MAX,HP_COLOR_RED_LOW,HP_COLOR_RED_LOW};

	// DrawBarGlossy(start位置, バーの右端X, バーの高さ, 色, 現在値, 最大値)
	DrawUtility::DrawBarGlossy(startPos, x + barWidth, barHeight, redColor, enemy_->GetHp(), enemy_->GetMaxHp());
	
	DrawFormatString(ENEMY_NAME_X
		, ENEMY_NAME_Y
		, COLOR_WHITE
		, "BOSS ゴーレム"
	);
	
}

void GamePanel::Release()
{
}