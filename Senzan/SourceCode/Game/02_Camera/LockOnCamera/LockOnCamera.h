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

    // 設定の保存/読み込み
    void SaveSettings() const;
    void LoadSettings();
    std::filesystem::path GetSettingsFileName() const { return std::filesystem::path("LockOnCamera.json"); }

private:
    std::reference_wrapper<const Player> m_rPlayer;
    std::reference_wrapper<const Boss>   m_rTarget;

    float m_FollowSpeed;  // 追従速度.
    float m_HeightOffset; // 高さオフセット (ワールド単位).
    float m_LookOffset;   // 注視点の高さオフセット (ワールド単位).
    float m_LookLerp;     // 注視点補間 (0.0=プレイヤー, 1.0=ボス).
};
