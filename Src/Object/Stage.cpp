#include <vector>
#include <map>
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "WarpStar.h"
#include "Player.h"
#include "EnemyGolem.h"
#include "Planet.h"
#include "Common/Collider.h"
#include "Common/Transform.h"
#include "Stage.h"

Stage::Stage(std::shared_ptr<Player> player,
	std::shared_ptr<EnemyGolem> enemy)
	: resMng_(ResourceManager::GetInstance())
	, player_(player)
	, enemy_(enemy)
	, activeName_(NAME::MAIN_PLANET)
	, activePlanet_()
	, planets_()
	, warpStars_()
	, nullPlanet(nullptr)
	, step_(INITIAL_VALUE)
{
	activeName_ = NAME::MAIN_PLANET;
	step_ = INITIAL_VALUE;
}

Stage::~Stage(void)
{
	
	// ワープスター
	warpStars_.clear();
	
	// 惑星
	planets_.clear();

}

void Stage::Init(void)
{
	MakeMainStage();
	MakeWarpStar();

	step_ = -STEP_INACTIVE;
}

void Stage::Update(void)
{

	// ワープスター
	for (const auto& s : warpStars_)
	{
		s->Update();
	}

	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Update();
	}

}

void Stage::Draw(void)
{
	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Draw();
	}

}

void Stage::DrawTranslucent(void)
{	
	// ワープスター
	for (const auto& s : warpStars_)
	{
		s->Draw();
	}
}

void Stage::ChangeStage(NAME type)
{

	activeName_ = type;

	// 対象のステージを取得する
	activePlanet_ = GetPlanet(activeName_);

	// ステージの当たり判定をプレイヤーに設定
	player_->ClearCollider();
	player_->AddCollider(activePlanet_.lock()->GetTransform().collider);

	enemy_->ClearCollider();
	enemy_->AddCollider(activePlanet_.lock()->GetTransform().collider);

	step_ = TIME_STAGE_CHANGE;

}

std::weak_ptr<Planet> Stage::GetPlanet(NAME type)
{
	if (planets_.count(type) == 0)
	{
		return nullPlanet;
	}

	return planets_[type];
}

std::weak_ptr<Planet> Stage::GetActivePlanet() const
{
	return activePlanet_;
}

void Stage::MakeMainStage(void)
{

	// 最初の惑星
	//------------------------------------------------------------------------------
	Transform planetTrans;
	planetTrans.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MAIN_PLANET));
	planetTrans.scl = AsoUtility::VECTOR_ONE;
	planetTrans.quaRot = Quaternion();
	planetTrans.pos = { INITIAL_VALUE, -MAIN_PLANET_POS_Y, INITIAL_VALUE };

	// 当たり判定(コライダ)作成
	planetTrans.MakeCollider(Collider::TYPE::STAGE);

	planetTrans.Update();

	NAME name = NAME::MAIN_PLANET;
	std::shared_ptr<Planet> planet =
		std::make_shared<Planet>(
			name, Planet::TYPE::GROUND, planetTrans);
	planet->Init();
	planets_.emplace(name, std::move(planet));

}

void Stage::MakeWarpStar(void)
{

	Transform trans;
	std::unique_ptr<WarpStar> star;

	// 落とし穴惑星へのワープスター
	//------------------------------------------------------------------------------
	trans.pos = { -WARP_STAR_POS_X, WARP_STAR_POS_Y, WARP_STAR_POS_Z };
	trans.scl = { WARP_STAR_SCALE, WARP_STAR_SCALE, WARP_STAR_SCALE };
	trans.quaRot = Quaternion::Euler(
		AsoUtility::Deg2RadF(WARP_STAR_ROT_X),
		AsoUtility::Deg2RadF(WARP_STAR_ROT_Y),
		AsoUtility::Deg2RadF(INITIAL_VALUE)
	);

	star = std::make_unique<WarpStar>(player_, trans);
	star->Init();
	warpStars_.push_back(std::move(star));
	//------------------------------------------------------------------------------

}
