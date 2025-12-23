#pragma once
#include "System/Singleton/SingletonTemplate.h"
#include "Resource/Sound/DirectSound.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

class DirectSound;

class SoundManager final 
    : public Singleton<SoundManager>
{
public:
    SoundManager();
    ~SoundManager();

    // 音声データの一括読み込み (ResourceManagerから呼ばれる)
    static bool LoadSounds(HWND hWnd);

    // 操作系
    static void Play(const std::string& name, bool isLoop = false);
    static void Stop(const std::string& name);
    static void AllStop();
    static void SetVolume(const std::string& name, int volume);

    // リスト取得
    static const std::vector<std::string>& GetKeyList();

private:
    // 内部での読み込み処理
    bool LoadAllInternal(HWND hWnd);
    void UpdateKeyList();

private:
    LPDIRECTSOUND8 m_lpSoundInterface = nullptr;
    // メモリ管理のため unique_ptr に変更
    std::unordered_map<std::string, std::unique_ptr<DirectSound>> m_pDxSounds;
    std::vector<std::string> m_keys;
};