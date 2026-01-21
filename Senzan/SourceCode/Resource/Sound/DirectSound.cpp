#include "DirectSound.h"

DirectSound::DirectSound()
    : m_lpSoundBuffer(nullptr)
{
}

DirectSound::~DirectSound()
{
    Release();
}

bool DirectSound::Init(const std::wstring& path, LPDIRECTSOUND8 lpSoundInterface)
{
    if (!lpSoundInterface) return false;

    WaveData wavData;

    // WAVファイルの読み込み
    if (!LoadWavFile(path, &wavData))
    {
        return false;
    }

    // バッファ情報の設定
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
    dsbd.dwBufferBytes = wavData.Size;
    dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;
    dsbd.lpwfxFormat = &wavData.WavFormat;

    // セカンダリバッファ作成
    if (FAILED(lpSoundInterface->CreateSoundBuffer(&dsbd, &m_lpSoundBuffer, NULL)))
    {
        delete[] wavData.SoundBuffer;
        return false;
    }

    // 波形データをセカンダリバッファに書き込む
    void* bufferPtr = nullptr;
    DWORD bufferSize = 0;
    if (FAILED(m_lpSoundBuffer->Lock(0, wavData.Size, &bufferPtr, &bufferSize, NULL, NULL, 0)))
    {
        delete[] wavData.SoundBuffer;
        return false;
    }

    memcpy(bufferPtr, wavData.SoundBuffer, bufferSize);

    m_lpSoundBuffer->Unlock(bufferPtr, bufferSize, NULL, 0);

    // 一時バッファの解放
    delete[] wavData.SoundBuffer;

    return true;
}

void DirectSound::Release()
{
    if (m_lpSoundBuffer)
    {
        m_lpSoundBuffer->Stop();
        m_lpSoundBuffer->Release();
        m_lpSoundBuffer = nullptr;
    }
}

void DirectSound::Play(bool isLoop)
{
    if (!m_lpSoundBuffer) return;

    // 再生位置を最初に戻してから再生
    ResetPosition();

    DWORD flags = isLoop ? DSBPLAY_LOOPING : 0;
    m_lpSoundBuffer->Play(0, 0, flags);
}

void DirectSound::Stop()
{
    if (m_lpSoundBuffer)
    {
        m_lpSoundBuffer->Stop();
    }
}

void DirectSound::ResetPosition()
{
    if (m_lpSoundBuffer)
    {
        m_lpSoundBuffer->SetCurrentPosition(0);
    }
}

void DirectSound::SetVolume(int volume)
{
    if (!m_lpSoundBuffer) return;

    // DirectSoundの音量は -10000(無音) ～ 0(最大)
    // 引数の 0 ～ 10000 を適切な範囲に変換
    LONG dsVolume = static_cast<LONG>(volume) - 10000;
    if (dsVolume < DSBVOLUME_MIN) dsVolume = DSBVOLUME_MIN;
    if (dsVolume > DSBVOLUME_MAX) dsVolume = DSBVOLUME_MAX;

    m_lpSoundBuffer->SetVolume(dsVolume);
}

bool DirectSound::LoadWavFile(const std::wstring& fileName, WaveData* outWaveData)
{
    HMMIO mmioHandle = mmioOpenW((LPWSTR)fileName.c_str(), NULL, MMIO_READ);
    if (!mmioHandle) return false;

    MMCKINFO riffckInfo;
    riffckInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (MMSYSERR_NOERROR != mmioDescend(mmioHandle, &riffckInfo, NULL, MMIO_FINDRIFF))
    {
        mmioClose(mmioHandle, 0);
        return false;
    }

    MMCKINFO ckInfo;
    ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (MMSYSERR_NOERROR != mmioDescend(mmioHandle, &ckInfo, &riffckInfo, MMIO_FINDCHUNK))
    {
        mmioClose(mmioHandle, 0);
        return false;
    }

    if (mmioRead(mmioHandle, (HPSTR)&outWaveData->WavFormat, sizeof(WAVEFORMATEX)) != sizeof(WAVEFORMATEX))
    {
        mmioClose(mmioHandle, 0);
        return false;
    }

    if (outWaveData->WavFormat.wFormatTag != WAVE_FORMAT_PCM)
    {
        mmioClose(mmioHandle, 0);
        return false;
    }

    mmioAscend(mmioHandle, &ckInfo, 0);

    ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (MMSYSERR_NOERROR != mmioDescend(mmioHandle, &ckInfo, &riffckInfo, MMIO_FINDCHUNK))
    {
        mmioClose(mmioHandle, 0);
        return false;
    }

    outWaveData->Size = ckInfo.cksize;
    outWaveData->SoundBuffer = new char[ckInfo.cksize];
    if (mmioRead(mmioHandle, (HPSTR)outWaveData->SoundBuffer, ckInfo.cksize) != (LONG)ckInfo.cksize)
    {
        delete[] outWaveData->SoundBuffer;
        mmioClose(mmioHandle, 0);
        return false;
    }

    mmioClose(mmioHandle, 0);
    return true;
}
