#pragma once
#include <memory>
#include "../Object/EnemyGolem.h"
#include"../UI/BGM.h"
#include "SceneBase.h"
class Stage;
class SkyDome;
class Player;
class PixelMaterial;
class GamePanel;
class CardManager;
class BGM;

class GameScene : public SceneBase
{

public:

	// ビネット強度
	static constexpr float VIGNETTE_POWER = 3.5f;

	// レンズ歪み強度
	static constexpr float LENS_POWER = 3.5f;

	enum class GAME_STATE
	{
		GAME,			//メインゲーム
		TUTORIAL_JUMP,	//ジャンプのチュートリアル
		TUTORIAL_SHOT,	//カウンターのチュートリアル
		TUTORIAL_END,	//チュートリアル終了
		GAMEOVER,		//ゲームオーバー
		GAMECLEAR		//ゲームクリア
	};

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool Release(void);

	void GameOver();
	void GameClear();

	// プレイヤー取得
	std::shared_ptr<Player> GetPlayer() const { return player_; }
	EnemyGolem* GetEnemy() const { return enemy_.get(); }

	//使うサウンドをまとめてロード
	void LoadSceneGameSound();

private:

	// ステージ
	std::shared_ptr<Stage> stage_;

	// スカイドーム
	std::unique_ptr<SkyDome> skyDome_;

	// プレイヤー
	std::shared_ptr<Player> player_;

	// 敵ゴーレム
	std::shared_ptr<EnemyGolem> enemy_;
	
	//ステータス
	std::unique_ptr<GamePanel> panel_;
	
	//カード
	CardManager* card_;

	//BGM
	BGM bgm_;

};
