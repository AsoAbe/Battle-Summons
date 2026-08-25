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
#include "../Renderer/PixelMaterial.h"
#include "GameScene.h"

using SOUND_ID = SoundManager::SOUND_ID;

GameScene::GameScene(void)
{
	player_ = nullptr;
	enemy_ = nullptr;
	skyDome_ = nullptr;
	stage_ = nullptr;
	panel_ = nullptr;
	card_ = nullptr;

}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	auto& sceneMng = SceneManager::GetInstance();

	// プレイヤー
	player_ = std::make_shared<Player>();
	player_->Init();

	// 敵ゴーレム
	enemy_ = std::make_shared<EnemyGolem>(*this);
	enemy_->Init();
	enemy_->SetPlayer(player_);

	player_->SetEnemy(enemy_);

	// ステージ
	stage_ = std::make_shared<Stage>(player_,enemy_);
	stage_->Init(Stage::NAME::MAIN_PLANET);
	SceneManager::GetInstance().SetStage(stage_);
	// ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	//カード
	card_ = &CardManager::GetInstance();
	card_->Init();
	card_->SetPlayer(player_.get());
	card_->SetEnemy(enemy_);


	//ステータス
	panel_ = std::make_unique<GamePanel>(*this,player_,enemy_);
	panel_->Init();

	// スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Init();

	mainCamera.SetFollow(&player_->GetTransform());
	mainCamera.ChangeMode(Camera::MODE::FOLLOW);

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

	if (ins.IsTrgDown(KEY_INPUT_BACK))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
	}

	skyDome_->Update();

	stage_->Update();

	player_->Update();
	
	enemy_->Update();
	
	card_->Update();
	
	panel_->Update();

}

void GameScene::Draw(void)
{
	
	// 背景
	skyDome_->Draw();
	stage_->Draw();
	
	player_->Draw();
	enemy_->Draw();

	card_->Draw();

	stage_->DrawTranslucent();
	
	//ステータス(画面全体の明るさを含む)
	panel_->Draw();

	int mainScreen = SceneManager::GetInstance().GetMainScreen();

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

	player_->Release();

	enemy_->Release();

	card_->Release();

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