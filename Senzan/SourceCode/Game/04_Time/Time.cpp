#include "Time.h"
#include <thread>

constexpr float TAEGET_FPS = 60.0f;//目標フレーム.

Time::Time()
    : m_PreviousTime    ()
    , m_TargetFrameTime ()
    , m_DeltaTime       ()
{
    m_TargetFrameTime   = 1.0f / TAEGET_FPS; // 目標フレームを計算.
    m_PreviousTime      = std::chrono::high_resolution_clock::now();//初期を取得.
}

Time::~Time()
{
}

// フレーム間の経過時間を更新.
void Time::Update()
{
    // インスタンスを取得.
    Time& pI = GetInstance();

    // 現在の時間を取得.
    auto currentTime = std::chrono::high_resolution_clock::now();

    // 前回からの経過時間を計算.
    std::chrono::duration<float> elapsed = currentTime - pI.m_PreviousTime;

    // 経過時間を秒単位で保持.
    pI.m_DeltaTime = elapsed.count();

    // 次のフレームのために更新.
    pI.m_PreviousTime = currentTime;
}

// FPSを維持するための処理.
void Time::MaintainFPS()
{
    Time& pI = GetInstance();

    if (pI.m_DeltaTime < pI.m_TargetFrameTime)
    {
        auto sleepTime = pI.m_TargetFrameTime - pI.m_DeltaTime;
        std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
    }
}

// デルタタイムを取得.
const float Time::GetDeltaTime()
{
    return GetInstance().m_DeltaTime;
}

float Time::GetNowTime()
{
    using namespace std::chrono;

    // 1. 現在の time_point を取得
    auto now_timepoint = high_resolution_clock::now();

    // 2. time_point を duration (期間) に変換
    //    (エポックから現在までの時間)
    auto duration_since_epoch = now_timepoint.time_since_epoch();

    // 3. duration を float型の秒数に変換
    // duration_cast を使用して目的の精度（例: 秒）に変換し、浮動小数点型にキャスト

    // high_resolution_clock::period を float に変換した秒数で計算
    using float_seconds = duration<float>;

    // duration_since_epoch を float_seconds 単位に変換
    float time_in_seconds = duration_cast<float_seconds>(duration_since_epoch).count();

    return time_in_seconds;
}
