#include "Character.h"
#include "Game/03_Collision/00_Core/ColliderBase.h"
#include "Game/03_Collision/00_Core/Ex_CompositeCollider/CompositeCollider.h"
#include "Game/04_Time/Time.h"
#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"
#include <algorithm>

// 押し戻し判定.
constexpr eCollisionGroup PRESS_GROUP = eCollisionGroup::Press;

Character::Character()
    : MeshObject()
    , m_upColliders()
    , m_MaxHP(100.f)
    , m_HP(100.f)
{
    m_upColliders = std::make_unique<CompositeCollider>();
}

Character::~Character()
{
}

void Character::Update()
{
    m_upColliders->SetDebugInfo();
}

void Character::LateUpdate()
{
    // 毎フレームスケジュールされた攻撃コライダーの状態を更新.
    UpdateScheduledColliders();

    HandleCollisionResponse();
}

void Character::Draw()
{
    MeshObject::Draw();
}

// 衝突応答処理.
void Character::HandleCollisionResponse()
{
    if (!m_upColliders) return;

    // このオブジェクトの当たり判定を取得.
    const  std::vector<std::unique_ptr<ColliderBase>>& internal_colliders = m_upColliders->GetInternalColliders();

    // 当たり判定を走査.
    for (const std::unique_ptr<ColliderBase>& collider_ptr : internal_colliders)
    {
        const ColliderBase* current_collider = collider_ptr.get();
        if (!current_collider) { continue; }

        // 自分が Press属性 でなければ continue.
        if ((current_collider->GetMyMask() & PRESS_GROUP) == eCollisionGroup::None) {
            continue;
        }

        // 衝突イベントリストを取得.
        const std::vector<CollisionInfo>& events = current_collider->GetCollisionEvents();

        // イベントを走査.
        for (const CollisionInfo& info : events)
        {
            if (!info.IsHit) continue;
            const ColliderBase* otherCollider = info.ColliderB;
            if (!otherCollider) { continue; }

            // 相手のグループが Press であるか (このPressグループとの衝突のみを処理する).
            if ((otherCollider->GetMyMask() & PRESS_GROUP) == eCollisionGroup::None) { continue; }

            if (info.PenetrationDepth > 0.0f)
            {
                DirectX::XMVECTOR v_normal = DirectX::XMLoadFloat3(&info.Normal);
                DirectX::XMVECTOR v_correction = DirectX::XMVectorScale(v_normal, -info.PenetrationDepth);
                // このゲームにy座標の概念はないのでyは切り捨て.
                v_correction = DirectX::XMVectorSetY(v_correction, 0.0f);
                DirectX::XMFLOAT3 correction = {};
                DirectX::XMStoreFloat3(&correction, v_correction);

                AddPosition(correction);
            }
        }
    }
}

void Character::ApplyDamage(float damageAmount)
{
    // すでに死亡している場合は何もしない
    if (0.f >= m_HP) return;

    // 無敵時間中ならダメージを受け付けない (必要であれば)
    // if (m_InvincibleTimer > 0.0f) return;

    // HPを減らす
    m_HP -= damageAmount;

    // HPが0以下になったら死亡処理
    if (m_HP <= 0.0f)
    {
        m_HP = 0.0f;

        // 死亡ステートへ（まだ作っていなければログやフラグのみでもOK）
        // ChangeState(PlayerState::eID::Die); 
        Log::GetInstance().Info("Player", "Player is Dead.");
    }
    else
    {
        // 被ダメージ時の共通処理（SE再生や一瞬だけ赤く光らせるフラグなど）
        // PlaySound(SE_PlayerDamage);

        // 無敵時間の設定 (例: 1.0秒間無敵)
        // m_InvincibleTimer = 1.0f; 
    }
}

// スケジュールされた攻撃コライダーを登録する.
void Character::ScheduleAttackCollider(AttackTypeId type, size_t index, float delay, float duration)
{
    // すでに同じスケジュールがある場合は上書き.
    for (auto &s : m_ScheduledColliders)
    {
        if (s.Type == type && s.Index == index)
        {
            s.Delay = delay;
            s.Duration = duration;
            s.Elapsed = 0.0f;
            s.Active = false;
            return;
        }
    }

    ScheduledCollider sc;
    sc.Type = type;
    sc.Index = index;
    sc.Delay = delay;
    sc.Duration = duration;
    sc.Elapsed = 0.0f;
    sc.Active = false;
    m_ScheduledColliders.push_back(sc);
}

// コライダーをremoveする.
void Character::CancelScheduledCollider(AttackTypeId type, size_t index)
{
    m_ScheduledColliders.erase(
        std::remove_if(
            m_ScheduledColliders.begin(),
            m_ScheduledColliders.end(),
            [&](const ScheduledCollider& s) { return s.Type == type && s.Index == index; }
        ),
        m_ScheduledColliders.end()
    );
}

// 指定攻撃タイプの全コライダーを有効/無効化する.
void Character::SetAttackCollidersActive(AttackTypeId type, bool active)
{
    auto it = m_AttackColliders.find(type);
    if (it == m_AttackColliders.end()) return;
    for (ColliderBase* col : it->second)
    {
        if (col) col->SetActive(active);
    }
}

// 指定攻撃タイプの単一コライダーを有効/無効化する.
void Character::SetAttackColliderActive(AttackTypeId type, size_t index, bool active)
{
    auto it = m_AttackColliders.find(type);
    if (it == m_AttackColliders.end()) return;
    if (index >= it->second.size()) return;
    ColliderBase* col = it->second[index];
    if (col) col->SetActive(active);
}

// スケジュールされたコライダーの状態を更新する.
void Character::UpdateScheduledColliders()
{
    if (m_ScheduledColliders.empty()) return;

    const float dt = static_cast<float>(Time::GetInstance().GetDeltaTime());
    bool needCleanup = false;

    for (auto &s : m_ScheduledColliders)
    {
        s.Elapsed += dt;

        // 有効化タイミング.
        if (!s.Active && s.Elapsed >= s.Delay)
        {
            SetAttackColliderActive(s.Type, s.Index, true);
            s.Active = true;
        }

        // 無効化タイミング.
        if (s.Active && s.Elapsed >= (s.Delay + s.Duration))
        {
            SetAttackColliderActive(s.Type, s.Index, false);
            s.Active = false;
            needCleanup = true; 
        }
    }

    // 不要になったスケジュールを削除.
    if (needCleanup)
    {
        m_ScheduledColliders.erase(
            std::remove_if(
                m_ScheduledColliders.begin(),
                m_ScheduledColliders.end(),
                [](const ScheduledCollider& sc) { return (!sc.Active && sc.Elapsed >= (sc.Delay + sc.Duration)); }
            ),
            m_ScheduledColliders.end()
        );
    }
}

void Character::CreateAttackCollidersFromDefs(const std::unordered_map<AttackTypeId, std::vector<ColliderSpec>>& defs)
{
    for (const auto& kv : defs)
    {
        AttackTypeId type = kv.first;
        const auto& specs = kv.second;
        std::vector<ColliderBase*> pointers;

        for (const auto& spec : specs)
        {
            auto col = std::make_unique<CapsuleCollider>(m_spTransform);
            ColliderBase* raw = col.get();

            col->SetRadius(spec.Radius);
            col->SetHeight(spec.Height);
            col->SetPositionOffset(spec.Offset.x, spec.Offset.y, spec.Offset.z);
            col->SetAttackAmount(spec.AttackAmount);
            col->SetMyMask(static_cast<eCollisionGroup>(spec.MyMask));
            col->SetTarGetTargetMask(static_cast<eCollisionGroup>(spec.TargetMask));
            col->SetColor(spec.DebugColor);
            col->SetActive(spec.Active);

            pointers.push_back(raw);
            m_upColliders->AddCollider(std::move(col));
        }

        m_AttackColliders[type] = std::move(pointers);
    }
}
