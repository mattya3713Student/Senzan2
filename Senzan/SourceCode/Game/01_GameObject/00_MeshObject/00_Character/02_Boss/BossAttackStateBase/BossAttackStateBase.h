#pragma once

#include "System//Utility//StateMachine//StateBase.h"
#include "Game\03_Collision\00_Core\01_Capsule\CapsuleCollider.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "System/Utility/FileManager/FileManager.h"

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

/*****************************************************************
*    ボスの攻撃ベースクラス(基底クラス).
**/

//前方宣言.
class Boss;
class Time;
class ColliderBase;
class GameObject;
class Animator;

// 攻撃中の移動（踏み込み）を制御する
struct MovementWindow
{
    float Start = 0.0f;    // 開始秒
    float Duration = 0.2f; // 継続時間
    float Speed = 10.0f;   // 移動速度
    bool IsAct = false;    // 内部フラグ（移動中か）

    void Reset() { IsAct = false; }
};

// 当たり判定の発生時間を制御する
struct ColliderWindow
{
    std::string BoneName;  // ボーン名
    float Start = 0.0f;    // 開始秒
    float Duration = 0.1f; // 持続秒
    bool IsAct = false;    // 内部フラグ（判定開始済みか）
    bool IsEnd = false;    // 内部フラグ（判定終了済みか）

    void Reset() { IsAct = false; IsEnd = false; }
};

class BossAttackStateBase
    : public StateBase<Boss>

{
public:
    BossAttackStateBase(Boss* owner);
    virtual ~BossAttackStateBase() = default;

    //最初に入る.
    virtual void Enter() override;
    //動作.
    virtual void Update() override;
    //かかなくていい.
    virtual void LateUpdate() override {};
    //描画.
    virtual void Draw() override {};
    //終わるときに一回だけ入る.
    virtual void Exit() override; 

    // 攻撃ステート調整用UI（必要なステートで呼ぶ）
    virtual void DrawImGui();

    // JSON 設定の読み書き
    virtual void LoadSettings();
    virtual void SaveSettings() const;
    // シリアライズ用（派生が追加フィールドを返すためにオーバーライド可）
    virtual nlohmann::json SerializeSettings() const;
    // 各ステートで上書きしてファイル名を変更する
    virtual std::filesystem::path GetSettingsFileName() const { return std::filesystem::path("BossAttackState_Base.json"); }

    //攻撃を実行させる関数.
    virtual void BossAttack() {};

    // ステート遷移を許可するか (デバッグ停止時は拒否)
    bool CanChangeState() const override { return !m_IsDebugStop; }

protected:
    // --- 共通タイムベース制御 ---
    float m_CurrentTime = 0.0f;    // ステートに入ってからの経過時間（秒）
    float m_AnimSpeed = 1.0f;      // 実行時の現在アニメ速度（倍率）

    // --- ステート設定値 ---
    float m_EndTime = 2.0f;        // ステート全体の時間（秒）
    float m_ChargeTime = 0.0f;     // 溜め終了時間（秒）
    float m_AttackTime = 0.0f;     // 攻撃終了時間（秒）

    // 当たり判定・移動ウィンドウ（可変長）
    std::vector<ColliderWindow>  m_ColliderWindows;  // 当たり判定設定
    std::vector<MovementWindow>  m_MovementWindows;  // 移動（踏み込み）設定

    void UpdateBaseLogic(float dt); // 共通更新ロジック

protected:
    //メンバ変数を作成.
    //アニメーションに必要なメンバ変数
    DirectX::XMFLOAT3            m_BonePos;      // ボーン座標
    LPD3DXANIMATIONCONTROLLER   AnimCtrl;
    std::shared_ptr<Transform> m_pTransform;

    // --- 共通攻撃パラメータ（設定値） ---
    float m_AttackAmount = 0.0f;   // 攻撃力（ダメージ）
    float m_AttackRange = 0.0f;    // 攻撃レンジ
    float m_ColliderWidth = 0.0f;  // 当たり判定の幅
    float m_ColliderHeight = 0.0f; // 当たり判定の高さ

    // フェーズ用アニメ速度（倍率）
    float m_AnimSpeedCharge = 1.0f; // 溜め
    float m_AnimSpeedAttack = 1.0f; // 攻撃中
    float m_AnimSpeedExit = 1.0f;   // 余波



    bool  m_IsDebugStop = false;         // デバッグ用停止フラグ

    std::shared_ptr<ColliderBase> m_pHitCollider;
    std::vector<GameObject*>      m_HitTargets;

    DirectX::XMFLOAT3 m_TargetDir{ 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_MotionVelocity{ 0.0f, 0.0f, 0.0f };

    DirectX::XMFLOAT3 m_StartPos{ 0.0f, 0.0f, 0.0f }; // 攻撃開始位置（共通）
};
