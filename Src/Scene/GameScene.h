#pragma once
#include <memory>
#include "../Object/EnemyGolem.h"
#include"../UI/BGM.h"
#include "SceneBase.h"
class Stage;
class SkyDome;
class Player;
class Metal;
class Water;
class PixelMaterial;
class PixelRenderer;
class GamePanel;
class CardManager;
class BGM;

class GameScene : public SceneBase
{

public:
	enum class GAME_STATE
	{
		GAME,			//メインゲーム
		TUTORIAL_JUMP,	//ジャンプのチュートリアル
		TUTORIAL_SHOT,	//カウンターのチュートリアル
		TUTORIAL_END,	//チュートリアル終了
		GAMEOVER,		//ゲームオーバー
		GAMECLEAR		//ゲームクリア
	};

	// ポストエフェクトモード
	enum class MODE
	{
		MAIN,
		MONO,
		SCAN,
		LENS,
		VINE,
		MAX
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

	void OnGameOver();

private:

	// ステージ
	std::shared_ptr<Stage> stage_;

	// スカイドーム
	std::unique_ptr<SkyDome> skyDome_;

	// プレイヤー
	std::shared_ptr<Player> player_;

	// 敵ゴーレム
	//std::unique_ptr<EnemyGolem> enemy_;
	std::shared_ptr<EnemyGolem> enemy_;
	
	//ステータス
	std::unique_ptr<GamePanel> panel_;
	
	//GamePanel* panel_;
	//カード
	CardManager* card_;

	// 月
	std::unique_ptr<Metal> metal_;
	//水
	std::unique_ptr<Water> water_;

	//BGM
	BGM bgm_;

	// ポストエフェクトモード
	MODE mode_;

	// ポストエフェクト用スクリーン
	int postEffectScreen_;
	
	// ポストエフェクト用(モノクロ)
	std::unique_ptr<PixelMaterial> monoMaterial_;
	std::unique_ptr<PixelRenderer> monoRenderer_;

	// ポストエフェクト用(走査線)
	std::unique_ptr<PixelMaterial> scanMaterial_;
	std::unique_ptr<PixelRenderer> scanRenderer_;

	// ポストエフェクト用(ビネット)
	std::unique_ptr<PixelMaterial> vineMaterial_;
	std::unique_ptr<PixelRenderer> vineRenderer_;

	// ポストエフェクト用(レンズの歪み)
	std::unique_ptr<PixelMaterial> lensMaterial_;
	std::unique_ptr<PixelRenderer> lensRenderer_;

};
