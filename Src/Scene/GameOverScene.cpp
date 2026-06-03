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


GameOverScene::GameOverScene(void)
{
	Clear = -1;
	Over = -1;
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
	// ÉVÅ[ÉìëJà⁄
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
        DrawGraph(
            CLEAR_X,
            CLEAR_Y,
            Clear,
            true);

        DrawFormatString(
            TEXT_X,
            TEXT_Y,
            TEXT_COLOR_BLACK,
            "PUSH SPACE",
            true);
    }
    else
    {
        DrawRotaGraph(
            Application::SCREEN_SIZE_X / 2,
            OVER_Y,
            OVER_SCALE,
            0.0f,
            Over,
            true);

        DrawFormatString(
            TEXT_X,
            TEXT_Y,
            TEXT_COLOR_WHITE,
            "PUSH SPACE",
            true);
    }
}

bool GameOverScene::Release(void)
{
	DeleteGraph(Clear);
	DeleteGraph(Over);
	return true;
}
