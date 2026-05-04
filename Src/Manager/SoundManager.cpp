#include <DxLib.h>
#include "../Application.h"
#include "SoundManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//変更箇所あり

SoundManager* SoundManager::instance_ = nullptr;
#define PATH_CURSOR        "カーソル移動2.mp3"
#define PATH_BLAST_S      "小爆発.mp3"
#define PATH_BLAST_M      "中爆発.mp3"
#define PATH_BLAST_L      "大爆発.mp3"
#define PATH_CAANON		  "大砲.mp3"

#define PATH_SHOT_S        "小射撃.mp3"
#define PATH_SHOT_M        "中射撃.mp3"
#define PATH_SHOT_L        "大射撃.mp3"
#define PATH_SHOT_BLAST    "射撃の爆発.mp3"

#define PATH_HIT           "被弾音.mp3"
#define PATH_HIT_PLAYER    "被弾音.mp3"
#define PATH_DEATH_PLAYER  "爆発1.mp3"
#define PATH_BOSS          "aaa.mp3"       

#define PATH_SUCCESS       "決定ボタンを押す53.mp3"
#define PATH_JUMP          "キックの素振り3.mp3"
#define PATH_CHARGE        "聖魔法.mp3"
#define PATH_WIN           "聖魔法.mp3"
#define PATH_ENEMY_DASH    "爆発1.mp3"

#define PATH_BGM_TITLE     "TitleBGM.mp3"
#define PATH_BGM_GAME      "GameBGM.mp3"  
#define PATH_BGM_CLEAR     "GameClear.mp3" 
#define PATH_BGM_GAMEOVER  "GameOver.mp3"

SoundManager::SoundData::SoundData(int handle, int volume)
{
	this->handle = handle;
	this->volume = volume;
	this->ct = 0;
}

void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SoundManager();
	}
}

SoundManager& SoundManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		// 安全策: ここで生成しておけば呼び出し側で CreateInstance を忘れても安全
		CreateInstance();
	}
	return *instance_;
}


void SoundManager::Update(void)
{
	//fpsから1フレームの時間を入力する
	CtCountDown(1.0f / Application::FPS);
}

void SoundManager::Draw(void)
{
#ifdef _DEBUG
	DrawFormatString(0, 0, 0xffffff, "LoadedSoundCount = %d", loadedSound_.size());
#endif
}

void SoundManager::Release(void)
{
	//インスタンス削除
	delete instance_;
	instance_ = nullptr;
}

void SoundManager::CtCountDown(float deltaTime)
{
	//クールタイム経過
	for (auto& s : loadedSound_)
	{
		if (s.second.ct > 0)
		{
			s.second.ct -= deltaTime;
		}
		else
		{
			s.second.ct = 0;
		}

	}
}

void SoundManager::ReleaseAllSound(void)
{
	for (auto& m : loadedSound_)
	{
		//サウンドハンドルを取り出してメモリ開放
		DeleteSoundMem(m.second.handle);
	}
	loadedSound_.clear();
	playingBgm_ = SOUND_ID::NONE;
	LoadSound(SOUND_ID::NONE);
}

void SoundManager::LoadSound(SOUND_ID sound, int volume)
{
	if (IsLoaded(sound))
	{
		//既に存在する
		return;
	}
	int handle = -1;
	if (sound != SOUND_ID::NONE)
	{
		handle = LoadSoundMem(GetSoundPath(sound).c_str());
		if (handle != -1)
		{
			ChangeVolumeSoundMem(volume, handle);
		}
	}
	loadedSound_.emplace(sound, SoundData(handle, volume));
}

void SoundManager::LoadBGM(SOUND_ID sound, int volume)
{
	if (IsLoaded(sound))
	{
		//既に存在する
		return;
	}
	int handle = -1;
	if (sound != SOUND_ID::NONE)
	{
		handle = LoadSoundMem(GetBGMPath(sound).c_str());
		if (handle != -1)
		{
			ChangeVolumeSoundMem(volume, handle);
		}
	}
	loadedSound_.emplace(sound, SoundData(handle, volume));
}

void SoundManager::PlaySE(SOUND_ID sound)const
{
	if (IsLoaded(sound))
	{
		PlaySoundMem(GetSoundData(sound)->handle, DX_PLAYTYPE_BACK);
	}
}

void SoundManager::PlaySE(SOUND_ID sound, bool load, int volume)
{
	if (IsLoaded(sound))
	{
		//音量変更
		ChangeSoundVolume(sound, volume);
		PlaySoundMem(GetSoundData(sound)->handle, DX_PLAYTYPE_BACK);
	}
	else if (load)
	{
		LoadSound(sound, volume);
		//ロードしてから鳴らす
		if (IsLoaded(sound))
		{
			PlaySoundMem(GetSoundData(sound)->handle, DX_PLAYTYPE_BACK);
		}
	}
}

void SoundManager::PlaySE_CT(SOUND_ID sound, float ct)
{
	if (IsLoaded(sound))
	{
		SoundData& soundData = loadedSound_.at(sound);
		if (soundData.ct > 0)
		{
			//ct中なので無効
			return;
		}
		//サウンドを再生してctをセット
		PlaySoundMem(soundData.handle, DX_PLAYTYPE_BACK);
		soundData.ct = ct;
	}
}

void SoundManager::ChangeBGM(SOUND_ID sound, bool load, int volume)
{
    // 1) 停止
    const SoundData* playingData = GetSoundData(playingBgm_);
    if (playingData && playingData->handle != -1)
        StopSoundMem(playingData->handle);

    if (sound == SOUND_ID::NONE)
    {
        playingBgm_ = SOUND_ID::NONE;
        return;
    }

    // 2) ロードしていなければロードする
    if (!IsLoaded(sound))
    {
        if (load)
        {
            if (IsBGM(sound))
                LoadBGM(sound, volume);    // ← BGM はこれ
            else
                LoadSound(sound, volume);  // ← SE は従来のこれ（要求通り残す）
        }
        else
        {
            return;
        }
    }

    // 3) データ取得
    const SoundData* data = GetSoundData(sound);
    if (!data || data->handle < 0) return;

    // 4) ボリューム適用 & 再生
    ChangeSoundVolume(sound, volume);
    PlaySoundMem(data->handle, DX_PLAYTYPE_LOOP);

    playingBgm_ = sound;
}

bool SoundManager::IsLoaded(SOUND_ID sound) const
{
	return (loadedSound_.find(sound) != loadedSound_.end());
}

void SoundManager::SetActiveBGM(bool b)
{
	isActiveBGM_ = b;
}

bool SoundManager::IsActiveBGM() const
{
	return isActiveBGM_;
}

int SoundManager::GetBgmVolume() const
{
	return isActiveBGM_ ? VOLUME_BGM : 0;
}

void SoundManager::ChangeVolumeSoundMem_bgm(int volume, int handle)
{
	ChangeVolumeSoundMem(isActiveBGM_ ? volume : 0, handle);
}

int SoundManager::GetBgmVolume(int volume) const
{
	return isActiveBGM_ ? volume : 0;
}

void SoundManager::StopSound(SOUND_ID sound) const
{
	if (GetSoundData(sound) == nullptr)
	{
		//該当なし
		return;
	}
	StopSoundMem(GetSoundData(sound)->handle);
}

SoundManager::SoundManager(void)
{
	playingBgm_ = SOUND_ID::NONE;
	loadedSound_.clear();
	LoadSound(SOUND_ID::NONE);
	isActiveBGM_ = true;

	// BGM は必ず LoadBGM で！
	LoadBGM(SOUND_ID::BGM_TITLE, VOLUME_BGM);
	LoadBGM(SOUND_ID::BGM_GAME, VOLUME_BGM);
	LoadBGM(SOUND_ID::BGM_CLEAR, VOLUME_BGM);
	LoadBGM(SOUND_ID::BGM_GAMEOVER, VOLUME_BGM);
}

SoundManager::~SoundManager(void)
{
	for (auto& m : loadedSound_)
	{
		//サウンドハンドルを取り出してメモリ開放
		DeleteSoundMem(m.second.handle);
	}
	loadedSound_.clear();
}

//ここを修正 retがSOUNDのみになっている,BGM追加

std::string SoundManager::GetSoundPath(SOUND_ID sound)
{
	//constexpr std::string s = ";";
	
	std::string ret = Application::PATH_SOUND;

	switch (sound)
	{
	case SOUND_ID::CURSOR:         ret += PATH_CURSOR; break;
	case SOUND_ID::BLAST_S:       ret += PATH_BLAST_S; break;
	case SOUND_ID::BLAST_M:       ret += PATH_BLAST_M; break;
	case SOUND_ID::BLAST_L:       ret += PATH_BLAST_L; break;
	case SOUND_ID::CAANON:   ret += PATH_CAANON; break;

	case SOUND_ID::SHOT_S:         ret += PATH_SHOT_S; break;
	case SOUND_ID::SHOT_M:         ret += PATH_SHOT_M; break;
	case SOUND_ID::SHOT_L:         ret += PATH_SHOT_L; break;
	case SOUND_ID::SHOT_BLAST:     ret += PATH_SHOT_BLAST; break;

	case SOUND_ID::HIT:            ret += PATH_HIT; break;
	case SOUND_ID::HIT_PLAYER:     ret += PATH_HIT_PLAYER; break;
	case SOUND_ID::DEATH_PLAYER:   ret += PATH_DEATH_PLAYER; break;

	case SOUND_ID::BOSS:           ret += PATH_BOSS; break;

	case SOUND_ID::SUCCESS:        ret += PATH_SUCCESS; break;
	case SOUND_ID::JUMP:           ret += PATH_JUMP; break;
	case SOUND_ID::CHARGE:         ret += PATH_CHARGE; break;
	case SOUND_ID::WIN:            ret += PATH_WIN; break;
	case SOUND_ID::ENEMY_DASH:     ret += PATH_ENEMY_DASH; break;

	case SOUND_ID::NONE:
	default:
		ret = "";
		break;
	}

	return ret;
}


std::string SoundManager::GetBGMPath(SOUND_ID bgm)
{
	//constexpr std::string s = ";";
	
	std::string ret = Application::PATH_BGM;

	switch (bgm)
	{
	case SOUND_ID::BGM_TITLE:      ret += PATH_BGM_TITLE; break;
	case SOUND_ID::BGM_GAME:      ret += PATH_BGM_GAME; break;     
	case SOUND_ID::BGM_CLEAR:     ret += PATH_BGM_CLEAR; break; 
	case SOUND_ID::BGM_GAMEOVER:  ret += PATH_BGM_GAMEOVER; break;
	case SOUND_ID::NONE:
	default:
		ret = "";
		break;
	}

	return ret;
}

bool SoundManager::IsBGM(SOUND_ID bgm)
{
	switch (bgm)
	{
	case SOUND_ID::BGM_TITLE:
	case SOUND_ID::BGM_GAME:
	case SOUND_ID::BGM_CLEAR:
	case SOUND_ID::BGM_GAMEOVER:
		return true;

	default:
		return false;
	}
}

const SoundManager::SoundData* SoundManager::GetSoundData(SOUND_ID sound) const
{
	if (!(IsLoaded(sound)))
	{
		return nullptr;
	}
	return &loadedSound_.at(sound);
}

void SoundManager::ChangeSoundVolume(SOUND_ID sound, int volume)
{
	if (!(IsLoaded(sound)))
	{
		return;
	}
	int& volumeData = loadedSound_.at(sound).volume;
	const int& handle = loadedSound_.at(sound).handle;
	if (volumeData == volume || handle < 0)
	{
		//書き換えなし
		return;
	}
	//書き換えて反映
	volumeData = volume;
	ChangeVolumeSoundMem(volumeData, handle);
}