#pragma once
class Vector2
{
public:
	// 基本値
	static constexpr int ZERO = 0;
	int x;
	int y;

	// コンストラクタ
	Vector2(void);

	// コンストラクタ
	Vector2(int vX, int vY);

	// デストラクタ
	~Vector2(void);

};

