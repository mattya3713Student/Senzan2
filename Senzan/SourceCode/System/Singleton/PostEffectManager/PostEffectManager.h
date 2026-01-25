#pragma once
#pragma once
#include <wrl/client.h> // Microsoft::WRL::ComPtr 用

class PixelShaderBase;
class VertexShaderBase;

class PostEffectManager final : public Singleton<PostEffectManager>
{
private:
    friend class Singleton<PostEffectManager>;
    PostEffectManager();
public:
    ~PostEffectManager();

    void Initialize();
    void BeginSceneRender();
    void DrawToBackBuffer();
    void Update(float deltaTime);  // 毎フレーム呼び出し

    void SetGray(bool enable);
    bool IsGray() const;

    // 円状グレースケールエフェクト開始
    void StartCircleGrayEffect(float expandDuration = 0.3f, float holdDuration = 0.5f, float shrinkDuration = 0.3f);
    bool IsCircleGrayActive() const { return m_CircleEffectActive; }

    // 背景テクスチャ（Resolve済み）を取得（Effekseer歪み用）
    ID3D11ShaderResourceView* GetSceneSRV() const { return m_SceneSRV; }

    // 内部テクスチャへのアクセス（Resolve用）
    ID3D11Texture2D* GetSceneMSAATex() const { return m_SceneMSAATex; }
    ID3D11Texture2D* GetSceneResolvedTex() const { return m_SceneResolvedTex; }

private:
    // シェーダー用定数バッファ構造体
    struct CircleGrayBuffer
    {
        float CircleRadius;
        float IsExpanding;
        float EffectActive;
        float AspectRatio;
    };

    void UpdateConstantBuffer();

private:
    ID3D11Texture2D* m_SceneMSAATex;
    ID3D11Texture2D* m_SceneResolvedTex;
    ID3D11Texture2D* m_SceneDSMSAA;
    ID3D11RenderTargetView* m_SceneRTV;
    ID3D11DepthStencilView* m_SceneDSV;
    ID3D11ShaderResourceView* m_SceneSRV;

    ID3D11SamplerState* m_Sampler;
    ID3D11Buffer* m_FullscreenVB;
    ID3D11Buffer* m_CircleGrayCB;  // 定数バッファ

    std::unique_ptr<PixelShaderBase>  m_pPixelShader;
    std::unique_ptr<VertexShaderBase> m_pVertexShader;

    bool m_IsGray;

    // 円状エフェクト用変数
    bool  m_CircleEffectActive;
    float m_CircleRadius;
    bool  m_IsExpanding;
    float m_ExpandDuration;
    float m_HoldDuration;
    float m_ShrinkDuration;
    float m_EffectTimer;
    int   m_EffectPhase;  // 0: 広がり, 1: 維持, 2: 戻り
};
