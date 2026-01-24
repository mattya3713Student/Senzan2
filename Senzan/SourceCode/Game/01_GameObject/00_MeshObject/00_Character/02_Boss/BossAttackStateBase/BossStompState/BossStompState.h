#pragma once

#include "..//BossAttackStateBase.h"	//基底クラス.

class BossIdolState;
class Boss;

class BossStompState final
	: public BossAttackStateBase
{
public:
	enum class enAttack : byte
	{
		None,		//何もしない.
		Stomp,		//踏みつけ攻撃をする.
		CoolTime,	//クールタイム.
		Trans		//Idolに状態遷移.
	};

public:
	BossStompState(Boss* owner);
	~BossStompState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

    // ImGui と設定の読み書き
    void DrawImGui() override;
    void LoadSettings() override;
    void SaveSettings() const override;
    std::filesystem::path GetSettingsFileName() const override { return std::filesystem::path("BossStompState.json"); }
private:
    void BossAttack();
    // ローカルで当たり判定ウィンドウを更新する（基底を使わずに同様の挙動を再現）
    void UpdateLocalColliderWindows(float dt);
private:
	enAttack m_List;

	std::shared_ptr<BossIdolState> m_pIdol;
	//====================================================
	// 踏みつけに必要になるメンバ変数を書く.
	//====================================================
	DirectX::XMFLOAT3 m_Velocity;

	//ジャンプの初速度.
	float m_JumpPower;
	//重力加速度.
	float m_Gravity;
	//ジャンプ中のフラグ.
	bool m_JumpFrag;
	// 前フレームの Y 値を保持してスムーズに移動させる
	// float m_LastY; // 一時無効化: Y 制御オフ
	//着地フラグ.
	//この時に時間を取得して3秒後にIdolに遷移等を書く.
	bool m_GroundedFrag;
	//===================================================
	// タイマー変数.
	//===================================================
	//タイマーの初期化変数.
	float m_Timer;
	//遷移させるタイミングの変数.
	float TransitionTimer;

	//上がる速度のスピード設定用.
	float m_UpSpeed;

	// State timers / flags
	float m_WaitSeconds = 0.4f;    // seconds before slowing animation
	float m_SlowAnimSpeed = 0.5f;  // slowed animation speed value
	bool  m_AnimSlowed = false;    // whether animation was slowed
	bool  m_IsMoving = false;      // whether boss is currently moving (stomp phase)
	float m_StateTimer = 0.0f;     // generic timer used for state timing
    // slow duration control: how long animation stays slowed after delay
    float m_SlowDuration = 1.5f;  // seconds to keep animation slowed
    float m_SlowElapsed = 0.0f;   // elapsed time since slow started

	// movement easing params
	float m_MoveDuration = 0.8f;   // duration of the movement/easing (seconds)
	float m_MoveTimer = 0.0f;      // elapsed time since movement started
	float m_Distance = 0.0f;       // total horizontal distance to move
	DirectX::XMFLOAT3 m_MoveVec;   // normalized horizontal direction vector

	//====================================================
	// 飛びかかり（前方移動）用メンバ変数.
	//====================================================
	float m_ForwardSpeed;           // 前方移動速度.
	DirectX::XMFLOAT3 m_TargetPos;  // 目標位置（プレイヤー位置）.
	DirectX::XMFLOAT3 m_StartPos_Stomp;   // 開始位置.
    bool m_HasLanded;               // 着地済みフラグ（ダメージ1回のみ）
    // デバッグ用: 当たり判定の可視化フラグ
    bool m_ShowStompDebug = false;
    // 派生で管理する当たり判定ウィンドウ（基底を使わない）
    std::vector<ColliderWindow> m_LocalColliderWindows;
};
