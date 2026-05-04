// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

// 定数バッファ：スロット4番目(b4と書く)
cbuffer cbParam : register(b4)
{
	float4 g_color;
	float3 g_light_dir;
	float dummy;
	float4 g_ambient_color;
}

float4 main(PS_INPUT PSInput) : SV_TARGET0
{

	float4 color;

	// テクスチャーの色を取得
	color = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.uv);
	if (color.a < 0.01f)
	{
		discard;
	}

	// 関数の戻り値がラスタライザに渡される
	//return color * g_color;
	//float3 lightDir = g_common.light[0].direction;
	//return float4(lightDir.x, lightDir.y, lightDir.z, 1.0f);

	//return float4(g_light_dir.x, g_light_dir.y, g_light_dir.z, 1.0f);

	float lightDot = dot(PSInput.normal,-g_light_dir);

	float3 rgb = (color.rgb * g_color* lightDot) + g_ambient_color.rgb;

	return float4(rgb, color.a);
}