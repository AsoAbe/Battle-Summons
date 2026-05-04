#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Renderer/ModelMaterial.h"
#include "../Renderer/ModelRenderer.h"
#include "Metal.h"

Metal::Metal(void)
{
}

Metal::~Metal(void)
{
}

void Metal::Init(void)
{
	//月
	// モデル制御の基本情報
	transform_.SetModel(
		resMng_.LoadModelDuplicate(
			ResourceManager::SRC::MOON));
	transform_.scl = { 1.0f,1.0f,1.0f };
	transform_.pos = { -1000.0f,81.0f,201.0f };
	transform_.Update();

	//モデル描画用
	material_ = std::make_unique<ModelMaterial>(
		"MetalModelVS.cso", 0,
		"MetalModelPS.cso", 6);
	//拡散光
	material_->AddConstBufPS({ 1.0f,1.0f,1.0f,1.0f, });
	//環境光
	material_->AddConstBufPS({ 0.2f,0.2f,0.2f,1.0f, });
	//反射光
	material_->AddConstBufPS({ 1.0f,1.0f,1.0f,1.0f,});	
	
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


	renderer_ = std::make_unique<ModelRenderer>(
		transform_.modelId, *material_);
}

void Metal::Update(void)
{
}

void Metal::Draw(void)
{
	//カメラの位置
	VECTOR cameraPos = SceneManager::GetInstance().GetCamera().GetPos();
	material_->SetConstBufPS(4,{ cameraPos.x,cameraPos.y,cameraPos.z,1.0f, });

	renderer_->Draw();
}
