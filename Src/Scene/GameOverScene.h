#pragma once
#include <memory>
#include "SceneBase.h"
#include"../UI/BGM.h"
#include "../Object/Common/Transform.h"
class SceneManager;
class SkyDome;
class AnimationController;

class GameOverScene : public SceneBase
{
public:

	// クリア画像位置
	static constexpr int CLEAR_X = -40;
	static constexpr int CLEAR_Y = -90;

	// ゲームオーバー画像位置
	static constexpr int OVER_Y = 350;

	// ゲームオーバー画像倍率
	static constexpr float OVER_SCALE = 2.8f;

	// テキスト位置
	static constexpr int TEXT_X = 480;
	static constexpr int TEXT_Y = 520;

	// テキスト色
	static constexpr unsigned int TEXT_COLOR_BLACK = 0x000000;
	static constexpr unsigned int TEXT_COLOR_WHITE = 0xffffff;
	
	// コンストラクタ
	GameOverScene(void);

	// デストラクタ
	~GameOverScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	//解放
	bool  Release(void)override;

private:

	//BGM
	BGM bgm_;

	int Clear;
	int Over;

};
