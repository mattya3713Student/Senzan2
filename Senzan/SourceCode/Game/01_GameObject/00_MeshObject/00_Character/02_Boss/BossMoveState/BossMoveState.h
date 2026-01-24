#pragma once

//基底クラス.
#include "System/Utility/StateMachine/StateBase.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "Game\03_Collision\00_Core\01_Capsule\CapsuleCollider.h"

//前方宣言.
class Time;

//---------------------
//ボスの攻撃の前方宣言.
//---------------------
class BossSlashState;
class BossChargeState;
class BossChargeSlashState;
class BossLaserState;
class BossShoutState;
class BossSlashState;
class BossJumpOnlState;
class BossStompState;
class BossThrowingState;

#include <random>
#include <array>

/******************************************************************************
*	ボスの動作(左右移動・プレイヤーを囲むように半円を描く).
**/

class BossMoveState final
	: public StateBase<Boss>
{
public:
	enum class DistanceAttack : byte
	{
		Melee,	//近距離.
		Mid,	//中距離.
		Long,	//遠距離.
		None,	//何もしない.
	};

	enum class MovePhase : byte
	{
		Start,
		Run,
		Stop,
		Strafe,
	};

    // Attack identifiers
    enum AttackId { Slash = 0, Stomp = 1, Charge = 2, Shout = 3, Throwing = 4, Count = 5 };

    MovePhase m_Phase = MovePhase::Start;
public:
	BossMoveState(Boss* owner);
	~BossMoveState();

	//最初に入る.
	void Enter() override;
	//動作.
	void Update() override;
    //かかなくていい.
    void LateUpdate() override;
	//描画.
	void Draw() override;
	//終わるときに一回だけ入る.
	void Exit() override;

public:
	//初期角度を設定する関数.
	void SetInitialAngle(float angle);
    // 設定の読み書き
    void LoadSettings();
    void SaveSettings() const;
private:
	//現在のボスの回転度を確認する.
	float m_RotationAngle;
	double m_RotationSpeed;
	float m_rotationDirection;	//1.0f:順方向, -1.0:逆方向.

	int							m_AnimNo;	//アニメーション番号.
	double						m_AnimSpeed = 0.0001;// アニメーション速度.
	double						m_AnimTimer;// アニメ―ション経過時間.
	DirectX::XMFLOAT3			m_BonePos;	// ボーン座標.

	float m_BaseAngle;

	DirectX::XMFLOAT3 m_InitBossPos;

	float m_Timer = 0.0f;
	float m_SecondTimer = 120.0f;

	std::unique_ptr<BossJumpOnlState>		m_pAttack;
	std::unique_ptr<BossSlashState>			m_pSlash;
	std::unique_ptr<BossStompState>			m_pStomp;
	std::unique_ptr<BossThrowingState>		m_pThrowing;
	std::unique_ptr<BossShoutState>			m_pShout;

	// === デバッグ用距離設定 ===
	static inline float s_NearRange = 15.0f;      // 近距離の閾値
	static inline float s_MidRange = 35.0f;       // 中距離の閾値
	static inline float s_AttackDelay = 1.0f;     // 攻撃開始までの遅延

	// === 攻撃有効/無効フラグ、重み、クールダウン（配列で管理） ===
	static inline float s_RepeatPenalty = 0.25f;

	// Per-attack settings (indexed by AttackId::Count)
	static inline std::array<bool, Count> s_Enable = { true, true, true, true, true };
	static inline std::array<float, Count> s_Weight = { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f }; // percentage-like (0..100)
	static inline std::array<float, Count> s_CooldownDefault = { 2.0f, 2.5f, 3.0f, 4.0f, 2.0f };
    // 日本語ラベルにして IMGUI 表示でそのまま使えるようにする
    // 表示用日本語ラベル
    static inline const char* s_AttackNames[Count] = { "斬り", "飛びかかり", "溜め", "叫び", "投擲" };
    // 内部保存用ID（英語）。JSONキーなどで使う。
    static inline const char* s_AttackIds[Count] = { "Slash", "Stomp", "Charge", "Shout", "Throwing" };

	// === デバッグ強制攻撃選択 ===
	static inline int s_ForceAttackIndex = -1;    // -1: ランダム, 0-4: 強制選択

    // runtime cooldown/last-attack tracking
    std::array<float, Count> m_CooldownRemaining{};
    int m_LastAttackId = -1; // last chosen attack id
};
