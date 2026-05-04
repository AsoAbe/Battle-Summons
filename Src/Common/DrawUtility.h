#pragma once

#include<DxLib.h>
#include<string>
#include"Vector2.h"
#include"../Application.h"


class DrawUtility
{
public:

	struct IntRGB
	{
		int r, g, b;
		IntRGB(int r, int g, int b);
	};

	//主にDrawStringの改行に使う
	static constexpr int DEFAULT_TEXT_SIZE = 16;

	//hpバーの枠線
	static constexpr int FRAME_WIDTH_BAR = 2;

	/// <summary>
	/// 点滅時のデフォルト引数
	/// </summary>
	static constexpr int DEFAULT_BLINK = Application::FPS / 2;
	/// <summary>
	/// DEFAULT_BLINKよりも速い間隔
	/// </summary>
	static constexpr int BLINK_HIGH = Application::FPS / 4;

	/// <summary>画面中央にstringを描画する</summary>
	/// <param name="string">テキスト</param>
	/// <param name="color">テキストの色</param>
	/// <param name="y"> スクリーン座標Y </param>
	/// <param name="addX">中心から加算するX(省略可)</param>
	static void DrawStringCenter(const std::string& string, int color, int y, int addX = 0);
	/// <summary>入力したスクリーンサイズを使って画面中央にstringを描画する</summary>
	/// <param name="string">テキスト</param>
	/// <param name="color">テキストの色</param>
	/// <param name="y"> スクリーン座標Y </param>
	/// <param name="screenSize"> スクリーンサイズX </param>
	/// <param name="addX">中心から加算するX(省略可)</param>
	static void DrawStringCenterScreen(const std::string& string, int color, int y, int screenSize, int addX = 0);

	/// <summary> HPバーなどを描画する</summary>
	/// <param name="start">開始座標</param>
	/// <param name="endX">最大到達座標</param>
	/// <param name="width">バーの幅</param>
	/// <param name="color">色</param>
	/// <param name="value">値</param>
	/// <param name="valueMax">値の上限値</param>
	static void DrawBar(Vector2 start, int endX, int width, const IntRGB& color, float value, float valueMax);

	/// <summary>画像でHPゲージなどを描画するときに使う </summary>
	/// <param name="num">現在値</param>
	/// <param name="max">最大値</param>
	/// <param name="imgT">満たしているときの画像ハンドル</param>
	/// <param name="imgF">空のときの画像ハンドル</param>
	/// <param name="shakeLine">この数値以下になると震え始める</param>
	static void DrawBarImg(int x, int y, int num, int max, int imgT, int imgF, int imgSize, int shakeLine);

	/// <summary>targetPosが2つの座標であらわされた箱の中に入っているか判定する(スクリーン座標)</summary>
	static bool IsInBox(Vector2 targetPos, Vector2 boxPos1, Vector2 boxPos2);

	/// <summary>
	/// テキストなどの点滅に使う
	/// </summary>
	/// <param name="counter">1フレームごとに+1される変数を入れる</param>
	/// <param name="cntDiv">Application::FPS / 4などを入れる</param>
	/// <returns>trueで表示falseで非表示</returns>
	static bool Blink(int counter, int cntDiv = DEFAULT_BLINK);

	/// <summary>
	/// std::lerpの結果をintにキャストするのと同じ(c++20未満はstd::lerpが使えない)
	/// </summary>
	static int LerpInt(int a, int b, float f);

	/// <summary>
	/// rgbの線形保管をしてint型で返す
	/// </summary>
	/// <param name="a">色1</param>
	/// <param name="b">色2</param>
	/// <param name="f">線形補間</param>
	/// <returns>0xffffffなどの色を表す値</returns>
	static unsigned int LerpColor(const IntRGB& a, const IntRGB& b, float f);

	/// <summary> HPバーなどを描画する(枠付き)</summary>
	/// <param name="start">開始座標</param>
	/// <param name="endX">最大到達座標</param>
	/// <param name="width">バーの幅</param>
	/// <param name="color">色</param>
	/// <param name="value">値</param>
	/// <param name="valueMax">値の上限値</param>
	static void DrawBarGlossy(Vector2 start, int endX, int width, const IntRGB& color, float value, float valueMax);
};