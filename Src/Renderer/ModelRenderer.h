#pragma once
#include <DxLib.h>
#include "../Common/Vector2.h"
#include "../Renderer/ModelMaterial.h"

class ModelRenderer
{

public:
	// テクスチャスロット番号
	static constexpr int TEXTURE_SLOT_BEGIN = 0;

	// テクスチャ解除
	static constexpr int TEXTURE_RELEASE = -1;
	
	// 定数バッファの先頭要素
	static constexpr int FIRST_CONST_BUF_INDEX = 0;

	// 頂点シェーダ用オリジナル定数バッファの使用開始スロット
	static constexpr int CONSTANT_BUF_SLOT_BEGIN_VS = 7;
	// ピクセルシェーダ用オリジナル定数バッファの使用開始スロット
	static constexpr int CONSTANT_BUF_SLOT_BEGIN_PS = 4;

	// コンストラクタ
	ModelRenderer(int modelId_, ModelMaterial& modelMaterial);

	// デストラクタ
	~ModelRenderer(void);

	// 描画
	void Draw(void);

private:

	// モデルのハンドルID
	int modelId_;

	// モデルマテリアル
	ModelMaterial& modelMaterial_;

	// シェーダ設定(頂点)
	void SetReserveVS(void);

	// シェーダ設定(ピクセル)
	void SetReservePS(void);

};
