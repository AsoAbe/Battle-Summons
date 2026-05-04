#pragma once
#include "../Common/Quaternion.h"
#include "ActorBase.h"
#include "Stage.h"

class ModelMaterial;
class ModelRenderer;

class Water :
	public ActorBase
{
public:
	// コンストラクタ
	Water(void);

	// デストラクタ
	~Water(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
private:

	//モデル描画用
	std::unique_ptr<ModelMaterial> material_;
	std::unique_ptr<ModelRenderer> renderer_;
};


