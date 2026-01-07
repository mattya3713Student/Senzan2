#pragma once

#include "../00_Scene/00_Base/SceneBase.h"
#include <memory>

class Player;
class Boss;
class Ground;
class UIGameMain;
class CapsuleCollider;
class DirectionLight;
class PlayerThirdPersonCamera; // forward declaration

class AnimationTuningScene
    : public SceneBase
{
public:
    AnimationTuningScene();
    virtual ~AnimationTuningScene();

    void Initialize() override;
    void Create() override;
    void Update() override;
    void LateUpdate() override;
    void Draw() override;
    HRESULT LoadData();

private:
    std::shared_ptr<PlayerThirdPersonCamera> m_pCamera;
    std::shared_ptr<DirectionLight> m_pLight;

    std::unique_ptr<Player> m_upPlayer;
    std::unique_ptr<Boss> m_upBoss;
    std::unique_ptr<Ground> m_upGround;
    std::shared_ptr<UIGameMain> m_upUI;
};
