#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "Common/Transform.h"
#include "../Renderer/ModelMaterial.h"
#include "../Renderer/ModelRenderer.h"
#include "WarpStar.h"
#include "Planet.h"

Planet::Planet(const Stage::NAME& name, const TYPE& type, const Transform& transform)
{

	name_ = name;
	type_ = type;
	transform_ = transform;

	gravityPow_ = 0.0f;
	gravityRadius_ = 0.0f;
	deadLength_ = 0.0f;

}

Planet::~Planet(void)
{
}

void Planet::Init(void)
{
	gravityPow_ = DEFAULT_GRAVITY_POW;
	gravityRadius_ = DEFAULT_GRAVITY_RADIUS;
	deadLength_ = DEFAULT_DEAD_LENGTH;

	//ƒ‚ƒfƒ‹•`‰æ—p
	material_ = std::make_unique<ModelMaterial>(
		"StdModelVS.cso", 0,
		"StdModelPS.cso", 3);
	material_->AddConstBufPS({ 1.0f, 1.0f,1.0f,1.0f });
	VECTOR worldLightDirection = GetLightDirection();
	material_->AddConstBufPS({ worldLightDirection.x,
		worldLightDirection.y,
		worldLightDirection.z,
		1.0f });

	//ŠÂ‹«Œõ
	material_->AddConstBufPS({ 0.2f,0.2f,0.2f,0.0f });

	renderer_ = std::make_unique<ModelRenderer>(
		transform_.modelId, *material_);

}

void Planet::Update(void)
{
}

void Planet::Draw(void)
{
	//MV1DrawModel(transform_.modelId);
	renderer_->Draw();
}

void Planet::SetPosition(const VECTOR& pos)
{
    transform_.pos = pos;
    transform_.Update();
}

void Planet::SetRotation(const Quaternion& rot)
{
	transform_.quaRot = rot;
	transform_.Update();
}

float Planet::GetGravityPow(void) const
{
	return gravityPow_;
}

void Planet::SetGravityPow(float pow)
{
	gravityPow_ = pow;
}

float Planet::GetGravityRadius(void) const
{
	return gravityRadius_;
}

void Planet::SetGravityRadius(float radius)
{
	gravityRadius_ = radius;
}

const Planet::TYPE& Planet::GetType(void) const
{
	return type_;
}

bool Planet::InRangeGravity(const VECTOR& pos) const
{
	return false;
}

bool Planet::InRangeDead(const VECTOR& pos) const
{
	return false;
}

void Planet::SetDeadLength(float len)
{
	deadLength_ = len;
}

const Stage::NAME& Planet::GetName(void) const
{
	return name_;
}
