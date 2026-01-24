#pragma once
#include "../Dodge.h"    
#include "Resource/Effect/EffectResource.h"

class Player;

/**************************************************
*	プレイヤーのジャスト回避のステート(派生).
*	担当:淵脇 未来.
**/

namespace PlayerState {
class JustDodge final : public Dodge
{
public:
    JustDodge(Player* owner);
    ~JustDodge();

    // IDの取得.
    constexpr PlayerState::eID GetStateID() const override;

    void Enter() override;
    void Update() override;
    void LateUpdate() override;
    void Draw() override;
    void Exit() override;

protected:
    ::Effekseer::Handle m_UIEffectHandle = -1; // UI用エフェクトハンドル.
};
}
