
// レジスタ b0: 投影行列 (アプリケーション側で更新).
cbuffer ViewProjCBuffer : register(b0)
{
    matrix ViewProj;
};

// 頂点データ (スロット0).
struct VS_INPUT_VERTEX
{
    float4 Position : POSITION;
};

// インスタンスデータ.
// DebugColliderInfo 構造体に相当する.
struct VS_INPUT_INSTANCE
{
    // 1. World (Offset 0 - 48)
    float4 World0 : WORLD0;
    float4 World1 : WORLD1;
    float4 World2 : WORLD2;
    float4 World3 : WORLD3;
    
    float4 Color : COLOR0;
    
    float4 ShapeInfo : SHAPETYPE0;
    
    float4 Data0 : DATA0;
    float4 Data1 : DATA1;
};
// VSからPSへ渡すデータ.
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;  // 最終的なスクリーン座標.
    float4 Color    : COLOR0;       // デバッグカラー.
};

// EP.
VS_OUTPUT VS_Main(VS_INPUT_VERTEX inputV, VS_INPUT_INSTANCE inputI)
{
    VS_OUTPUT output;
    
    // ワールド行列を再構築.
    matrix World = { inputI.World0, inputI.World1, inputI.World2, inputI.World3 };

    // World * ViewProj で最終座標を計算.
    output.Position = mul(mul(inputV.Position, World), ViewProj);
    
    // ピクセルシェーダーへデバッグカラーを渡す.
    output.Color = inputI.Color;
    
    return output;
}
