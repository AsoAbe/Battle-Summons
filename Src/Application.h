#pragma once
#include <string>

class Application
{

public:

	// スクリーンサイズ
	static constexpr int SCREEN_SIZE_X = 1024;
	static constexpr int SCREEN_SIZE_Y = 640;
	static constexpr int MAINGAME_SIZE_X = 720;//メインゲームのスクリーンサイズ
	static constexpr int MAINGAME_POS_X = 280;//メインゲームを中央に寄せる

	//60FPS基準で設定された定数値を調整する用の60
	static constexpr int DEFAULT_FPS = 60;//こっちは変更禁止
	//基準fps
	static constexpr int FPS = 60;
	//キー長押し判定時間(秒)
	static constexpr float LONG_KEY_DOWN_TIME_S = 1.0f;

	//1フレーム当たりの秒数
	static constexpr float FRAME_RATE = (1000.0f / FPS);

	// データパス関連
	//-------------------------------------------
	static const std::string PATH_IMAGE;
	static const std::string PATH_MODEL;
	static const std::string PATH_EFFECT;
	static const std::string PATH_SHADER;
	static const std::string PATH_STAGE;
	static const std::string PATH_BGM;
	static const std::string PATH_SOUND;
	//-------------------------------------------

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static Application& GetInstance(void);

	// 初期化
	void Init(void);

	// ゲームループの開始
	void Run(void);

	// リソースの破棄
	void Destroy(void);

	// 初期化成功／失敗の判定
	bool IsInitFail(void) const;

	// 解放成功／失敗の判定
	bool IsReleaseFail(void) const;

private:

	// 静的インスタンス
	static Application* instance_;

	// 初期化失敗
	bool isInitFail_;

	// 解放失敗
	bool isReleaseFail_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	Application(void);
	Application(const Application& manager) = default;
	~Application(void) = default;

	// Effekseerの初期化
	void InitEffekseer(void);

};
