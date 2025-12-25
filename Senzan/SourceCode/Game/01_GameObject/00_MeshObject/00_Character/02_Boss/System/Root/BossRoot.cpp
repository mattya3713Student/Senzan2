#include "BossRoot.h"
#include "../../Boss.h"
#include "../../BossIdolState/BossIdolState.h"

namespace BossState
{
    BossRoot::BossRoot(Boss* pOwner)
        : BossStateBase(pOwner)
        , m_pIdol(nullptr) // Playerと同様に最初はnullptr
        // まだステートがないので、一旦自分自身(this)を指しておく（安全策）
        , m_CurrentState(*this)
    {
    }

    BossRoot::~BossRoot()
    {
    }

    BossState::enID BossRoot::GetStateID() const
    {
        // 自身を指している時は None、ステート決定後はそのIDを返す
        if (&m_CurrentState.get() == this) return BossState::enID::None;
        return m_CurrentState.get().GetStateID();
    }

    void BossRoot::Enter()
    {
        // Playerと同様に Enter ですべて生成
        m_pIdol = std::make_unique<BossIdolState>(m_pOwner);
        // m_pMove = std::make_unique<BossMoveState>(m_pOwner);

        // 初期ステートをセット
        m_CurrentState = std::ref(static_cast<::BossStateBase&>(*m_pIdol));
        m_CurrentState.get().Enter();
    }

    void BossRoot::Update()
    {
        // 自身を指していない時のみ更新（無限ループ防止）
        if (&m_CurrentState.get() != this) {
            m_CurrentState.get().Update();
        }
    }

    void BossRoot::LateUpdate()
    {
        if (&m_CurrentState.get() != this) {
            m_CurrentState.get().LateUpdate();
        }
    }

    void BossRoot::Draw()
    {
        if (&m_CurrentState.get() != this) {
            m_CurrentState.get().Draw();
        }
    }

    void BossRoot::Exit()
    {
        if (&m_CurrentState.get() != this) {
            m_CurrentState.get().Exit();
        }
    }

    void BossRoot::ChangeState(BossState::enID id)
    {
        // ここに Player と同様の切り替えロジックを実装します
        // Bossクラスに GetStateReference(id) を作る必要があります
        try {
            std::reference_wrapper<::BossStateBase> newStateRef = GetBoss()->GetStateReference(id);

            if (&m_CurrentState.get() != &newStateRef.get())
            {
                m_CurrentState.get().Exit();
                m_CurrentState = newStateRef;
                m_CurrentState.get().Enter();
            }
        }
        catch (...) {
            // エラー処理
        }
    }

#pragma region GetStateRef
    std::reference_wrapper<::BossStateBase> BossRoot::GetIdolStateRef()
    {
        return std::ref(static_cast<::BossStateBase&>(*m_pIdol));
    }
#pragma endregion

} // namespace BossState