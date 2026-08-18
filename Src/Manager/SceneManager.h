#pragma once
#include <vector>
#include <memory>
#include <chrono>
#include "../Object/Stage.h"

// 推奨しませんが、どうしても使いたい方は
#define mainCamera SceneManager::GetInstance().GetCamera()

class SceneBase;
class Fader;
class Camera;
class Stage;
class ShotBase;

class SceneManager
{
public:
	// 無効なグラフィックハンドル
	static constexpr int INVALID_HANDLE = -1;

	// 数値の初期値
	static constexpr float INITIAL_VALUE = 0.0f;

	// デルタタイムの基準フレームレート
	static constexpr float DEFAULT_FRAME_RATE = 60.0f;

	// 1秒をナノ秒に変換する値
	static constexpr double NANOSECONDS_PER_SECOND = 1000000000.0;

	// デルタタイムの初期値
	static constexpr float DEFAULT_DELTA_TIME = 1.0f / 60.0f;

	// 背景色
	static constexpr int BG_RED = 0;
	static constexpr int BG_GREEN = 200;
	static constexpr int BG_BLUE = 139;

	// ライトの方向
	static constexpr float LIGHT_DIR_X = 0.3f;
	static constexpr float LIGHT_DIR_Y = -0.7f;
	static constexpr float LIGHT_DIR_Z = 0.8f;

	// フォグの色
	static constexpr int FOG_RED = 5;
	static constexpr int FOG_GREEN = 5;
	static constexpr int FOG_BLUE = 5;

	// フォグ開始距離
	static constexpr float FOG_START = 10000.0f;

	// フォグ終了距離
	static constexpr float FOG_END = 20000.0f;

	// 画面座標の原点
	static constexpr int SCREEN_ORIGIN = 0;

	// シーン管理用
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		GAME,
		GAMEOVER,
	};
	
	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static SceneManager& GetInstance(void);

	void Init(void);
	void Init3D(void);
	void Update(void);
	void Draw(void);

	// リソースの破棄
	void Destroy(void);

	// 状態遷移
	void ChangeScene(SCENE_ID nextId);

	// シーンIDの取得
	SCENE_ID GetSceneID(void);

	// デルタタイムの取得
	float GetDeltaTime(void) const;

	// カメラの取得
	Camera& GetCamera(void);

	int GetMainScreen(void) const;

	float GetTotalTime(void) const;

	std::shared_ptr<Stage> GetStage()const;
	void SetStage(std::shared_ptr<Stage> stage);

	void AddShot(std::shared_ptr<ShotBase> shot);

	void SetPlayerAlive(bool alive);
	bool IsPlayerAlive() const;
private:

	// 静的インスタンス
	static SceneManager* instance_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;

	// 各種シーン
	std::unique_ptr<SceneBase> scene_;

	// フェード
	std::unique_ptr<Fader> fader_;

	// カメラ
	std::unique_ptr<Camera> camera_;

	//ステージ
	std::shared_ptr<Stage> stage_;

	//ショットを管理
	std::vector<std::shared_ptr<ShotBase>> shots_;

	// シーン遷移中判定
	bool isSceneChanging_;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;

	// ゲーム実行時間
	float totalTime_;
	
	// メインスクリーン
	int mainScreen_;

	//プレイヤーの生存判定
	bool isPlayerAlive_ = true;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);
	// コピーコンストラクタも同様
	SceneManager(const SceneManager& manager) = default;
	// デストラクタも同様
	~SceneManager(void) = default;

	// デルタタイムをリセットする
	void ResetDeltaTime(void);

	// シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	// フェード
	void Fade(void);

	void UpdateShots();
	void DrawShots();

};
