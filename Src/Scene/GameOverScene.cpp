#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/SkyDome.h"
#include "../Object/Player.h"
#include "GameOverScene.h"

//エフェクトストップを書く　CardManagerにも

GameOverScene::GameOverScene(void)
{

}

GameOverScene::~GameOverScene(void)
{
}

void GameOverScene::Init(void)
{
	Clear = LoadGraph("Data/Image/GameClear.png");
	Over = LoadGraph("Data/Image/GameOver.jpeg");

    if (SceneManager::GetInstance().IsPlayerAlive())
    {
        bgm_.Set(BGM::BgmType::CLEAR);
        SoundManager::GetInstance().ChangeBGM(
            static_cast<SoundManager::SOUND_ID>(BGM::ToSoundID(BGM::BgmType::CLEAR)),
            true,
            SoundManager::VOLUME_BGM
        );
    }
    else
    {
        bgm_.Set(BGM::BgmType::GAMEOVER);
        SoundManager::GetInstance().ChangeBGM(
            static_cast<SoundManager::SOUND_ID>(BGM::ToSoundID(BGM::BgmType::GAMEOVER)),
            true,
            SoundManager::VOLUME_BGM
        );
    }
}

void GameOverScene::Update(void)
{
	// シーン遷移
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		SoundManager::GetInstance().PlaySE(
			SoundManager::SOUND_ID::SUCCESS,
			true,
			SoundManager::VOLUME_STANDARD
		);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

void GameOverScene::Draw(void)
{
	if (SceneManager::GetInstance().IsPlayerAlive())
	{
		DrawGraph(-40, -90, Clear, true);
		DrawFormatString(480, 520, 0x00ffff, "PUSH SPACE", true);
	}
	else
	{
		DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 350, 2.8, 0.0, Over, true);
		DrawFormatString(480, 520, 0xffffff, "PUSH SPACE", true);
	}
	//DrawGraph(- 40, - 90, Clear, true);
	//DrawFormatString(740, 100, 0x00ffff, "GameOverScene", true);
	//DrawFormatString(740, 120, 0x00ffff, "シーン遷移　　：右SHIFT", true);
	
}

bool GameOverScene::Release(void)
{
	DeleteGraph(Clear);
	DeleteGraph(Over);
	return true;
}
