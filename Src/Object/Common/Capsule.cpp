#include <DxLib.h>
#include "../../Utility/AsoUtility.h"
#include "Transform.h"
#include "Capsule.h"

Capsule::Capsule(const Transform& parent) : transformParent_(parent)
{
	radius_ = INITIAL_VALUE;
	localPosTop_ = AsoUtility::VECTOR_ZERO;
	localPosDown_ = AsoUtility::VECTOR_ZERO;
	offset_ = VGet(INITIAL_VALUE, INITIAL_VALUE, INITIAL_VALUE); // 初期オフセットなし
}

Capsule::Capsule(const Capsule& base, const Transform& parent) : transformParent_(parent)
{
	radius_ = base.GetRadius();
	localPosTop_ = base.GetLocalPosTop();
	localPosDown_ = base.GetLocalPosDown();
	offset_ = base.GetOffset();
}

Capsule::~Capsule(void)
{
}

void Capsule::Draw(void)
{

	// 上の球体
	VECTOR pos1 = GetPosTop();
	DrawSphere3D(pos1, radius_, CAPSULE_SPHERE_SEGMENTS, COLOR, COLOR, false);

	// 下の球体
	VECTOR pos2 = GetPosDown();
	DrawSphere3D(pos2, radius_, CAPSULE_SPHERE_SEGMENTS, COLOR, COLOR, false);

	VECTOR dir;
	VECTOR s;
	VECTOR e;

	// 球体を繋ぐ線(X+)
	dir = transformParent_.GetRight();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, COLOR);

	// 球体を繋ぐ線(X-)
	dir = transformParent_.GetLeft();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, COLOR);

	// 球体を繋ぐ線(Z+)
	dir = transformParent_.GetForward();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, COLOR);

	// 球体を繋ぐ線(Z-)
	dir = transformParent_.GetBack();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, COLOR);

	// カプセルの中心
	DrawSphere3D(GetCenter(), CENTER_RADIUS,
		CENTER_SPHERE_SEGMENTS, COLOR, COLOR, true);

}

VECTOR Capsule::GetLocalPosTop(void) const
{
	return localPosTop_;
}

VECTOR Capsule::GetLocalPosDown(void) const
{
	return localPosDown_;
}

void Capsule::SetLocalPosTop(const VECTOR& pos)
{
	localPosTop_ = pos;
}

void Capsule::SetLocalPosDown(const VECTOR& pos)
{
	localPosDown_ = pos;
}

VECTOR Capsule::GetPosTop(void) const
{
	return GetRotPos(localPosTop_);
}

VECTOR Capsule::GetPosDown(void) const
{
	return GetRotPos(localPosDown_);
}

VECTOR Capsule::GetRotPos(const VECTOR& localPos) const
{
	
	VECTOR localWithOffset = VAdd(localPos, offset_);
	VECTOR localRotPos = transformParent_.quaRot.PosAxis(localWithOffset);
	return VAdd(transformParent_.pos, localRotPos);
}

float Capsule::GetRadius(void) const
{
	return radius_;
}

void Capsule::SetRadius(float radius)
{
	radius_ = radius;
}

float Capsule::GetHeight(void) const
{
	return localPosTop_.y;
}

VECTOR Capsule::GetCenter(void) const
{
	VECTOR top = GetPosTop();
	VECTOR down = GetPosDown();

	VECTOR diff = VSub(top, down);
	return VAdd(down, VScale(diff, CENTER_RATIO));
}

void Capsule::SetOffset(const VECTOR& offset)
{
	offset_ = offset;
}

VECTOR Capsule::GetOffset() const
{
	return offset_;
}
