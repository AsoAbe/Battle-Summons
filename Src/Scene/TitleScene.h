#pragma once
#include <memory>
#include "SceneBase.h"
#include "../Manager/SoundManager.h"
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
	//アニメーション初期値
	static constexpr int RUN_ANIM_ID = 0;

	// アルファ値の初期値
	static constexpr float PUSH_ALPHA_INITIAL = 0.0f;

	// 画面中央位置
	static constexpr int SCREEN_CENTER_DIVISOR = 2;

	// 描画回転角度
	static constexpr double DRAW_ROTATION_ZERO = 0.0;

	// ブレンドモードのアルファ値
	static constexpr int BLEND_ALPHA_MIN = 0;

	// BGM音量調整率
	static constexpr float BGM_TITLE_VOLUME_RATE = 1.17f;

	// メインBGM音量割合
	static constexpr int BGM_VOLUME_PERCENT = 75;
	static constexpr int PERCENT_BASE = 100;

	static constexpr int appear_interval = 20;  // 出現までのフレーム
	static constexpr int menu_line_height = 40; // メニューの1つあたりの高さ
	static constexpr int margin_size = 20;      // ポーズメニュー枠の余白

	static constexpr int CONTROL_TEXT_SIZE = 18;

	//SoundManager側をそのまま使うと計算が合わなくなるため、後で数値を書き換えてこの定数は不要にする
	static constexpr int VOLUME_BGM_TITLE = static_cast<int>(SoundManager::VOLUME_BGM * 1.17f);
	static constexpr int VOLUME_BGM_MAIN = 255 * 75 / 100;

	static constexpr int TITLE_MODEL_ROT_X = -90;

	// キャラクター配置位置
	static constexpr float CHARACTER_POS_X = -250.0f;
	static constexpr float CHARACTER_POS_Y = -32.0f;
	static constexpr float CHARACTER_POS_Z = -105.0f;

	// キャラクター表示倍率
	static constexpr float CHARACTER_SCALE = 0.4f;

	// キャラクター初期向き(Y軸回転角度)
	static constexpr float CHARACTER_ROT_Y = 90.0f;

	// 走るアニメーション
	static constexpr float RUN_ANIM_SPEED = 20.0f;

	// PUSH SPACE 点滅設定
	static constexpr float PUSH_ALPHA_MAX = 255.0f;
	static constexpr float PUSH_ALPHA_MIN = 50.0f;
	static constexpr float PUSH_ALPHA_SPEED = 2.0f;
	static constexpr float PUSH_ALPHA_DIR_PLUS = 1.0f;
	static constexpr float PUSH_ALPHA_DIR_MINUS = -1.0f;

	// タイトル画像描画位置
	static constexpr int TITLE_DRAW_Y = 350;
	static constexpr double TITLE_DRAW_SCALE = 0.7;

	// PUSH SPACE画像描画位置
	static constexpr int PUSH_DRAW_Y = 500;
	static constexpr double PUSH_DRAW_SCALE = 1.0;
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

	float pushAlpha_;     // 0.0 ～ 255.0
	float pushAlphaDir_;  // 変化方向
};
