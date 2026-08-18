#include "../Application.h"
#include "DrawUtility.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


//minとmaxが勝手に置き換えられないように
#undef min
#undef max

//構造体のコンストラクタなど
#pragma region struct
DrawUtility::IntRGB::IntRGB(int r, int g, int b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}
#pragma endregion

void DrawUtility::DrawStringCenter(const std::string& string, int color, int y, int addX)
{
	int x = (Application::SCREEN_SIZE_X / DOUBLE_VALUE)
		- static_cast<int>(
			GetDrawStringWidth(string.c_str(), strlen(string.c_str()))
			/ DOUBLE_VALUE
			);
	x += addX;
	DrawString(x, y, string.c_str(), color);
}

void DrawUtility::DrawStringCenterScreen(const std::string& string, int color, int y, int screenSize, int addX)
{
	int x = (screenSize / DOUBLE_VALUE) - static_cast<int>(GetDrawStringWidth(string.c_str(), strlen(string.c_str())) / DOUBLE_VALUE);
	x += addX;
	DrawString(x, y, string.c_str(), color);
}

void DrawUtility::DrawBar(Vector2 start, int endX, int width, const IntRGB& color, float value, float valueMax)
{
	int barX = start.x;
	if (value > VALUE_MIN)
	{
		//進行度を計算
		barX = start.x + static_cast<int>((endX - start.x) * (value / valueMax));
	}

	if (start.x < barX)
	{
		DrawBox(start.x, start.y
			, barX, start.y + width
			, GetColor(color.r, color.g, color.b), true);
	}
	if (barX < endX)
	{
		DrawBox(barX, start.y
			, endX, start.y + width
			, 0x000000, true);
	}
}

void DrawUtility::DrawBarImg(int x, int y, int num, int max, int imgT, int imgF, int imgSize, int shakeLine)
{
	constexpr int SHAKE = SHAKE_WIDTH;//揺れる幅

	for (int i = VALUE_MIN; i < max; i++)
	{
		//yをずらす
		int addY = VALUE_MIN;

		int img;
		if (i + VALUE_MAX <= num)
		{
			img = imgT;//表示
		}
		else
		{
			img = imgF;//空白表示
		}
		if (num <= shakeLine)
		{
			//ライン以下になったので震える
			addY = GetRand(SHAKE) - (SHAKE / DOUBLE_VALUE);
		}
		if (img != INVALID_IMAGE_HANDLE)
		{
			DrawRotaGraph(i * (imgSize * DOUBLE_VALUE) + x + imgSize / DOUBLE_VALUE
				, y + imgSize / DOUBLE_VALUE + addY
				, DOUBLE_VALUE, VALUE_MIN, img, true);
		}
	}
}

bool DrawUtility::IsInBox(Vector2 targetPos, Vector2 boxPos1, Vector2 boxPos2)
{
	if (boxPos1.x > boxPos2.x)
	{
		//反転させる
		int x = boxPos2.x;
		boxPos2.x = boxPos1.x;
		boxPos1.x = x;
	}
	if (boxPos1.y > boxPos2.y)
	{
		//反転させる
		int y = boxPos2.y;
		boxPos2.y = boxPos1.y;
		boxPos1.y = y;
	}
	//範囲内か判定する
	return ((targetPos.x >= boxPos1.x && targetPos.x < boxPos2.x) &&
		(targetPos.y >= boxPos1.y && targetPos.y < boxPos2.y));
}

bool DrawUtility::Blink(int counter, int cntDiv)
{
	return (counter / cntDiv) % DOUBLE_VALUE == VALUE_MIN;
}

int DrawUtility::LerpInt(int a, int b, float f)
{
	if (f <= VALUE_MIN)
	{
		return a;
	}
	if (f >= VALUE_MAX)
	{
		return b;
	}
	int ret = static_cast<int>(a + (f * (b - a)));
	//return static_cast<int>(a + (f * (b-a)));
	return ret;
}

unsigned int DrawUtility::LerpColor(const IntRGB& a, const IntRGB& b, float f)
{
	IntRGB ret = IntRGB(LerpInt(a.r, b.r, f),
		LerpInt(a.g, b.g, f),
		LerpInt(a.b, b.b, f));
	return GetColor(LerpInt(a.r, b.r, f),
		LerpInt(a.g, b.g, f),
		LerpInt(a.b, b.b, f));
}

void DrawUtility::DrawBarGlossy(Vector2 start, int endX, int width, const IntRGB& color, float value, float valueMax)
{
	//枠線

	DrawBox(start.x, start.y,
		endX + FRAME_WIDTH_BAR * DOUBLE_VALUE, start.y + width + FRAME_WIDTH_BAR * DOUBLE_VALUE,
		GetColor(FRAME_COLOR_R, FRAME_COLOR_G, FRAME_COLOR_B), true);
	//枠線のぶんだけ移動
	endX += FRAME_WIDTH_BAR;
	start.x += FRAME_WIDTH_BAR;
	start.y += FRAME_WIDTH_BAR;

	int barX = start.x;
	if (value > VALUE_MIN)
	{
		//進行度を計算
		barX = start.x + static_cast<int>((endX - start.x) * (value / valueMax));
	}

	if (start.x < barX)
	{
		int loop = GLOSSY_LOOP_COUNT;
		for (int i = 0; i < loop; i++)
		{
			float t = GLOSSY_GRADIENT_RATE / loop * (loop - 1 - i);
			//グラデーション
			//DrawBox(start.x, start.y + i
			//	, barX, start.y + width - i
			//	, LerpColor(color, { 255,255,255 }, 1.0f / loop * i), true);
			DrawBox(std::min(barX, static_cast<int>(start.x + (i * static_cast<float>(endX - start.x) / loop)))
				, start.y
				, barX, start.y + width
				, LerpColor(color, { COLOR_MIN,COLOR_MIN,COLOR_MIN }, t), true);
		}
	}
	if (barX < endX)
	{
		DrawBox(barX, start.y
			, endX, start.y + width
			, 0x000000, true);
	}
}
