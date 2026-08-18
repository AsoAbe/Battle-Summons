#pragma once
#include <string>
#include <DxLib.h>
#include "Transform.h"
class SceneManager;

class SpeechBalloon
{

public:

	// 表示設定
	static constexpr double INITIAL_VISIBLE_TIME = -1.0;
	static constexpr double INVISIBLE_TIME = 0.0;

	// 画面表示判定
	static constexpr float SCREEN_DEPTH_MIN = 0.0f;
	static constexpr float SCREEN_DEPTH_MAX = 1.0f;

	// 吹き出し表示設定
	static constexpr double SPEECH_DRAW_SCALE = 0.5;
	static constexpr int SPEECH_TEXT_OFFSET_Y = 15;

	// 描画色
	static constexpr int TEXT_COLOR = 0xdd0000;
	static constexpr int SPEECH_TEXT_COLOR = 0x000000;

	// 文字を中央揃えするための分割値
	static constexpr int TEXT_CENTER_DIVISOR = 2;

	static constexpr double DRAW_ANGLE = 0.0;

	// 表示種別
	enum class TYPE
	{
		TEXT,
		SPEECH
	};

	// コンストラクタ
	SpeechBalloon(TYPE type, const Transform& parent);

	// デストラクタ
	~SpeechBalloon(void);

	void Draw(void);
	void DrawTextOrg(void);
	void DrawSpeech(void);

	void SetText(const std::string& text);
	void SetTime(double time);

	void SetType(TYPE type);
	void SetVisiblePermanent(void);

	void SetLocalPos(const VECTOR& localPos);

private:

	TYPE type_;

	int image_;

	// trueの場合、永続的に表示する
	bool isVisiblePermanent_;
	double visibleTime_;

	// 吹き出しを付ける親オブジェクト
	const Transform& transformParent_;

	// 表示座標
	VECTOR pos_;

	// 表示位置の調整用
	VECTOR localPos_;

	// 表示文字
	std::string text_;

};
