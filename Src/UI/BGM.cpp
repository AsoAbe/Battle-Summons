#include<DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include "BGM.h"

BGM::BGM()
{
    bgmPath_ = "";
    bgmName_ = "";
}

BGM::~BGM()
{
    bgmPath_.clear();
    bgmName_.clear();
}

void BGM::Set(BgmType type)
{
    switch (type)
    {
    case BgmType::TITLE:
        bgmPath_ = Application::PATH_BGM + "TitleBGM.mp3";
        bgmName_ = "タイトルBGM";
        break;

    case BgmType::GAME:
        bgmPath_ = Application::PATH_BGM + "GameBGM.mp3";
        bgmName_ = "ゲームBGM";
        break;

    case BgmType::CLEAR:
        bgmPath_ = Application::PATH_BGM + "ClearBGM.mp3";
        bgmName_ = "クリアBGM";
        break;

    case BgmType::GAMEOVER:
        bgmPath_ = Application::PATH_BGM + "GameOverBGM.mp3";
        bgmName_ = "ゲームオーバーBGM";
        break;

    case BgmType::NONE:
    default:
        bgmPath_.clear();
        bgmName_.clear();
        break;
    }
}

int BGM::ToSoundID(BgmType type)
{
    using SID = SoundManager::SOUND_ID;

    switch (type)
    {
    case BgmType::TITLE:    return static_cast<int>(SID::BGM_TITLE);
    case BgmType::GAME:     return static_cast<int>(SID::BGM_GAME);
    case BgmType::CLEAR:    return static_cast<int>(SID::BGM_CLEAR);
    case BgmType::GAMEOVER: return static_cast<int>(SID::BGM_GAMEOVER);
    default:                return static_cast<int>(SID::NONE);
    }
}