#pragma once
#include <memory>
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
#include"../UI/BGM.h"
class SceneManager;
class SkyDome;
class AnimationController;
class BGM;
class InputManager;
class Player;

class TitleScene : public SceneBase
{

public:

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool Release(void) override;
private:

	// 画像
	int imgTitle_;
	int imgPush_;

	// スカイドーム用
	Transform spaceDomeTran_;

	// スカイドーム(背景)
	std::unique_ptr<SkyDome> skyDome_;

	// 惑星
	Transform planet_;
	Transform movePlanet_;

	// キャラクター
	Transform charactor_;

	// アニメーション
	std::unique_ptr<AnimationController> animationController_;

	//BGM
	BGM bgm_;

	// BGMメニューの文字列と関数のテーブル
	using MenuFunc_t = std::function<void(InputManager&)>;
	std::vector<std::string>menuList_b = {};
	std::map<std::string, MenuFunc_t> menuFuncTable_b;
	int bgmplay_;
	int bgmcount_;
	int bgmendcount_ = -1;
	int bgmtamesi_;
	//void bgmCountUpdat();

	float pushAlpha_;     // 0.0 ～ 255.0
	float pushAlphaDir_;  // 変化方向
};
