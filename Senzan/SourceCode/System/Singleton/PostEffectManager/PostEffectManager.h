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

    void SetGray(bool enable);
    bool IsGray() const;

    // 背景テクスチャ（Resolve済み）を取得（Effekseer歪み用）
    ID3D11ShaderResourceView* GetSceneSRV() const { return m_SceneSRV; }

    // 内部テクスチャへのアクセス（Resolve用）
    ID3D11Texture2D* GetSceneMSAATex() const { return m_SceneMSAATex; }
    ID3D11Texture2D* GetSceneResolvedTex() const { return m_SceneResolvedTex; }

private:
    ID3D11Texture2D* m_SceneMSAATex;
    ID3D11Texture2D* m_SceneResolvedTex;
    ID3D11Texture2D* m_SceneDSMSAA;
    ID3D11RenderTargetView* m_SceneRTV;
    ID3D11DepthStencilView* m_SceneDSV;
    ID3D11ShaderResourceView* m_SceneSRV;

    ID3D11SamplerState* m_Sampler;
    ID3D11Buffer* m_FullscreenVB;

    std::unique_ptr<PixelShaderBase>  m_pPixelShader;
    std::unique_ptr<VertexShaderBase> m_pVertexShader;

    bool m_IsGray;
};
