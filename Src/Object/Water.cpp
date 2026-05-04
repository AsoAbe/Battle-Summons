#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Renderer/ModelMaterial.h"
#include "../Renderer/ModelRenderer.h"
#include "Water.h"

Water::Water(void)
{
}

Water::~Water(void)
{
}

void Water::Init(void)
{
	//水
	// モデル制御の基本情報
	transform_.SetModel(
		resMng_.LoadModelDuplicate(
			ResourceManager::SRC::WATER));
	transform_.scl = { 4.0f,1.0f,2.0f };
	transform_.pos = { 300.0f,81.0f,201.0f };
	transform_.Update();
	//モデル描画用
	material_ = std::make_unique<ModelMaterial>(
		"WaterVS.cso", 2,
		"WaterPS.cso", 7);

	//VS---------------------------------
	//スケール
	material_->AddConstBufVS({ 4.0f, 4.0f, 0.0f, 0.0f}); 
	
	//totaltime
	material_->AddConstBufVS({ 0.0f, 0.0f, 0.0f, 0.0f}); 
	


	//PS-----------------------------------------------------
	//拡散光
	material_->AddConstBufPS({ 1.0f,1.0f,1.0f,1.0f, });
	//環境光
	material_->AddConstBufPS({ 0.2f,0.2f,0.2f,1.0f, });
	//反射光
	material_->AddConstBufPS({ 1.0f,1.0f,1.0f,1.0f, });

	//反射光の強さ
	float specularPow = 40.0f;

	//光の向き
	//光の方向の取得
	VECTOR worldLightDirection = GetLightDirection();
	material_->AddConstBufPS({
		worldLightDirection.x,
	worldLightDirection.y,
	worldLightDirection.z,
	specularPow });

	//カメラの位置
	VECTOR cameraPos = SceneManager::GetInstance().GetCamera().GetPos();
	material_->AddConstBufPS({ cameraPos.x,cameraPos.y,cameraPos.z,1.0f, });

	// UVスクロール速度 (float2 + padding) 時間とまとめる(x,y,time,0.0f)
	material_->AddConstBufPS({ 0.1f, 0.05f,0.0f,0.0f }); // slot 5

	renderer_ = std::make_unique<ModelRenderer>(
		transform_.modelId, *material_);
}

void Water::Update(void)
{
	// 経過時間を取得（例：フレームごとの加算時間を取得）
	static float totalTime = 0.0f;
	totalTime += SceneManager::GetInstance().GetDeltaTime(); // 秒単位と想定

	// g_time を渡す（pad用のfloatも一緒に渡す）
	material_->SetConstBufPS(5, { 1.01f,0.85f,totalTime, 0.0f });

	material_->SetConstBufVS(1,{ totalTime, 0.0f, 0.0f, 0.0f });

}

void Water::Draw(void)
{

	renderer_->Draw();
}
