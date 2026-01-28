#pragma once

// Enable FrameCapture ImGui display even in Release builds when set to 1.
#define ENABLE_FRAMECAPTURE_IMGUI 1

#include "System/Singleton/SingletonTemplate.h"
#include <D3D11.h>
#include <vector>
#include <memory>
#include <string>

/**************************************************
*	フレームキャプチャマネージャ.
*	GameMain開始から指定秒数のフレームをキャプチャし、
*	ボタン押下でループ再生する機能を提供.
**/
class FrameCaptureManager final : public Singleton<FrameCaptureManager>
{
	friend class Singleton<FrameCaptureManager>;
private:
	FrameCaptureManager();
public:
	~FrameCaptureManager();

	// 初期化（シェーダー、頂点バッファ等の生成）
	void Initialize();
	
	// 解放
	void Release();

	// キャプチャ開始
	// @param seconds キャプチャする秒数
	// @param fps     キャプチャするフレームレート
	void StartCapture(float seconds, int fps = 30);

	// キャプチャ停止
	void StopCapture();

	// 更新処理（キャプチャ中はフレームコピーを実行）
	// @param deltaTime フレーム経過時間
	void Update(float deltaTime);

	// バックバッファをコピー（Render完了後、Present前に呼ぶ）
	void CaptureFrame();

	// 再生開始
	// @param loop ループ再生するかどうか
	void StartPlayback(bool loop = true);

	// 再生停止
	void StopPlayback();

	// 再生用描画（再生中のみ呼ぶ）
	// @param deltaTime フレーム経過時間
	void RenderPlayback(float deltaTime);

	// 状態取得
	bool IsCapturing() const { return m_bCapturing; }
	bool IsPlaying() const { return m_bPlaying; }
	int  GetCapturedFrameCount() const { return static_cast<int>(m_CapturedFrameCount); }
	int  GetMaxFrameCount() const { return m_MaxFrames; }

	// 設定の読み込み/保存
	void LoadSettings(const std::string& filePath = "Data/Json/FrameCaptureSettings.json");
	void SaveSettings(const std::string& filePath = "Data/Json/FrameCaptureSettings.json");

	// ImGuiデバッグ表示
	void DebugImGui();

	// 設定値取得・設定
	float GetCaptureDuration() const { return m_CaptureDuration; }
	void SetCaptureDuration(float seconds) { m_CaptureDuration = seconds; }
	int  GetCaptureFPS() const { return m_CaptureFPS; }
	void SetCaptureFPS(int fps) { m_CaptureFPS = fps; }

    // 常時ロールバッファキャプチャ（Gameシーン開始から常に保存）
    // sampleIntervalFrames: 何フレームごとに1枚保存するか（例:30 で30フレームに1回）
    // assumedFPS: バッファサイズ計算時に用いる想定FPS（既定 60）。
    // バッファサイズ = 3分 * assumedFPS / sampleIntervalFrames + 10
    void StartRollingCapture(int sampleIntervalFrames = 30, int assumedFPS = 60);
    bool IsRolling() const { return m_bRolling; }

	// 再生トリガーキー（VK_F9をデフォルトとする）
	void SetPlaybackTriggerKey(int vkKey) { m_PlaybackTriggerKey = vkKey; }
	int  GetPlaybackTriggerKey() const { return m_PlaybackTriggerKey; }

private:
	// フレーム保存用テクスチャの作成
	void CreateCaptureTextures();
	// フレーム保存用テクスチャの解放
	void ReleaseCaptureTextures();
	// フルスクリーンクワッド用リソースの作成
	void CreateFullscreenQuadResources();
	// フルスクリーンクワッド用リソースの解放
	void ReleaseFullscreenQuadResources();

private:
	// キャプチャ設定
	float m_CaptureDuration;        // キャプチャ秒数
	int   m_CaptureFPS;             // キャプチャFPS
	int   m_MaxFrames;              // 最大フレーム数 = CaptureDuration * CaptureFPS

	// キャプチャ状態
	bool  m_bCapturing;             // キャプチャ中フラグ
	float m_CaptureTimer;           // キャプチャ経過時間
	float m_FrameInterval;          // フレーム間隔 = 1.0f / CaptureFPS
	float m_FrameAccumulator;       // フレーム蓄積時間
	int   m_WriteIndex;             // 書き込み位置（リングバッファ）
	int   m_CapturedFrameCount;     // キャプチャ済みフレーム数

	// 再生状態
	bool  m_bPlaying;               // 再生中フラグ
	bool  m_bLoopPlayback;          // ループ再生フラグ
	int   m_PlaybackIndex;          // 再生位置
	float m_PlaybackAccumulator;    // 再生用フレーム蓄積時間

	// 操作トリガー
	int   m_PlaybackTriggerKey;     // 再生開始キー（VKコード）

	// DirectX11リソース
	std::vector<ID3D11Texture2D*>          m_CaptureTextures;   // キャプチャ用テクスチャ
	std::vector<ID3D11ShaderResourceView*> m_CaptureSRVs;       // シェーダーリソースビュー
	std::vector<ID3D11RenderTargetView*>    m_CaptureRTVs;       // レンダーターゲットビュー（ダウンサンプリング用）

	// キャプチャ解像度
	int m_CaptureWidth;
	int m_CaptureHeight;
    // 実際に作成するキャプチャテクスチャのサイズ（ダウンサンプリング後）
    int m_TargetCaptureWidth;
    int m_TargetCaptureHeight;
    // ダウンサンプリング因子（1 = オリジナル, 2 = 半分幅/高さ -> 1/4 面積）
    int m_DownsampleFactor;

	// デバッグ用選択フレーム
	int m_DebugSelectedFrame;
    
    // ロールバッファモードフラグ
    bool m_bRolling;
    // サンプリング間隔（フレーム数）
    int m_SampleIntervalFrames;
    // 想定FPS（バッファサイズ計算用）
    int m_AssumedFPS;
    // ゲームフレームカウンタ（ロールキャプチャ用）
    int m_FrameCounter;
    // 巻き戻しモード
    bool m_bRewindMode;
    // 再読み込みフラグ（巻き戻し完了後にシーンを再構築）
    bool m_bReloadOnComplete;

	// フルスクリーンクワッド用リソース
	ID3D11Buffer*             m_pFullscreenVB;      // 頂点バッファ
	ID3D11SamplerState*       m_pSamplerState;      // サンプラーステート
	ID3D11VertexShader*       m_pVertexShader;      // 頂点シェーダー
	ID3D11PixelShader*        m_pPixelShader;       // ピクセルシェーダー
	ID3D11InputLayout*        m_pInputLayout;       // 入力レイアウト

	// 初期化済みフラグ
	bool m_bInitialized;

    // 動的にテクスチャを作成するヘルパ
    bool CreateCaptureTextureAt(int index);
};
