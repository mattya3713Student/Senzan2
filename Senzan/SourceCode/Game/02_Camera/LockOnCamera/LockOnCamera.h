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

private:
    std::reference_wrapper<const Player> m_rPlayer;
    std::reference_wrapper<const Boss>   m_rTarget;

    float m_FollowSpeed;  // 補間ファクタ (0.0f .. 1.0f).
    float m_HeightOffset; // 高さオフセット (ワールド単位).
    float m_LookOffset;   // 注視点の高さオフセット (ワールド単位).
};
