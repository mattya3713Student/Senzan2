#include "MonoChrome.hlsli"

Texture2D tex0 : register(t0);
SamplerState samp : register(s0);

float4 main(VS_OUT input) : SV_Target
{
    // テクスチャサンプリング
    float3 c = tex0.Sample(samp, input.uv).rgb;

    // NTSC係数による輝度計算
    float g = dot(c, float3(0.299, 0.587, 0.114));

    return float4(g, g, g, 1.0f);
}
