#pragma once
#include "../CameraBase.h"

class Player;
class Boss;

class LockOnCamera : public CameraBase
{
public:
    LockOnCamera(const Player& player, const Boss& target);
    virtual ~LockOnCamera() = default;

    virtual void Update() override;

    // カメラのパラメータ調整用.
    void SetFollowSpeed(float speed) { m_FollowSpeed = speed; }
    void SetDistance(float dist) { m_Distance = dist; }
    void SetHeightOffset(float height) { m_HeightOffset = height; }

private:
    std::reference_wrapper<const Player>m_rPlayer;
    std::reference_wrapper<const Boss>  m_rTarget;

    float m_FollowSpeed;  // 追従の滑らかさ (0.0f ～ 1.0f)
    float m_HeightOffset; // プレイヤーからどれくらい浮かせるか
    float m_LookOffset;   // ボスのどこを見るか（足元ではなく胴体など）
};