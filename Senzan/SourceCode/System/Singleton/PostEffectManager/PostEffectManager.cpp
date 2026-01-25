#include "PostEffectManager.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "Graphic/Shader/ShaderResource.h"
#include "Graphic/Shader/ShaderCompile.h"


namespace {
    constexpr char PS_FILE_PATH[] = "Data\\Shader\\Monochrome\\MonochromePS.hlsl";
    constexpr char VS_FILE_PATH[] = "Data\\Shader\\Monochrome\\MonochromeVS.hlsl";
}

//-------------------------------------------------------------------------------------------------------------------------------------

PostEffectManager::PostEffectManager()
   : m_pPixelShader(std::make_unique<PixelShaderBase>())
   , m_pVertexShader(std::make_unique<VertexShaderBase>())
   , m_SceneRTV         (nullptr)
   , m_SceneSRV         (nullptr)
   , m_FullscreenVB     (nullptr)
   , m_IsGray           (false)
   , m_Sampler          (nullptr)
   , m_SceneDSMSAA      (nullptr)
   , m_SceneDSV         (nullptr)
   , m_SceneMSAATex     (nullptr)
   , m_SceneResolvedTex (nullptr)
   , m_CircleGrayCB     (nullptr)
   , m_CircleEffectActive(false)
   , m_CircleRadius     (0.0f)
   , m_IsExpanding      (true)
   , m_ExpandDuration   (0.3f)
   , m_HoldDuration     (0.5f)
   , m_ShrinkDuration   (0.3f)
   , m_EffectTimer      (0.0f)
   , m_EffectPhase      (0)
{
}

PostEffectManager::~PostEffectManager()
{
    SAFE_RELEASE(m_SceneRTV);
    SAFE_RELEASE(m_SceneSRV);
    SAFE_RELEASE(m_Sampler);
    SAFE_RELEASE(m_FullscreenVB);
    SAFE_RELEASE(m_CircleGrayCB);
}

//-------------------------------------------------------------------------------------------------------------------------------------

void PostEffectManager::Initialize()
{
    auto dev = DirectX11::GetInstance().GetDevice();

    // 1. レンダリング用バッファ (MSAAあり).
    D3D11_TEXTURE2D_DESC td{};

    td.Width = WND_W;
    td.Height = WND_H;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 8;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_RENDER_TARGET; // SRVはここでは付けない(MSAAのため).
    dev->CreateTexture2D(&td, nullptr, &m_SceneMSAATex);
    dev->CreateRenderTargetView(m_SceneMSAATex, nullptr, &m_SceneRTV);

    // 2. Resolvedバッファ (サンプリング用 / MSAAなし).
    td.SampleDesc.Count = 1;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    dev->CreateTexture2D(&td, nullptr, &m_SceneResolvedTex);
    dev->CreateShaderResourceView(m_SceneResolvedTex, nullptr, &m_SceneSRV);

    // 3. 深度バッファ (MSAA数を合わせる).
    D3D11_TEXTURE2D_DESC dsd{};
    dsd.Width = WND_W;
    dsd.Height = WND_H;
    dsd.MipLevels = 1;
    dsd.ArraySize = 1;
    dsd.Format = DXGI_FORMAT_D32_FLOAT;
    dsd.SampleDesc.Count = 8; // RTVと一致させる.
    dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    dev->CreateTexture2D(&dsd, nullptr, &m_SceneDSMSAA);
    dev->CreateDepthStencilView(m_SceneDSMSAA, nullptr, &m_SceneDSV);

    // 4. その他リソース (Sampler, VB).
    D3D11_SAMPLER_DESC sdesc{};
    sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sdesc.AddressU = sdesc.AddressV = sdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    dev->CreateSamplerState(&sdesc, &m_Sampler);

    ID3DBlob* pBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    // 頂点インプットレイアウトを定義.
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    // VSの作成.
    ShaderCompile(new std::string(VS_FILE_PATH), "main", "vs_5_0", pBlob, pErrorBlob);
    m_pVertexShader->ConfigInputLayout(layout);
    m_pVertexShader->Init(pBlob);
    SAFE_RELEASE(pErrorBlob);


    // PSの作成.
    ShaderCompile(new std::string(PS_FILE_PATH), "main", "ps_5_0", pBlob, pErrorBlob);
    m_pPixelShader->Init(pBlob);
    SAFE_RELEASE(pErrorBlob);

    // 定数バッファの作成
    D3D11_BUFFER_DESC cbDesc{};
    cbDesc.ByteWidth = sizeof(CircleGrayBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    dev->CreateBuffer(&cbDesc, nullptr, &m_CircleGrayCB);
}

//-------------------------------------------------------------------------------------------------------------------------------------

void PostEffectManager::BeginSceneRender()  
{
    auto ctx = DirectX11::GetInstance().GetContext();

    // 書き込み競合回避.
    ID3D11ShaderResourceView* nullSRV = nullptr;
    ctx->PSSetShaderResources(0, 1, &nullSRV);

    float clearColor[4] = { 0.0f, 0.0f, 0.6f, 1.0f };
    ctx->ClearRenderTargetView(m_SceneRTV, clearColor);
    ctx->ClearDepthStencilView(m_SceneDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    ctx->OMSetRenderTargets(1, &m_SceneRTV, m_SceneDSV);

    auto& dx = DirectX11::GetInstance();
    dx.SetDepth(true);
    dx.SetAlphaBlend(true);
}

//-------------------------------------------------------------------------------------------------------------------------------------

void PostEffectManager::DrawToBackBuffer()
{
    auto& dx = DirectX11::GetInstance();
    auto ctx = dx.GetContext();

    // 【重要】MSAAバッファをResolvedバッファに集約.
    ctx->ResolveSubresource(m_SceneResolvedTex, 0, m_SceneMSAATex, 0, DXGI_FORMAT_R8G8B8A8_UNORM);

    // ターゲットをバックバッファへ戻す.
    ID3D11RenderTargetView* rtv = dx.GetBackBufferRTV();
    ctx->OMSetRenderTargets(1, &rtv, dx.GetBackBufferDSV());

    // 描画ステート設定.
    ctx->IASetInputLayout(nullptr);
    ctx->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    ctx->VSSetShader(m_pVertexShader->GetVertexShader(), nullptr, 0);
    ctx->PSSetShader(m_pPixelShader->GetPixelShader(), nullptr, 0);

    // 定数バッファを更新してセット
    UpdateConstantBuffer();
    ctx->PSSetConstantBuffers(0, 1, &m_CircleGrayCB);

    // Resolvedされたテクスチャを渡す.
    ctx->PSSetShaderResources(0, 1, &m_SceneSRV);
    ctx->PSSetSamplers(0, 1, &m_Sampler);

    ctx->Draw(4, 0);

    // 後片付け.
    ID3D11ShaderResourceView* nullSRV = nullptr;
    ctx->PSSetShaderResources(0, 1, &nullSRV);
}

//-------------------------------------------------------------------------------------------------------------------------------------

void PostEffectManager::SetGray(bool enable)
{
    m_IsGray = enable;
}

bool PostEffectManager::IsGray() const
{
    return m_IsGray;
}

//-------------------------------------------------------------------------------------------------------------------------------------

void PostEffectManager::Update(float deltaTime)
{
    if (!m_CircleEffectActive) return;

    m_EffectTimer += deltaTime;

    switch (m_EffectPhase)
    {
    case 0: // 広がりフェーズ
        m_IsExpanding = true;
        m_CircleRadius = (m_EffectTimer / m_ExpandDuration) * 1.5f;  // 0 -> 1.5
        if (m_EffectTimer >= m_ExpandDuration)
        {
            m_CircleRadius = 1.5f;
            m_EffectTimer = 0.0f;
            m_EffectPhase = 1;
        }
        break;

    case 1: // 維持フェーズ
        m_CircleRadius = 1.5f;
        if (m_EffectTimer >= m_HoldDuration)
        {
            m_EffectTimer = 0.0f;
            m_EffectPhase = 2;
        }
        break;

    case 2: // 戻りフェーズ（中心から通常色が広がる）
        m_IsExpanding = false;
        m_CircleRadius = (m_EffectTimer / m_ShrinkDuration) * 1.5f;  // 0 -> 1.5（中心から広がる）
        if (m_EffectTimer >= m_ShrinkDuration)
        {
            m_CircleRadius = 1.5f;
            m_CircleEffectActive = false;
            m_EffectPhase = 0;
            m_EffectTimer = 0.0f;
        }
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------

void PostEffectManager::StartCircleGrayEffect(float expandDuration, float holdDuration, float shrinkDuration)
{
    m_CircleEffectActive = true;
    m_CircleRadius = 0.0f;
    m_IsExpanding = true;
    m_ExpandDuration = expandDuration;
    m_HoldDuration = holdDuration;
    m_ShrinkDuration = shrinkDuration;
    m_EffectTimer = 0.0f;
    m_EffectPhase = 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------

void PostEffectManager::UpdateConstantBuffer()
{
    auto ctx = DirectX11::GetInstance().GetContext();

    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(ctx->Map(m_CircleGrayCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        CircleGrayBuffer* cb = static_cast<CircleGrayBuffer*>(mapped.pData);
        cb->CircleRadius = m_CircleRadius;
        cb->IsExpanding = m_IsExpanding ? 1.0f : 0.0f;
        cb->EffectActive = m_CircleEffectActive ? 1.0f : 0.0f;
        cb->AspectRatio = static_cast<float>(WND_W) / static_cast<float>(WND_H);
        ctx->Unmap(m_CircleGrayCB, 0);
    }
}
