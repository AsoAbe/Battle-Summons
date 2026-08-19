#include "ModelRenderer.h"

ModelRenderer::ModelRenderer(int modelId, ModelMaterial& modelMaterial)
	: modelId_(modelId), modelMaterial_(modelMaterial)
{
}

ModelRenderer::~ModelRenderer(void)
{
}

void ModelRenderer::Draw(void)
{

	// オリジナルシェーダ設定(ON)
	MV1SetUseOrigShader(true);

	// シェーダ設定(頂点)
	SetReserveVS();

	// シェーダ設定(ピクセル)
	SetReservePS();

	// テクスチャアドレスタイプの取得
	auto texA = modelMaterial_.GetTextureAddress();
	int texAType = static_cast<int>(texA);

	// テクスチャアドレスタイプを変更
	SetTextureAddressModeUV(texAType, texAType);

	// 描画
	MV1DrawModel(modelId_);

	// テクスチャアドレスタイプを元に戻す
	SetTextureAddressModeUV(DX_TEXADDRESS_WRAP, DX_TEXADDRESS_WRAP);

	// 後始末
	//-----------------------------------------

	// テクスチャ解除
	const auto& textures = modelMaterial_.GetTextures();
	size_t size = textures.size();
	if (size == TEXTURE_SLOT_BEGIN)
	{
		// 前回使用分のテクスチャを引き継がないように
		SetUseTextureToShader(TEXTURE_SLOT_BEGIN, TEXTURE_RELEASE);
	}
	else
	{
		for (const auto& pair : textures)
		{
			SetUseTextureToShader(pair.first, TEXTURE_RELEASE);
		}
	}

	// 頂点シェーダ解除
	SetUseVertexShader(TEXTURE_RELEASE);

	// ピクセルシェーダ解除
	SetUsePixelShader(TEXTURE_RELEASE);

	// オリジナルシェーダ設定(OFF)
	MV1SetUseOrigShader(false);
	//-----------------------------------------

}

void ModelRenderer::SetReserveVS(void)
{

	// 定数バッファハンドル
	int constBuf = modelMaterial_.GetConstBufVS();

	FLOAT4* constBufsPtr = (FLOAT4*)GetBufferShaderConstantBuffer(constBuf);
	const auto& constBufs = modelMaterial_.GetConstBufsVS();

	size_t size = constBufs.size();
	for (int i = TEXTURE_SLOT_BEGIN; i < size; i++)
	{
		if (i != TEXTURE_SLOT_BEGIN)
		{
			constBufsPtr++;
		}
		constBufsPtr->x = constBufs[i].x;
		constBufsPtr->y = constBufs[i].y;
		constBufsPtr->z = constBufs[i].z;
		constBufsPtr->w = constBufs[i].w;
	}

	// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
	UpdateShaderConstantBuffer(constBuf);

	// 頂点シェーダー用の定数バッファを定数バッファレジスタにセット
	SetShaderConstantBuffer(
		constBuf, DX_SHADERTYPE_VERTEX, CONSTANT_BUF_SLOT_BEGIN_VS);

	// 頂点シェーダー設定
	SetUseVertexShader(modelMaterial_.GetShaderVS());

}

void ModelRenderer::SetReservePS(void)
{

	// ピクセルシェーダーにテクスチャを転送
	const auto& textures = modelMaterial_.GetTextures();
	size_t size = textures.size();
	if (size == TEXTURE_SLOT_BEGIN)
	{
		// 前回使用分のテクスチャを引き継がないように
		SetUseTextureToShader(TEXTURE_SLOT_BEGIN, TEXTURE_RELEASE);
	}
	else
	{
		for (const auto& pair : textures)
		{
			SetUseTextureToShader(pair.first, pair.second);
		}
	}

	// 定数バッファハンドル
	int constBuf = modelMaterial_.GetConstBufPS();

	FLOAT4* constBufsPtr = (FLOAT4*)GetBufferShaderConstantBuffer(constBuf);
	const auto& constBufs = modelMaterial_.GetConstBufsPS();

	size = constBufs.size();
	for (int i = TEXTURE_SLOT_BEGIN; i < size; i++)
	{
		if (i != TEXTURE_SLOT_BEGIN)
		{
			constBufsPtr++;
		}
		constBufsPtr->x = constBufs[i].x;
		constBufsPtr->y = constBufs[i].y;
		constBufsPtr->z = constBufs[i].z;
		constBufsPtr->w = constBufs[i].w;
	}

	// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
	UpdateShaderConstantBuffer(constBuf);

	// ピクセルシェーダー用の定数バッファを定数バッファレジスタにセット
	SetShaderConstantBuffer(
		constBuf, DX_SHADERTYPE_PIXEL, CONSTANT_BUF_SLOT_BEGIN_PS);

	// ピクセルシェーダー設定
	SetUsePixelShader(modelMaterial_.GetShaderPS());

}
