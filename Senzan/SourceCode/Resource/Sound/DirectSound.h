#pragma once

#include <dsound.h>
#include <mmsystem.h>
#include <string>

// DirectSoundおよびマルチメディアAPI用のライブラリリンク
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")

/******************************************************************
* CDirectSoundクラス.
* 個別の音声データ（セカンダリバッファ）を管理するクラス.
******************************************************************/
class DirectSound
{
public:
    // WAVファイル解析用構造体
    struct WaveData
    {
        WAVEFORMATEX WavFormat;
        char* SoundBuffer;
        DWORD        Size;

        WaveData() : SoundBuffer(nullptr), Size(0) { ZeroMemory(&WavFormat, sizeof(WavFormat)); }
    };

public:
    DirectSound();
    ~DirectSound();

    // 初期化（ファイルパスとDirectSoundインターフェースを受け取る）
    bool Init(const std::wstring& path, LPDIRECTSOUND8 lpSoundInterface);

    // 解放
    void Release();

    // 再生
    void Play(bool isLoop);
    // 停止
    void Stop();
    // 再生位置を最初に戻す
    void ResetPosition();
    // 音量調整 (0 ～ 10000)
    void SetVolume(int volume);

private:
    // WAVファイルの読み込み
    bool LoadWavFile(const std::wstring& fileName, WaveData* outWaveData);

private:
    LPDIRECTSOUNDBUFFER m_lpSoundBuffer;    // セカンダリバッファ
};