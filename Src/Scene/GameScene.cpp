#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include"../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/CardManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/SkyDome.h"
#include "../Object/Stage.h"
#include "../Object/Player.h"
#include "../Object/enemyGolem.h"
#include "../Object/Planet.h"
#include "../UI/GamePanel.h"
#include "../Object/Metal.h"
#include "../Object/Water.h"
#include "../Renderer/PixelMaterial.h"
#include "../Renderer/PixelRenderer.h"
#include "GameScene.h"

using SOUND_ID = SoundManager::SOUND_ID;

GameScene::GameScene(void)
{
	player_ = nullptr;
	enemy_ = nullptr;
	skyDome_ = nullptr;
	stage_ = nullptr;
	panel_ = nullptr;
	metal_ = nullptr;
	water_ = nullptr;
	card_ = nullptr;
}

GameScene::~GameScene(void)
{
	DeleteGraph(postEffectScreen_);
}

void GameScene::Init(void)
{
	auto& sceneMng = SceneManager::GetInstance();

	// プレイヤー
	player_ = std::make_shared<Player>();
	player_->Init();

	// 敵ゴーレム
	//enemy_ = std::make_unique<EnemyGolem>(*this);
	enemy_ = std::make_shared<EnemyGolem>(*this);
	enemy_->Init();
	enemy_->SetPlayer(player_);

	player_->SetEnemy(enemy_);

	// ステージ
	stage_ = std::make_shared<Stage>(player_,enemy_);
	stage_->Init();
	SceneManager::GetInstance().SetStage(stage_);
	// ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	//カード
	//card_ = std::make_unique<CardManager>();
	card_ = &CardManager::GetInstance();
	card_->Init();
	card_->SetPlayer(player_.get());
	card_->SetEnemy(enemy_);


	//ステータス
	panel_ = std::make_unique<GamePanel>(*this,player_,enemy_);
	//panel_ = new GamePanel(*this,*player_);
	panel_->Init();

	// スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Init();

	//// 金属
	//metal_ = std::make_unique<Metal>();
	//metal_->Init();
	//	
	//// 水
	//water_ = std::make_unique<Water>();
	//water_->Init();

	mainCamera.SetFollow(&player_->GetTransform());
	mainCamera.ChangeMode(Camera::MODE::FOLLOW);

	// ポストエフェクト用スクリーン
	postEffectScreen_ = MakeScreen(
		Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true);

	// ポストエフェクト用(モノトーン)
	monoMaterial_ = std::make_unique<PixelMaterial>("Monotone.cso", 1);
	monoMaterial_->AddConstBuf({ 1.0f, 1.0f, 1.0f, 1.0f });
	monoMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	monoRenderer_ = std::make_unique<PixelRenderer>(*monoMaterial_);
	monoRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);

	// ポストエフェクト用(走査線)
	scanMaterial_ = std::make_unique<PixelMaterial>("ScanLine.cso", 2);
	scanMaterial_->AddConstBuf({ 1.0f, 1.0f, 1.0f, 1.0f });
	scanMaterial_->AddConstBuf({ 0.0f, 0.0f, 0.0f, 0.0f });
	scanMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	scanRenderer_ = std::make_unique<PixelRenderer>(*scanMaterial_);
	scanRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);

	// ポストエフェクト用(ビネット)
	vineMaterial_ = std::make_unique<PixelMaterial>("Vignette.cso", 1);
	vineMaterial_->AddConstBuf({ 3.5f, 0.0f, 0.0f, 0.0f });
	vineMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	vineRenderer_ = std::make_unique<PixelRenderer>(*vineMaterial_);
	vineRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);

	// ポストエフェクト用(レンズの歪み)
	lensMaterial_ = std::make_unique<PixelMaterial>("LensDistortion.cso", 1);
	lensMaterial_->AddConstBuf({ 3.5f, 0.0f, 0.0f, 0.0f });
	lensMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	lensRenderer_ = std::make_unique<PixelRenderer>(*lensMaterial_);
	lensRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);

	// 初期モード
	mode_ = MODE::MAIN;

	bgm_.Set(BGM::BgmType::GAME);

	SoundManager::GetInstance().ChangeBGM(
		static_cast<SoundManager::SOUND_ID>(BGM::ToSoundID(BGM::BgmType::GAME)),
		true,
		SoundManager::VOLUME_BGM
	);
}

void GameScene::Update(void)
{

	// シーン遷移
	InputManager& ins = InputManager::GetInstance();

	if (ins.IsTrgDown(KEY_INPUT_RSHIFT))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
	}

	// モード切替
	if (ins.IsTrgDown(KEY_INPUT_BACKSLASH))
	{
		int tmp = (int)mode_;
		tmp++;
		if (tmp >= (int)MODE::MAX)
		{
			tmp = 0;
		}
		mode_ = (MODE)tmp;
	}

	skyDome_->Update();

	stage_->Update();

	player_->Update();
	
	enemy_->Update();
	
	card_->Update();
	
	panel_->Update();

	//metal_->Update();

	//water_->Update();

}

void GameScene::Draw(void)
{
	bool isLeftDir;
	isLeftDir = false;
	double angle;
	angle = 0.0;
	// 背景
	skyDome_->Draw();
	stage_->Draw();
	
	player_->Draw();
	enemy_->Draw();

	//metal_->Draw();
	//water_->Draw();

	card_->Draw();

	stage_->DrawTranslucent();
	
	//ステータス(画面全体の明るさを含む)
	panel_->Draw();

	// ヘルプ
	/*DrawFormatString(740, 20, 0xFFFFFF, "シーン遷移　　：右SHIFT");
	DrawFormatString(740, 40, 0xFFFFFF, "大砲　：Zキー");
	DrawFormatString(740, 60, 0xFFFFFF, "回復：Xキー");
	DrawFormatString(740, 80, 0xFFFFFF, "射撃：Cキー");
	DrawFormatString(740, 100, 0xFFFFFF, "カードレア度変更：Vキー");
	DrawFormatString(740, 120, 0xFFFFFF, "ジャンプ：＼(スペース)");*/


	int mainScreen = SceneManager::GetInstance().GetMainScreen();

	// ポストエフェクト(モノクロ)
	//-----------------------------------------
	if (mode_ < MODE::MONO) { return; }
	
	SetDrawScreen(postEffectScreen_);

	// 画面を初期化
	ClearDrawScreen();

	monoRenderer_->Draw();

	// メインに戻す
	SetDrawScreen(mainScreen);
	DrawGraph(0, 0, postEffectScreen_, false);
	//-----------------------------------------

	// ポストエフェクト(走査線)
	//-----------------------------------------
	if (mode_ < MODE::SCAN) { return; }
	
	SetDrawScreen(postEffectScreen_);

	// 画面を初期化
	ClearDrawScreen();

	scanMaterial_->SetConstBuf(
		1,
		{ SceneManager::GetInstance().GetTotalTime(), 0.0f, 0.0f, 0.0f }
	);
	scanRenderer_->Draw();

	// メインに戻す
	SetDrawScreen(mainScreen);
	DrawGraph(0, 0, postEffectScreen_, false);
	//-----------------------------------------


	// ポストエフェクト(レンズの歪み)
	//-----------------------------------------
	if (mode_ < MODE::LENS) { return; }

	SetDrawScreen(postEffectScreen_);

	// 画面を初期化
	ClearDrawScreen();

	lensRenderer_->Draw();

	// メインに戻す
	SetDrawScreen(mainScreen);
	DrawGraph(0, 0, postEffectScreen_, false);
	//-----------------------------------------
	

	// ポストエフェクト(ビネット)
	//-----------------------------------------
	if (mode_ < MODE::VINE) { return; }

	SetDrawScreen(postEffectScreen_);

	// 画面を初期化
	ClearDrawScreen();

	vineRenderer_->Draw();

	// メインに戻す
	SetDrawScreen(mainScreen);
	DrawGraph(0, 0, postEffectScreen_, false);
	//-----------------------------------------
}

void GameScene::LoadSceneGameSound()
{
	SoundManager& soundManager = SoundManager::GetInstance();

	soundManager.LoadSound(SoundManager::SOUND_ID::NONE, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::CURSOR, SoundManager::VOLUME_MAX);
	soundManager.LoadSound(SoundManager::SOUND_ID::CAANON, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::BLAST_S, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::BLAST_M, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::BLAST_L, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::SHOT_S, SoundManager::VOLUME_LOW);
	soundManager.LoadSound(SoundManager::SOUND_ID::SHOT_M, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::SHOT_L, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::SHOT_BLAST, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::HIT, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::HIT_PLAYER, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::DEATH_PLAYER, SoundManager::VOLUME_HIGH);
	soundManager.LoadSound(SoundManager::SOUND_ID::BOSS, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::SUCCESS, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::JUMP, SoundManager::VOLUME_HIGH);
	soundManager.LoadSound(SoundManager::SOUND_ID::CHARGE, SoundManager::VOLUME_LOW);
	soundManager.LoadSound(SoundManager::SOUND_ID::WIN, SoundManager::VOLUME_HIGH);
	soundManager.LoadSound(SoundManager::SOUND_ID::ENEMY_DASH, SoundManager::VOLUME_STANDARD);
	soundManager.LoadSound(SoundManager::SOUND_ID::BGM_TITLE, SoundManager::VOLUME_STANDARD);
}


bool GameScene::Release(void)
{
	return true;
}

void GameScene::GameOver()
{
	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
}

void GameScene::GameClear()
{
	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
}
