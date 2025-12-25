#pragma once

#include <functional>
#include <memory>

#include "../BossStateBase.h"
#include "../BossStateID.h"

class Boss;

namespace BossState
{
    // 今後増えるステートをここに追記していく
    class BossIdolState;
    // class BossMoveState; 

    /**********************************************
    *	ボスの状態の最上位(Root).
    * 各ステートの所有権と使用ステート切り替えをする.
    **/
    class BossRoot final : public BossStateBase
    {
    public:
        BossRoot(Boss* pOwner);
        ~BossRoot();

        // ステートIDの取得.
        BossState::enID GetStateID() const override;

        void Enter() override;
        void Update() override;
        void LateUpdate() override;
        void Draw() override;
        void Exit() override;

        // ステートの変更.
        void ChangeState(BossState::enID id);

#pragma region GetStateRef
        // 各ステートの取得関数
        std::reference_wrapper<::BossStateBase> GetIdolStateRef();
        // std::reference_wrapper<::BossStateBase> GetMoveStateRef();
#pragma endregion

    private:
        // 各ステートの所有権
        std::unique_ptr<BossIdolState> m_pIdol;
        // std::unique_ptr<BossMoveState> m_pMove;

        // 現在の状態を保持
        std::reference_wrapper<::BossStateBase> m_CurrentState;
    };
}