#include "FrameCaptureManager.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "Game/05_InputDevice/Input.h"
#include "Game/04_Time/Time.h"

#include <d3dcompiler.h>
#include <fstream>

#pragma comment(lib, "d3dcompiler.lib")

#if _DEBUG
#include "System/Singleton/ImGui/CImGuiManager.h"
#endif

// FileManagerを利用してjson読み書き
#include "System/Utility/FileManager/FileManager.h"

namespace
{
	// フルスクリーンクワッド頂点構造体
	struct FullscreenVertex
	{
		float x, y, z;
		float u, v;
	};

	// フルスクリーンクワッド頂点データ（2つの三角形）
	const FullscreenVertex g_FullscreenQuadVertices[] =
	{
		// 左上三角形
		{ -1.0f,  1.0f, 0.0f, 0.0f, 0.0f },
		{  1.0f,  1.0f, 0.0f, 1.0f, 0.0f },
		{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
		// 右下三角形
		{  1.0f,  1.0f, 0.0f, 1.0f, 0.0f },
		{  1.0f, -1.0f, 0.0f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
	};

	// 頂点シェーダー（インラインHLSL）
	const char* g_VS_Source = R"(
		struct VS_INPUT
		{
			float3 Pos : POSITION;
			float2 Tex : TEXCOORD0;
		};
		struct PS_INPUT
		{
			float4 Pos : SV_POSITION;
			float2 Tex : TEXCOORD0;
		};
		PS_INPUT main(VS_INPUT input)
		{
			PS_INPUT output;
			output.Pos = float4(input.Pos, 1.0f);
			output.Tex = input.Tex;
			return output;
		}
	)";

	// ピクセルシェーダー（インラインHLSL）
	const char* g_PS_Source = R"(
		Texture2D g_Texture : register(t0);
		SamplerState g_Sampler : register(s0);
		struct PS_INPUT
		{
			float4 Pos : SV_POSITION;
			float2 Tex : TEXCOORD0;
		};
		float4 main(PS_INPUT input) : SV_Target
		{
			return g_Texture.Sample(g_Sampler, input.Tex);
		}
	)";
}

// コンストラクタ
FrameCaptureManager::FrameCaptureManager()
	: m_CaptureDuration(5.0f)
	, m_CaptureFPS(30)
	, m_MaxFrames(0)
	, m_bCapturing(false)
	, m_CaptureTimer(0.0f)
	, m_FrameInterval(1.0f / 30.0f)
	, m_FrameAccumulator(0.0f)
	, m_WriteIndex(0)
	, m_CapturedFrameCount(0)
	, m_bPlaying(false)
	, m_bLoopPlayback(true)
	, m_PlaybackIndex(0)
	, m_PlaybackAccumulator(0.0f)
	, m_PlaybackTriggerKey(VK_F9)
	, m_pFullscreenVB(nullptr)
	, m_pSamplerState(nullptr)
	, m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pInputLayout(nullptr)
	, m_bInitialized(false)
{
}

// デストラクタ
FrameCaptureManager::~FrameCaptureManager()
{
	Release();
}

// 初期化
void FrameCaptureManager::Initialize()
{
	if (m_bInitialized) return;

	LoadSettings();
	CreateFullscreenQuadResources();

	m_bInitialized = true;
}

// 解放
void FrameCaptureManager::Release()
{
	StopCapture();
	StopPlayback();
	ReleaseCaptureTextures();
	ReleaseFullscreenQuadResources();
	m_bInitialized = false;
}

// キャプチャ開始
void FrameCaptureManager::StartCapture(float seconds, int fps)
{
	if (m_bCapturing || m_bPlaying) return;

	m_CaptureDuration = seconds;
	m_CaptureFPS = fps;
	m_MaxFrames = static_cast<int>(m_CaptureDuration * m_CaptureFPS);
	m_FrameInterval = 1.0f / static_cast<float>(m_CaptureFPS);

	// 既存のテクスチャを解放してから再作成
	ReleaseCaptureTextures();
	CreateCaptureTextures();

	m_CaptureTimer = 0.0f;
	m_FrameAccumulator = 0.0f;
	m_WriteIndex = 0;
	m_CapturedFrameCount = 0;
	m_bCapturing = true;
}

// キャプチャ停止
void FrameCaptureManager::StopCapture()
{
	m_bCapturing = false;
}

// 更新処理
void FrameCaptureManager::Update(float deltaTime)
{
	// キャプチャ中の処理
	if (m_bCapturing)
	{
		m_CaptureTimer += deltaTime;
		m_FrameAccumulator += deltaTime;

		// フレーム間隔ごとにキャプチャ
		while (m_FrameAccumulator >= m_FrameInterval && m_CapturedFrameCount < m_MaxFrames)
		{
			CaptureFrame();
			m_FrameAccumulator -= m_FrameInterval;
		}

		// 指定秒数経過でキャプチャ終了
		if (m_CaptureTimer >= m_CaptureDuration)
		{
			StopCapture();
		}
	}

	// 再生トリガーキー判定（キャプチャ完了後のみ）
	if (!m_bCapturing && m_CapturedFrameCount > 0 && !m_bPlaying)
	{
		if (Input::IsKeyDown(m_PlaybackTriggerKey))
		{
			StartPlayback(true);
		}
	}
}

// バックバッファをコピー
void FrameCaptureManager::CaptureFrame()
{
	if (!m_bCapturing || m_CapturedFrameCount >= m_MaxFrames) return;
	if (m_CaptureTextures.empty()) return;

	auto* pDevice = DirectX11::GetInstance().GetDevice();
	auto* pContext = DirectX11::GetInstance().GetContext();
	auto* pSwapChain = DirectX11::GetInstance().GetSwapChain();
	if (!pDevice || !pContext || !pSwapChain) return;

	// バックバッファを取得
	ID3D11Texture2D* pBackBuffer = nullptr;
	HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr) || !pBackBuffer) return;

	// バックバッファをキャプチャテクスチャにコピー
	int index = m_WriteIndex % m_MaxFrames;
	pContext->CopyResource(m_CaptureTextures[index], pBackBuffer);

	pBackBuffer->Release();

	m_WriteIndex++;
	if (m_CapturedFrameCount < m_MaxFrames)
	{
		m_CapturedFrameCount++;
	}
}

// 再生開始
void FrameCaptureManager::StartPlayback(bool loop)
{
	if (m_bPlaying || m_CapturedFrameCount == 0) return;

	m_bLoopPlayback = loop;
	m_PlaybackIndex = 0;
	m_PlaybackAccumulator = 0.0f;
	m_bPlaying = true;
}

// 再生停止
void FrameCaptureManager::StopPlayback()
{
	m_bPlaying = false;
}

// 再生用描画
void FrameCaptureManager::RenderPlayback(float deltaTime)
{
	if (!m_bPlaying || m_CapturedFrameCount == 0) return;

	auto* pContext = DirectX11::GetInstance().GetContext();
	if (!pContext) return;

	// フレーム進行
	m_PlaybackAccumulator += deltaTime;
	while (m_PlaybackAccumulator >= m_FrameInterval)
	{
		m_PlaybackAccumulator -= m_FrameInterval;
		m_PlaybackIndex++;

		if (m_PlaybackIndex >= m_CapturedFrameCount)
		{
			if (m_bLoopPlayback)
			{
				m_PlaybackIndex = 0;
			}
			else
			{
				StopPlayback();
				return;
			}
		}
	}

    // 現在のフレームを描画
    if (m_CapturedFrameCount == 0) {
        // キャプチャが無ければ再生を停止して呼び出し元へ戻す
        StopPlayback();
        return;
    }
    int frameIndex = m_PlaybackIndex % m_CapturedFrameCount;
    if (m_CaptureSRVs.empty() || !m_CaptureSRVs[frameIndex]) {
        // 無効なSRVなら再生を停止してデフォルト描画にフォールバック
        StopPlayback();
        return;
    }

	// バックバッファをクリア
	DirectX11::GetInstance().ClearBackBuffer();
	DirectX11::GetInstance().ResetRenderTarget();

	// 深度テストOFF、アルファブレンドOFF
	DirectX11::GetInstance().SetDepth(false);
	DirectX11::GetInstance().SetAlphaBlend(false);

	// シェーダー設定
	pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pContext->PSSetShader(m_pPixelShader, nullptr, 0);
	pContext->IASetInputLayout(m_pInputLayout);

	// 頂点バッファ設定
	UINT stride = sizeof(FullscreenVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pFullscreenVB, &stride, &offset);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// テクスチャとサンプラー設定
	pContext->PSSetShaderResources(0, 1, &m_CaptureSRVs[frameIndex]);
	pContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// 描画
	pContext->Draw(6, 0);

	// シェーダーリソースをアンバインド
	ID3D11ShaderResourceView* nullSRV = nullptr;
	pContext->PSSetShaderResources(0, 1, &nullSRV);

	// 深度テストを戻す
	DirectX11::GetInstance().SetDepth(true);
}

// フレーム保存用テクスチャの作成
void FrameCaptureManager::CreateCaptureTextures()
{
	auto* pDevice = DirectX11::GetInstance().GetDevice();
	auto* pSwapChain = DirectX11::GetInstance().GetSwapChain();
	if (!pDevice || !pSwapChain) return;

	// バックバッファからフォーマットとサイズを取得
	ID3D11Texture2D* pBackBuffer = nullptr;
	HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr) || !pBackBuffer) return;

	D3D11_TEXTURE2D_DESC backBufferDesc;
	pBackBuffer->GetDesc(&backBufferDesc);
	pBackBuffer->Release();

	// キャプチャ用テクスチャを作成
    D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = backBufferDesc.Width;
	texDesc.Height = backBufferDesc.Height;
    m_CaptureWidth = backBufferDesc.Width;
    m_CaptureHeight = backBufferDesc.Height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = backBufferDesc.Format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	m_CaptureTextures.resize(m_MaxFrames);
	m_CaptureSRVs.resize(m_MaxFrames);

	for (int i = 0; i < m_MaxFrames; ++i)
	{
		hr = pDevice->CreateTexture2D(&texDesc, nullptr, &m_CaptureTextures[i]);
		if (FAILED(hr))
		{
			m_CaptureTextures[i] = nullptr;
			m_CaptureSRVs[i] = nullptr;
			continue;
		}

		// SRV作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(m_CaptureTextures[i], &srvDesc, &m_CaptureSRVs[i]);
		if (FAILED(hr))
		{
			m_CaptureSRVs[i] = nullptr;
		}
	}
}

// フレーム保存用テクスチャの解放
void FrameCaptureManager::ReleaseCaptureTextures()
{
	for (auto* pSRV : m_CaptureSRVs)
	{
		if (pSRV) pSRV->Release();
	}
	m_CaptureSRVs.clear();

	for (auto* pTex : m_CaptureTextures)
	{
		if (pTex) pTex->Release();
	}
	m_CaptureTextures.clear();

	m_CapturedFrameCount = 0;
	m_WriteIndex = 0;
}

// フルスクリーンクワッド用リソースの作成
void FrameCaptureManager::CreateFullscreenQuadResources()
{
	auto* pDevice = DirectX11::GetInstance().GetDevice();
	if (!pDevice) return;

	HRESULT hr;

	// 頂点バッファ作成
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(g_FullscreenQuadVertices);
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = g_FullscreenQuadVertices;

	hr = pDevice->CreateBuffer(&vbDesc, &vbData, &m_pFullscreenVB);
	if (FAILED(hr)) return;

	// サンプラーステート作成
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = pDevice->CreateSamplerState(&sampDesc, &m_pSamplerState);
	if (FAILED(hr)) return;

	// 頂点シェーダーコンパイル
	ID3DBlob* pVSBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompile(
		g_VS_Source, strlen(g_VS_Source), "VS",
		nullptr, nullptr, "main", "vs_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0,
		&pVSBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob) pErrorBlob->Release();
		return;
	}

	hr = pDevice->CreateVertexShader(
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return;
	}

	// 入力レイアウト作成
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = pDevice->CreateInputLayout(
		layout, 2,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&m_pInputLayout);
	pVSBlob->Release();
	if (FAILED(hr)) return;

	// ピクセルシェーダーコンパイル
	ID3DBlob* pPSBlob = nullptr;
	hr = D3DCompile(
		g_PS_Source, strlen(g_PS_Source), "PS",
		nullptr, nullptr, "main", "ps_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0,
		&pPSBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob) pErrorBlob->Release();
		return;
	}

	hr = pDevice->CreatePixelShader(
		pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(),
		nullptr, &m_pPixelShader);
	pPSBlob->Release();
}

// フルスクリーンクワッド用リソースの解放
void FrameCaptureManager::ReleaseFullscreenQuadResources()
{
	if (m_pInputLayout) { m_pInputLayout->Release(); m_pInputLayout = nullptr; }
	if (m_pPixelShader) { m_pPixelShader->Release(); m_pPixelShader = nullptr; }
	if (m_pVertexShader) { m_pVertexShader->Release(); m_pVertexShader = nullptr; }
	if (m_pSamplerState) { m_pSamplerState->Release(); m_pSamplerState = nullptr; }
	if (m_pFullscreenVB) { m_pFullscreenVB->Release(); m_pFullscreenVB = nullptr; }
}

// 設定の読み込み
void FrameCaptureManager::LoadSettings(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		// デフォルト値を使用
		return;
	}

	try
	{
		json j;
		file >> j;

		if (j.contains("CaptureDuration"))
			m_CaptureDuration = j["CaptureDuration"].get<float>();
		if (j.contains("CaptureFPS"))
			m_CaptureFPS = j["CaptureFPS"].get<int>();
		if (j.contains("PlaybackTriggerKey"))
			m_PlaybackTriggerKey = j["PlaybackTriggerKey"].get<int>();
	}
	catch (...)
	{
		// パースエラー時はデフォルト値を使用
	}
}

// 設定の保存
void FrameCaptureManager::SaveSettings(const std::string& filePath)
{
	json j;
	j["CaptureDuration"] = m_CaptureDuration;
	j["CaptureFPS"] = m_CaptureFPS;
	j["PlaybackTriggerKey"] = m_PlaybackTriggerKey;

	std::ofstream file(filePath);
	if (file.is_open())
	{
		file << j.dump(2);
	}
}

// ImGuiデバッグ表示
void FrameCaptureManager::DebugImGui()
{
#if _DEBUG
	if (ImGui::Begin(IMGUI_JP("フレームキャプチャ")))
	{
		ImGui::Text(IMGUI_JP("状態: %s"), 
			m_bCapturing ? IMGUI_JP("キャプチャ中") : 
			(m_bPlaying ? IMGUI_JP("再生中") : IMGUI_JP("待機中")));

		ImGui::Text(IMGUI_JP("キャプチャ済みフレーム: %d / %d"), m_CapturedFrameCount, m_MaxFrames);

		if (m_bCapturing)
		{
			float progress = m_CaptureTimer / m_CaptureDuration;
			ImGui::ProgressBar(progress, ImVec2(-1, 0), IMGUI_JP("キャプチャ進行"));
		}

		if (m_bPlaying)
		{
			float progress = static_cast<float>(m_PlaybackIndex) / static_cast<float>(m_CapturedFrameCount);
			ImGui::ProgressBar(progress, ImVec2(-1, 0), IMGUI_JP("再生進行"));
		}

		ImGui::Separator();
		ImGui::Text(IMGUI_JP("設定"));

		float duration = m_CaptureDuration;
		if (ImGui::SliderFloat(IMGUI_JP("キャプチャ秒数"), &duration, 1.0f, 10.0f))
		{
			m_CaptureDuration = duration;
		}

		int fps = m_CaptureFPS;
		if (ImGui::SliderInt(IMGUI_JP("キャプチャFPS"), &fps, 10, 60))
		{
			m_CaptureFPS = fps;
		}

		// メモリ見積もり表示
		int estimatedFrames = static_cast<int>(m_CaptureDuration * m_CaptureFPS);
		float estimatedMemoryMB = estimatedFrames * (1920.0f * 1080.0f * 4.0f) / (1024.0f * 1024.0f);
		ImGui::Text(IMGUI_JP("推定メモリ使用量: %.1f MB"), estimatedMemoryMB);

		ImGui::Separator();

		if (!m_bCapturing && !m_bPlaying)
		{
			if (ImGui::Button(IMGUI_JP("キャプチャ開始")))
			{
				StartCapture(m_CaptureDuration, m_CaptureFPS);
			}
		}
		else if (m_bCapturing)
		{
			if (ImGui::Button(IMGUI_JP("キャプチャ停止")))
			{
				StopCapture();
			}
		}

		if (!m_bCapturing && m_CapturedFrameCount > 0 && !m_bPlaying)
		{
			if (ImGui::Button(IMGUI_JP("再生開始")))
			{
				StartPlayback(true);
			}
		}
		else if (m_bPlaying)
		{
			if (ImGui::Button(IMGUI_JP("再生停止")))
			{
				StopPlayback();
			}
		}

		ImGui::Separator();
		if (ImGui::Button(IMGUI_JP("設定を保存")))
		{
			SaveSettings();
		}
		ImGui::SameLine();
		if (ImGui::Button(IMGUI_JP("設定を読込")))
		{
			LoadSettings();
		}

        // デバッグ用: 特定フレーム表示
        ImGui::Separator();
        ImGui::Text(IMGUI_JP("解像度: %d x %d"), m_CaptureWidth, m_CaptureHeight);
        if (m_CapturedFrameCount > 0)
        {
            if (ImGui::SliderInt(IMGUI_JP("表示フレーム選択"), &m_DebugSelectedFrame, 0, m_CapturedFrameCount - 1))
            {
                // 選択変更
            }
            // Image 表示 (ImGui::Image expects ImTextureID)
            // DirectX11 SRV を ImTextureID として渡すために uintptr_t キャスト
            if (m_DebugSelectedFrame >= 0 && m_DebugSelectedFrame < m_CapturedFrameCount)
            {
                ID3D11ShaderResourceView* srv = m_CaptureSRVs[m_DebugSelectedFrame];
                if (srv)
                {
                    ImVec2 size(320, 180);
                    ImGui::Image((void*)srv, size);
                }
                else
                {
                    ImGui::Text(IMGUI_JP("選択したフレームのSRVが無効です"));
                }
            }
        }
	}
	ImGui::End();
#endif
}
