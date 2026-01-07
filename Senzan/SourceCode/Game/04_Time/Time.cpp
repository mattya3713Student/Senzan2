#include "Time.h"
#include <thread>
#include <chrono>

constexpr float TAEGET_FPS = 60.0f;//目標フレーム.

Time::Time()
    : m_PreviousTime        ()
    , m_TargetFrameTime     ()
    , m_DeltaTime           ()
    , m_WorldTimeScale      ( 1.0f )
    , m_OriginalTimeScale   ( 1.0f )
    , m_TimeScaleRestoreTime( 0.0f )
{
    m_TargetFrameTime   = 1.0f / TAEGET_FPS;
    m_PreviousTime      = std::chrono::steady_clock::now();
}

Time::~Time()
{
}

// フレーム間の経過時間を更新.
void Time::Update()
{
    // デルタタイム計算.
    auto current_time = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = current_time - m_PreviousTime;
    m_DeltaTime = elapsed.count();

    // スパイク対策：PCのガクつきで1秒とか飛んだ時に、キャラがワープしないように制限
    if (m_DeltaTime > 0.1f) m_DeltaTime = m_TargetFrameTime;

    m_PreviousTime = current_time;

    // 時間スケールの復帰.
    if (m_TimeScaleRestoreTime > 0.0f)
    {
        float nowTime = GetNowTime();

        if (nowTime >= m_TimeScaleRestoreTime)
        {
            m_WorldTimeScale = m_OriginalTimeScale;
            m_TimeScaleRestoreTime = 0.0f;
        }
    }

    // Note: do NOT multiply m_DeltaTime by world scale here.
    // Keep m_DeltaTime as raw elapsed seconds. Apply world time scale when requested by callers.
}

// アプリが復帰したとき時間基準をリセットしてでかいDeltaTimeになるのを防ぐ.
void Time::ResetOnResume()
{
    m_PreviousTime = std::chrono::steady_clock::now();
    m_DeltaTime = m_TargetFrameTime;
}

// FPSを維持するために待機が必要かチェックする.
bool Time::IsReadyForNextFrame()
{
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = now - m_PreviousTime;

    // 目標とするフレーム時間に達していれば true.
    return elapsed.count() >= m_TargetFrameTime;
}

// FPSを維持するための処理.
void Time::MaintainFPS()
{
    // フレームの残り時間を計算し sleep.
    auto frame_end = std::chrono::steady_clock::now();
    std::chrono::duration<float> frameElapsed = frame_end - m_PreviousTime; // フレーム開始からの経過時間.

    float remaining = m_TargetFrameTime - frameElapsed.count();
    if (remaining <= 0.0f) return;

    // ミリ秒単位で Sleep して短時間はスピンで待機する.
    auto ms = static_cast<int>(remaining * 1000.0f);
    if (ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

    // スピンで残り時間を待つ.
    while (true) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> d = now - m_PreviousTime;
        if (d.count() >= m_TargetFrameTime) break;
        // 省CPUの為、軽くyield.
        std::this_thread::yield();
    }
}

// デルタタイムを取得.
const float Time::GetDeltaTime() const
{
    // Return raw delta time (unscaled). Callers can multiply by world scale if needed.
    return m_DeltaTime;
}

// アプリ起動からの時間を取得.
float Time::GetNowTime()
{
    static auto appStart = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = now - appStart;
    return elapsed.count();
}

// ワールドの時間尺度を取得.
float Time::GetWorldTimeScale() const
{
    return m_WorldTimeScale;
}

// ワールドの時間の流れを永続的に変更する.
void Time::SetWorldTimeScale(float NewTimeScale)
{
    m_WorldTimeScale = NewTimeScale;
    m_TimeScaleRestoreTime = 0.0f;
}

// ワールドの時間の流れを一時的に変更する.
void Time::SetWorldTimeScale(float NewTimeScale, float DurationSeconds)
{
    // 一時的な変更の場合は現在の値を保存してから適用.
    m_OriginalTimeScale = m_WorldTimeScale;
    m_WorldTimeScale = NewTimeScale;

    if (DurationSeconds > 0.0f)
    {
        m_TimeScaleRestoreTime = GetNowTime() + DurationSeconds;
    }
    else
    {
        m_TimeScaleRestoreTime = 0.0f;
    }
}
