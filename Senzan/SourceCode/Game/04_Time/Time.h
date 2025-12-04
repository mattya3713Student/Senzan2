#pragma once
#include "System/Singleton/SingletonTemplate.h"
#include <chrono>

/************************
*   タイムクラス.
************************/
class Time final
	: public Singleton<Time>
{
public:
	friend class Singleton<Time>;
public:
	~Time();

	// フレーム間の経過時間を更新.
	static void Update();

	// FPSを維持するための処理.
	static void MaintainFPS();

	// デルタタイムを取得.
	static const float GetDeltaTime();

	static float GetNowTime();
	static float GetWorldTimeScale();
private:
	Time();

	// 生成やコピーを削除.
	Time(const Time& rhs)				= delete;
	Time& operator = (const Time& rhs)	= delete;
private:
	// 前フレームの時間.
	std::chrono::time_point<std::chrono::high_resolution_clock> m_PreviousTime;

	float m_TargetFrameTime;// 目標フレーム時間(秒).
	float m_DeltaTime;		// フレーム間の時間差.

	float m_WorldTimeScale;	// 時間の尺度(通常1f, 2fで二倍の尺度で動く).
};
