#pragma once
#include "../CameraBase.h"

/**********************************************************************************
* @author    : 淵脇 未来.
* @date      : 2026/01/02.
* @brief     : ロックオンカメラクラス.
*              プレイヤーとターゲット（ボス）間の注視制御を提供します.
**********************************************************************************/
class Player;
class Boss;

class LockOnCamera : public CameraBase
{
public:
    // カメラモード.
    enum class eCameraMode
    {
        Normal,     // 通常モード.
        Parry,      // パリィ演出モード（下から潜り込む）.
    };

public:
    LockOnCamera(const Player& player, const Boss& target);
    virtual ~LockOnCamera() = default;

    virtual void Update() override;

    // フォロースピードを設定.
    void SetFollowSpeed(float speed) { m_FollowSpeed = speed; }
    // カメラとプレイヤー間の距離を設定.
    void SetDistance(float dist) { m_Distance = dist; }
    // カメラの高さオフセットを設定.
    void SetHeightOffset(float height) { m_HeightOffset = height; }
    // 注視点の高さオフセットを設定.
    void SetLookOffset(float offset) { m_LookOffset = offset; }
    // 注視点補間値を設定.
    void SetLookLerp(float lerp) { m_LookLerp = lerp; }

    // パリィカメラ演出開始.
    void StartParryCamera();
    // パリィカメラ演出終了（通常モードへ復帰）.
    void EndParryCamera();
    // パリィカメラ演出中か.
    bool IsParryCameraActive() const { return m_CameraMode == eCameraMode::Parry; }

    // 設定の保存/読み込み
    void SaveSettings() const;
    void LoadSettings();
    std::filesystem::path GetSettingsFileName() const { return std::filesystem::path("LockOnCamera.json"); }

private:
    // 通常カメラ更新.
    void UpdateNormalCamera(float dt);
    // パリィカメラ更新.
    void UpdateParryCamera(float dt);

private:
    std::reference_wrapper<const Player> m_rPlayer;
    std::reference_wrapper<const Boss>   m_rTarget;

    float m_FollowSpeed;  // 追従速度.
    float m_HeightOffset; // 高さオフセット (ワールド単位).
    float m_LookOffset;   // 注視点の高さオフセット (ワールド単位).
    float m_LookLerp;     // 注視点補間 (0.0=プレイヤー, 1.0=ボス).

    // パリィカメラ演出用.
    eCameraMode m_CameraMode = eCameraMode::Normal;
    float m_ParryTime = 0.0f;           // パリィ演出経過時間.
    float m_ParryDuration = 0.8f;       // パリィ演出時間.
    float m_ParryHeightOffset = 1.0f;   // パリィ時の低い高さ.
    float m_ParryLookOffset = 6.0f;     // パリィ時の注視点高さ（見上げる）.
    float m_ParryDistance = 8.0f;       // パリィ時の距離（近づく）.
};
