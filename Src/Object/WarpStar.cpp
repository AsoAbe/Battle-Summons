#include <DxLib.h>
#include "../Manager/ResourceManager.h"
#include "Common/Transform.h"
#include "Player.h"
#include "../Renderer/ModelMaterial.h"
#include "../Renderer/ModelRenderer.h"
#include "WarpStar.h"

WarpStar::WarpStar(
	std::shared_ptr<Player> player, const Transform& transform) : player_(player)
{

	transform_ = transform;

	state_ = STATE::NONE;

	// 状態管理
	stateChanges_.emplace(STATE::IDLE, std::bind(&WarpStar::ChangeStateIdle, this));
	stateChanges_.emplace(STATE::RESERVE, std::bind(&WarpStar::ChangeStateReserve, this));
	stateChanges_.emplace(STATE::MOVE, std::bind(&WarpStar::ChangeStateMove, this));

}

WarpStar::~WarpStar(void)
{
}

void WarpStar::Init(void)
{

	// モデルの基本情報
	transform_.SetModel(
		resMng_.LoadModelDuplicate(
			ResourceManager::SRC::WARP_STAR)
	);
	transform_.Update();

	material_ = std::make_unique<ModelMaterial>(
		"NoTexVS.cso", 0,
		"NoTexPS.cso", 1);
	material_->AddConstBufPS({ 1.0f, 0.9f,0.1f,0.8f});

	renderer_ = std::make_unique<ModelRenderer>(
		transform_.modelId, *material_);


	ChangeState(STATE::IDLE);

}

void WarpStar::Update(void)
{

	// 更新ステップ
	stateUpdate_();

}

void WarpStar::Draw(void)
{
	renderer_->Draw();
}

void WarpStar::ChangeState(STATE state)
{

	// 状態変更
	state_ = state;

	// 各状態遷移の初期処理
	stateChanges_[state_]();

}

void WarpStar::ChangeStateNone(void)
{
}

void WarpStar::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&WarpStar::UpdateIdle, this);
}

void WarpStar::ChangeStateReserve(void)
{
	stateUpdate_ = std::bind(&WarpStar::UpdateReserve, this);
}

void WarpStar::ChangeStateMove(void)
{
	stateUpdate_ = std::bind(&WarpStar::UpdateMove, this);
}

void WarpStar::UpdateNone(void)
{
}

void WarpStar::UpdateIdle(void)
{
}

void WarpStar::UpdateReserve(void)
{
}

void WarpStar::UpdateMove(void)
{
}
