#pragma once

#include <map>
class SoundManager
{
public:
	struct SoundData
	{
		int handle;
		int volume;
		float ct;
		SoundData(int handle, int volume = 255);
	};

	enum class SOUND_ID
	{
		NONE,
		CURSOR,
		CAANON,
		BLAST_S,
		BLAST_M,
		BLAST_L,
		SHOT_S,
		SHOT_M,
		SHOT_L,
		SHOT_BLAST,
		HIT,
		HIT_PLAYER,
		DEATH_PLAYER,
		BOSS,
		SUCCESS,
		JUMP,
		CHARGE,
		WIN,
		ENEMY_DASH,
		BGM_TITLE,
		BGM_GAME,
		BGM_CLEAR,    
		BGM_GAMEOVER,
	};

	static constexpr int VOLUME_MAX = 255;
	static constexpr int VOLUME_HIGH = 230;
	static constexpr int VOLUME_STANDARD = 192;
	static constexpr int VOLUME_LOW = 152;
	static constexpr int VOLUME_VERYLOW = 128;

	static constexpr int VOLUME_BGM = 255 * 60 / 100;//255*85/100;

	//インスタンスの生成(シングルトン)(外部から静的にインスタンスを生成)
	static void CreateInstance(void);
	//インスタンスを返す
	static SoundManager& GetInstance();


	//毎フレーム実行処理
	void Update(void);
	//描画処理
	void Draw(void);

	//メモリ解放してインスタンスを破棄
	void Release(void);

	/// <summary>
	/// すべてのクールタイムを減少させる。public関数だが現在はUpdate関数の中で呼んでいるので外部から呼ぶ必要はない
	/// </summary>
	/// <param name="deltaTime">そのフレームで経過した時間(秒)</param>
	void CtCountDown(float deltaTime);

	/// <summary>
	/// サウンドを空にして初期状態に戻す(シーン遷移用)
	/// </summary>
	/// <returns></returns>
	void ReleaseAllSound(void);

	/// <summary>
	/// サウンド読み込み(音量指定)
	/// </summary>
	/// <param name="sound">対象</param>
	/// <param name="volume">音量(255が最大)</param>
	void LoadSound(SOUND_ID sound, int volume = 255);

	void LoadBGM(SOUND_ID sound, int volume);

	/// <summary>
	/// サウンドを再生する。サウンドが見つからない場合は無効
	/// </summary>
	/// <param name="sound">対象</param>
	void PlaySE(SOUND_ID sound)const;

	/// <summary>
	/// 音量を設定してサウンドを再生する。(SoundDataの音量を書き換える)
	/// </summary>
	/// <param name="sound">対象</param>
	/// <param name="load">trueにするとサウンドが見つからない場合にLoadSoundの実行を許可する</param>
	/// <param name="volume">音量を変更する</param>
	void PlaySE(SOUND_ID sound, bool load, int volume = 255);

	/// <summary>
	/// クールタイムを使用してサウンドを再生する。(高頻度に鳴るサウンドで密集するとうるさくなるサウンド用)
	/// </summary>
	/// <param name="sound">対象サウンド</param>
	/// <param name="ct">再生後のクールタイム(秒)</param>
	void PlaySE_CT(SOUND_ID sound, float ct = 0.06f);

	/// <summary>
	/// BGMを変更する
	/// </summary>
	/// <param name="sound"></param>
	/// <param name="load"></param>
	/// <param name="volume"></param>
	void ChangeBGM(SOUND_ID sound, bool load, int volume = 255);

	/// <summary>
	/// 指定したサウンドがロード済みならtrueを返す
	/// </summary>
	bool IsLoaded(SOUND_ID sound)const;

	void SetActiveBGM(bool b);

	bool IsActiveBGM()const;

	/// <summary>
	/// BGMの音量。isActiveBGM_がfalseの時は0を返す
	/// </summary>
	int GetBgmVolume()const;


	/// <summary>
	/// DxLibのChangeVolumeSoundMemを実行する。isActiveBGM_がfalseの時はvolumeを0として扱う
	/// </summary>
	void ChangeVolumeSoundMem_bgm(int volume, int handle);

	/// <summary>
	/// BGMの音量。isActiveBGM_がfalseの時は0を返す
	/// </summary>
	/// <param name="volume">bgmが有効な場合の音量</param>
	/// <returns></returns>
	int GetBgmVolume(int volume)const;

	/// <summary>
	/// 再生中のサウンドを止める
	/// </summary>
	void StopSound(SOUND_ID sound)const;
private:
	//インスタンス
	static SoundManager* instance_;

	//ロード済みサウンド、-1でサウンドなし(エラーの場合でも)
	std::map<SOUND_ID, SoundData> loadedSound_;

	//再生中のBGM
	SOUND_ID playingBgm_;

	//bgmの有効化状態
	bool isActiveBGM_;


	//コンストラクタ
	SoundManager(void);
	//コピーのコンストラクタも潰す(privateに)
	SoundManager(const SoundManager& ins) = delete;

	//デストラクタ
	~SoundManager(void);


	/// <summary>
	/// サウンドのパスを返す
	/// </summary>
	/// <param name="sound"></param>
	/// <returns></returns>
	std::string GetSoundPath(SOUND_ID sound);

	std::string GetBGMPath(SOUND_ID bgm);

	bool IsBGM(SOUND_ID bgm);

	/// <summary>
	/// サウンドデータの参照を返す。実行時点で存在しない場合はnullptr
	/// </summary>
	const SoundData* GetSoundData(SOUND_ID sound)const;

	/// <summary>
	/// 該当するサウンドデータの音量を書き換える
	/// </summary>
	/// <param name="sound"></param>
	void ChangeSoundVolume(SOUND_ID sound, int volume);
};
