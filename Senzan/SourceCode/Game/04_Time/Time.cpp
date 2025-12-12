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
    m_PreviousTime      = std::chrono::high_resolution_clock::now();
}

Time::~Time()
{
}

// フレーム間の経過時間を更新.
void Time::Update()
{
    // デルタタイム計算.
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = currentTime - m_PreviousTime;
    m_DeltaTime = elapsed.count();
    m_PreviousTime = currentTime;

    // 時間スケールの復帰.
    if (m_TimeScaleRestoreTime > 0.0f)
    {
        float nowTime = GetNowTime();

        if (nowTime >= m_TimeScaleRestoreTime)
        {
            m_WorldTimeScale = m_OriginalTimeScale;
            m_TimeScaleRestoreTime = 0.0f;
        }

        Log::GetInstance().Info("あ", nowTime - m_TimeScaleRestoreTime);
    }

    // 時間スケールを適用.
    m_DeltaTime *= m_WorldTimeScale; 
}

// FPSを維持するための処理.
void Time::MaintainFPS()
{
    if (m_DeltaTime < m_TargetFrameTime) {
        m_DeltaTime = m_TargetFrameTime;
        std::this_thread::sleep_for(
            std::chrono::duration<float>(m_TargetFrameTime - m_DeltaTime));
    }
}

// デルタタイムを取得.
const float Time::GetDeltaTime() const
{
    return m_DeltaTime;
}

// 現在時刻を取得.
float Time::GetNowTime()
{
    using namespace std::chrono;

    // 現在の time_point を取得.
    auto now_timepoint = high_resolution_clock::now();

    // time_point を duration (期間) に変換.
    auto duration_since_epoch = now_timepoint.time_since_epoch();

    // duration を float型の秒数に変換.
    using float_seconds = duration<float>;
    float time_in_seconds = duration_cast<float_seconds>(duration_since_epoch).count();

    return time_in_seconds;
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
    if (GetNowTime() >= m_TimeScaleRestoreTime)
    {
        // 元の時間スケールを保存.
        m_OriginalTimeScale = m_WorldTimeScale;
    }

    // 新しい時間スケールを適用.
    m_WorldTimeScale = NewTimeScale;

    // 復帰時刻を設定.
    if (DurationSeconds > 0.0f)
    {
        m_TimeScaleRestoreTime = GetNowTime() + DurationSeconds;
    }
    else
    {
        // DurationSecondsが0以下の場合は永続的な設定として扱い、タイマーを解除.
        m_TimeScaleRestoreTime = 0.0f;
    }
}