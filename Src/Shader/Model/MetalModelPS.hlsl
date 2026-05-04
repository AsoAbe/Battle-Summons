// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

// 定数バッファ：スロット4番目(b4と書く)
cbuffer cbParam : register(b4)
{
	//拡散光
	float4 g_color;
	//環境光
	float4 g_ambient_color;
	//反射光
	float4 g_specular_color;
	//光の向き
	float3 g_light_dir;
	float g_specular_pow;
	//カメラの位置
	float3 g_camera_pos;
	float dummy;

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

	//法線
	float3 norm = PSInput.normal;

	//拡散光の強さ
	float lightDot = dot(norm,-g_light_dir);

	//頂点からのカメラ方向
	float3 toEye = normalize(g_camera_pos - PSInput.worldPos);

	//反射ベクトル
	float3 reflectDir = normalize(g_light_dir + (norm * 2.0f));

	//反射光の強さ
	float refDot = abs(dot(reflectDir, toEye));
	//反射光の強さ
	float refDotEx = pow(refDot, g_specular_pow);

	//拡散光
	float3 diffuse = (color.rgb * g_color * lightDot);

	//反射光
	float3 specular = refDotEx * g_specular_color;

	//色の合成
	float3 rgb = diffuse + specular + g_ambient_color.rgb;

	//リムライト
	float cDot = abs(dot(norm , toEye));
	//cDot = abs(cDot);
	cDot = 1.0f - cDot;

	float rimDot = pow(cDot, 2.0f); // 強弱を強める
	float4 rimColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	rgb += rimColor * rimDot;

	return float4(rgb, color.a);
}