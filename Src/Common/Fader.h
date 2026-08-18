#pragma once

class Fader
{

public:
	// アルファ値の最小値・最大値
	static constexpr float ALPHA_MIN = 0.0f;
	static constexpr float ALPHA_MAX = 255.0f;

	// 描画範囲の開始座標
	static constexpr int DRAW_START_X = 0;
	static constexpr int DRAW_START_Y = 0;

	// フェード色
	static constexpr unsigned int FADE_COLOR = 0x000000;

	// ブレンドモード解除時の値
	static constexpr int NO_BLEND_PARAM = 0;
	// フェードが進む速さ
	static constexpr float SPEED_ALPHA = 5.0f;

	// 状態
	enum class STATE
	{
		NONE, 
		FADE_OUT,	// 徐々に暗転
		FADE_IN		// 徐々に明転
	};

	// 状態の取得
	STATE GetState(void) const;

	// フェード処理が終了しているか
	bool IsEnd(void) const;

	// 指定フェードを開始する
	void SetFade(STATE state);

	void Init(void);
	void Update(void);
	void Draw(void);

private:

	// 状態
	STATE state_;

	// 透明度
	float alpha_;

	// 状態(STATE)を保ったまま終了判定を行うため、
	// Update->Draw->Updateの1フレーム判定用
	bool isPreEnd_;

	// フェード処理の終了判定
	bool isEnd_;

};
