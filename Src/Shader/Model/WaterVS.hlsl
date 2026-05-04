// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"
// IN
#include "../Common/Vertex/VertexInputType.hlsli"
#define VERTEX_INPUT DX_MV1_VERTEX_TYPE_NMAP_1FRAME
// OUT
#define VS_OUTPUT VertexToPixelLit
#include "../Common/Vertex/VertexShader3DHeader.hlsli"

// 定数バッファ：スロット7番目(b7と書く)
cbuffer cbParam : register(b7)
{
	float4 g_uv_scale;

	float g_time; // 時間（秒）
	float3 padding; // アライメントのため
}

VS_OUTPUT main(VS_INPUT VSInput)
{
	VS_OUTPUT ret;

	// ----- 波を追加する処理ここから -----
	float3 modifiedPos = VSInput.pos;

	// パラメータ調整（必要に応じて定数化してもOK）
	float frequency = 2.0f;   // 波の細かさ
	float speed = 2.0f;       // 波の動く速さ
	float amplitude = 0.5f;   // 波の高さ

	// 正弦波でY座標を変化させる（X座標に応じて）
	modifiedPos.y += sin(modifiedPos.x * frequency + g_time * speed) * amplitude;
	// ----- 波を追加する処理ここまで -----


	// 頂点座標変換 +++++++++++++++++++++++++++++++++++++( 開始 )
	float4 lLocalPosition;
	float4 lWorldPosition;
	float4 lViewPosition;
	// float3 → float4
	lLocalPosition.xyz = modifiedPos;
	lLocalPosition.w = 1.0f;
	// ローカル座標をワールド座標に変換(剛体)
	lWorldPosition.w = 1.0f;
	lWorldPosition.xyz = mul(lLocalPosition, g_base.localWorldMatrix);
	ret.worldPos = lWorldPosition.xyz;

	// ワールド座標をビュー座標に変換
	lViewPosition.w = 1.0f;
	lViewPosition.xyz = mul(lWorldPosition, g_base.viewMatrix);
	ret.vwPos.xyz = lViewPosition.xyz;
	// ビュー座標を射影座標に変換
	ret.svPos = mul(lViewPosition, g_base.projectionMatrix);

	// 頂点座標変換 +++++++++++++++++++++++++++++++++++++( 終了 )
	// 
	// 
	// // その他、ピクセルシェーダへ引継&初期化 ++++++++++++( 開始 )
	//ret.uv.x = VSInput.uv0.x;
	//ret.uv.y = VSInput.uv0.y;

	// UV座標

	ret.uv.x = VSInput.uv0.x * g_uv_scale.x;
	ret.uv.y = VSInput.uv0.y * g_uv_scale.y;

	// 法線
	//ret.normal = VSInput.norm;
	ret.normal = normalize(
		mul(VSInput.norm, (float3x3)g_base.localWorldMatrix));

	// ディフューズカラー
	ret.diffuse = VSInput.diffuse;
	// ライト方向(ローカル)
	ret.lightDir = float3(0.0f, 0.0f, 0.0f);
	// ライトから見た座標
	ret.lightAtPos = float3(0.0f, 0.0f, 0.0f);
	// その他、ピクセルシェーダへ引継&初期化 ++++++++++++( 終了 )
	// 出力パラメータを返す


	return ret;
}