#include "SoundManager.h"
#include <filesystem>
#include <algorithm>

namespace {
constexpr char SOUND_DIRECTORY_PATH[] = "Data\\Sound";
}

SoundManager::SoundManager() : m_lpSoundInterface(nullptr) {}

SoundManager::~SoundManager() {
    m_pDxSounds.clear();
    if (m_lpSoundInterface) {
        m_lpSoundInterface->Release();
        m_lpSoundInterface = nullptr;
    }
}

bool SoundManager::LoadSounds(HWND hWnd) {
    return GetInstance().LoadAllInternal(hWnd);
}

bool SoundManager::LoadAllInternal(HWND hWnd) {
    // インターフェース生成
    if (FAILED(DirectSoundCreate8(NULL, &m_lpSoundInterface, NULL))) return false;
    if (FAILED(m_lpSoundInterface->SetCooperativeLevel(hWnd, DSSCL_NORMAL))) return false;

    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(SOUND_DIRECTORY_PATH)) {
            std::string ext = entry.path().extension().string();
            if (ext != ".wav" && ext != ".WAV") continue;

            std::string name = entry.path().stem().string();
            std::wstring path = entry.path().wstring();

            auto sound = std::make_unique<DirectSound>();
            if (sound->Init(path, m_lpSoundInterface)) {
                m_pDxSounds[name] = std::move(sound);
            }
        }
    }
    catch (...) {
        return false;
    }

    UpdateKeyList();
    return true;
}

void SoundManager::Play(const std::string& name, bool isLoop) {
#ifdef _SOUND_STOP_DEBUG
    return;
#endif // _SOUND_STOP_DEBUG.

    auto& sounds = GetInstance().m_pDxSounds;
    if (sounds.count(name)) sounds[name]->Play(isLoop);
}

void SoundManager::Stop(const std::string& name) {
    auto& sounds = GetInstance().m_pDxSounds;
    if (sounds.count(name)) sounds[name]->Stop();
}

void SoundManager::AllStop() {
    for (auto& pair : GetInstance().m_pDxSounds) {
        pair.second->Stop();
        pair.second->ResetPosition();
    }
}

void SoundManager::SetVolume(const std::string& name, int volume) {
    auto& sounds = GetInstance().m_pDxSounds;
    if (sounds.count(name)) sounds[name]->SetVolume(volume);
}

void SoundManager::UpdateKeyList() {
    m_keys.clear();
    m_keys.reserve(m_pDxSounds.size());
    for (const auto& pair : m_pDxSounds) {
        m_keys.push_back(pair.first);
    }
}

const std::vector<std::string>& SoundManager::GetKeyList() {
    return GetInstance().m_keys;
}
