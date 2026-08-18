#pragma once
#include <DxLib.h>
class Transform;

class Capsule
{

public :
	// 初期値
	static constexpr float INITIAL_VALUE = 0.0f;

	// カプセルのデバッグ表示に使用する球体の分割数
	static constexpr int CAPSULE_SPHERE_SEGMENTS = 5;

	// カプセル中心のデバッグ表示に使用する球体の半径
	static constexpr float CENTER_RADIUS = 5.0f;

	// カプセル中心のデバッグ表示に使用する球体の分割数
	static constexpr int CENTER_SPHERE_SEGMENTS = 10;

	// カプセル中心の位置を求めるための中間値
	static constexpr float CENTER_RATIO = 0.5f;
	// デバッグ時の簡易カプセル表示の色
	static constexpr int COLOR = 0xffffff;

	// コンストラクタ
	// カプセルをつけるオブジェクトを指定する
	Capsule(const Transform& parent);
	Capsule(const Capsule& base, const Transform& parent);

	// デストラクタ
	~Capsule(void);

	// 描画
	void Draw(void);

	// 親Transformからの相対位置を取得
	VECTOR GetLocalPosTop(void) const;
	VECTOR GetLocalPosDown(void) const;

	// 親Transformからの相対位置をセット
	void SetLocalPosTop(const VECTOR& pos);
	void SetLocalPosDown(const VECTOR& pos);

	// ワールド座標を取得
	VECTOR GetPosTop(void) const;
	VECTOR GetPosDown(void) const;

	// 相対座標を回転させてワールド座標で取得する
	VECTOR GetRotPos(const VECTOR& localPos) const;

	// 半径
	float GetRadius(void) const;
	void SetRadius(float radius);

	// 高さ
	float GetHeight(void) const;

	// カプセルの中心座標
	VECTOR GetCenter(void) const;

	void SetOffset(const VECTOR& offset);  // カプセル全体の位置調整
	VECTOR GetOffset() const;

private :
	VECTOR offset_;

	// カプセルをくっつける相手
	const Transform& transformParent_;

	// 親Transformからの相対位置(上側)
	VECTOR localPosTop_;

	// 親Transformからの相対位置(下側)
	VECTOR localPosDown_;

	// 半径
	float radius_;

};
