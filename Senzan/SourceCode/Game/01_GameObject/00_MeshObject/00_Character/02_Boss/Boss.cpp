#include "Boss.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossMoveState/BossMoveState.h"

#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"
#include "Game/03_Collision/00_Core/Ex_CompositeCollider/CompositeCollider.h"

#include "BossAttackStateBase/BossAttackStateBase.h"
#include "BossAttackStateBase/BossStompState/BossStompState.h"
#include "BossAttackStateBase/BossSlashState/BossSlashState.h"
#include "BossAttackStateBase/BossChargeSlashState/BossChargeSlashState.h"
#include "BossAttackStateBase/BossShoutState/BossShoutState.h"

#include "System/Utility/StateMachine/StateMachine.h"

#include "BossAttackStateBase/BossSpecialState/BossSpecialState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossLaserState/BossLaserState.h"

#include "Resource/Mesh/02_Skin/SkinMesh.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossDeadState/BossDeadState.h"

#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeState/BossChargeState.h"

#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossThrowingState/BossThrowingState.h"

#include "System/Singleton/CollisionDetector/CollisionDetector.h"
#include "System/Singleton/CameraManager/CameraManager.h"


constexpr float HP_Max = 100.0f;

// Use Character::CreateAttackCollidersFromDefs to create and register colliders

Boss::Boss()
    : Character()
    , m_State(std::make_unique<StateMachine<Boss>>(this))
    , m_PlayerPos{}
    , m_TurnSpeed(0.1f)
    , m_MoveSpeed(0.3f)
    , m_vCurrentMoveVelocity(0.f, 0.f, 0.f)
    , m_CurrentAttackType(AttackType::Normal)
{
    AttachMesh(MeshManager::GetInstance().GetSkinMesh("boss"));

    DirectX::XMFLOAT3 pos = { 0.05f, 0.05f, 20.05f };
    DirectX::XMFLOAT3 scale = { 10.0f, 10.0f, 10.0f };
    DirectX::XMFLOAT3 Rotation = { 0.0f,0.0f,0.0f };
    m_spTransform->SetPosition(pos);
    m_spTransform->SetScale(scale);
    m_spTransform->SetRotationDegrees(Rotation);

    m_MaxHP = 100.f;
    m_HP = m_MaxHP;

    m_State->ChangeState(std::make_shared<BossIdolState>(this));

    // sample default specs (use ColliderSpec as spec carrier)
    {
        ColliderSpec s; s.Radius = 15.0f; s.Height = 40.0f; s.Offset = {0.0f,10.0f,-30.0f}; s.AttackAmount = 10.0f;
        s.MyMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Attack);
        s.TargetMask = static_cast<uint32_t>(eCollisionGroup::Player_Damage);
        s.DebugColor = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
        s.Active = false;
        m_AttackColliderDefs[static_cast<Character::AttackTypeId>(AttackType::Normal)] = { s };
    }
    {
        ColliderSpec s; s.Radius = 30.0f; s.Height = 15.0f; s.Offset = {0.0f,0.0f,0.0f}; s.AttackAmount = 5.0f;
        s.MyMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Attack);
        s.TargetMask = static_cast<uint32_t>(eCollisionGroup::Player_Damage);
        m_AttackColliderDefs[static_cast<Character::AttackTypeId>(AttackType::Stomp)] = { s };
    }
    {
        ColliderSpec a; a.Radius = 20.0f; a.Height = 30.0f; a.Offset = {0,10,-20}; a.AttackAmount = 20.0f; a.MyMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Attack); a.TargetMask = static_cast<uint32_t>(eCollisionGroup::Player_Damage);
        ColliderSpec b; b.Radius = 10.0f; b.Height = 10.0f; b.Offset = {5,5,-25}; b.AttackAmount = 15.0f; b.MyMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Attack); b.TargetMask = static_cast<uint32_t>(eCollisionGroup::Player_Damage);
        m_AttackColliderDefs[static_cast<Character::AttackTypeId>(AttackType::Special)] = { a, b };
    }
    {
        ColliderSpec s; s.Radius = 8.0f; s.Height = 8.0f; s.Offset = {0,5,-10}; s.AttackAmount = 12.0f; s.MyMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Attack); s.TargetMask = static_cast<uint32_t>(eCollisionGroup::Player_Damage);
        m_AttackColliderDefs[static_cast<Character::AttackTypeId>(AttackType::Throwing)] = { s };
    }
    {
        ColliderSpec s; s.Radius = 18.0f; s.Height = 30.0f; s.Offset = {0,8,-25}; s.AttackAmount = 18.0f; s.MyMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Attack); s.TargetMask = static_cast<uint32_t>(eCollisionGroup::Player_Damage);
        m_AttackColliderDefs[static_cast<Character::AttackTypeId>(AttackType::Charge)] = { s };
    }
    {
        ColliderSpec s; s.Radius = 25.0f; s.Height = 20.0f; s.Offset = {0,2,0}; s.AttackAmount = 8.0f; s.MyMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Attack); s.TargetMask = static_cast<uint32_t>(eCollisionGroup::Player_Damage);
        m_AttackColliderDefs[static_cast<Character::AttackTypeId>(AttackType::Jump)] = { s };
    }
    {
        ColliderSpec s; s.Radius = 5.0f; s.Height = 50.0f; s.Offset = {0,25,100}; s.AttackAmount = 30.0f; s.MyMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Attack); s.TargetMask = static_cast<uint32_t>(eCollisionGroup::Player_Damage);
        m_AttackColliderDefs[static_cast<Character::AttackTypeId>(AttackType::Laser)] = { s };
    }
    {
        ColliderSpec s; s.Radius = 50.0f; s.Height = 75.0f; s.Offset = {0,1.5f,0}; s.AttackAmount = 10.0f; s.MyMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Attack); s.TargetMask = static_cast<uint32_t>(eCollisionGroup::Player_Damage);
        m_AttackColliderDefs[static_cast<Character::AttackTypeId>(AttackType::Shout)] = { s };
    }

    // Pre-create and register colliders for each attack type into m_upColliders using Character helper
    CreateAttackCollidersFromDefs(m_AttackColliderDefs);

    // Keep composite registered to global detector.
    CollisionDetector::GetInstance().RegisterCollider(*m_upColliders);
}

Boss::~Boss()
{
    CollisionDetector::GetInstance().UnregisterCollider(*m_upColliders);
}

void Boss::SetSlashBoneName(const std::string& name)
{
    m_slashBoneName = name;
}

void Boss::SetStompBoneName(const std::string& name)
{
    m_stompBoneName = name;
}

void Boss::SetAttackCollidersActive(AttackType type, bool active)
{
    Character::AttackTypeId id = static_cast<Character::AttackTypeId>(type);
    auto it = m_AttackColliders.find(id);
    if (it == m_AttackColliders.end()) return;
    for (ColliderBase* col : it->second)
    {
        if (col) col->SetActive(active);
    }
}

void Boss::SetAttackColliderActive(AttackType type, size_t index, bool active)
{
    Character::AttackTypeId id = static_cast<Character::AttackTypeId>(type);
    auto it = m_AttackColliders.find(id);
    if (it == m_AttackColliders.end()) return;
    if (index >= it->second.size()) return;
    ColliderBase* col = it->second[index];
    if (col) col->SetActive(active);
}

void Boss::SetAttackColliderActive(bool active)
{
    // Backwards compatibility: toggle colliders for current attack type
    SetAttackCollidersActive(m_CurrentAttackType, active);
}

void Boss::Update()
{
    Character::Update();

    //距離の計算後にステートを更新する.
    m_State->Update();

#if _DEBUG
    if (GetAsyncKeyState(VK_RETURN) & 0x0001)
    {
        Hit();
    }
#endif
}

void Boss::LateUpdate()
{

    Character::LateUpdate();

    if (!m_State) {
        return;
    }

    // Update cached bone matrices once per frame if names provided
    if (!m_slashBoneName.empty())
    {
        auto sp = GetAttachMesh().lock();
        auto skin = std::dynamic_pointer_cast<SkinMesh>(sp);
        if (skin)
        {
            DirectX::XMMATRIX m;
            if (skin->GetMatrixFromBone(m_slashBoneName.c_str(), &m))
            {
                m_slashBoneMatrix = m;
                m_hasSlashBoneMatrix = true;
            }
            else
            {
                m_hasSlashBoneMatrix = false;
            }
        }
    }

    if (!m_stompBoneName.empty())
    {
        auto sp = GetAttachMesh().lock();
        auto skin = std::dynamic_pointer_cast<SkinMesh>(sp);
        if (skin)
        {
            DirectX::XMMATRIX m;
            if (skin->GetMatrixFromBone(m_stompBoneName.c_str(), &m))
            {
                m_stompBoneMatrix = m;
                m_hasStompBoneMatrix = true;
            }
            else
            {
                m_hasStompBoneMatrix = false;
            }
        }
    }

    UpdateSlashColliderTransform();
    UpdateStompColliderTransform();

    // ステートマシーンの最終更新を実行.
    m_State->LateUpdate();

    // 衝突イベント処理を実行
    HandleParryDetection();
    HandleDamageDetection();
    HandleAttackDetection();
    HandleDodgeDetection();
}

void Boss::Draw()
{
    MeshObject::Draw();
    m_State->Draw();

}
void Boss::Init()
{
}

StateMachine<Boss>* Boss::GetStateMachine()
{
    return m_State.get();
}

LPD3DXANIMATIONCONTROLLER Boss::GetAnimCtrl() const
{
    return m_pAnimCtrl;
}

void Boss::Hit()
{
    //ボスの体力の最小値.
    constexpr float zero = 0.0f;
    //ボスがPlayerからの攻撃を受けるダメージ変数.
    //このダメージは今は仮でおいているだけです
    //通常攻撃.
    constexpr float ten = 10.0f;
    //必殺技.
    constexpr float twenty = 20.0f;
    //ジャスト回避時の攻撃.
    constexpr float Five = 5.0f;
    //パリィの時の与えるダメージ.
    constexpr float Fifteen = 15.0f;

    //Bossの体力でのステートにいれる.
    constexpr float Dead_HP = zero;


    //いったんこの10ダメだけにしておく.
    //最後はTenをBaseにして+や-を使用する感じになると思っている.
    m_HP -= ten;
    if (m_HP <= 0.0f)
    {
        //死んだときにDeadStateclassに入る.
        m_State->ChangeState(std::make_shared<BossDeadState>(this));
    }

    Update();
}

void Boss::SetTargetPos(const DirectX::XMFLOAT3 Player_Pos)
{
    m_PlayerPos = Player_Pos;
}


// 衝突_被ダメージ.
void Boss::HandleDamageDetection()
{
    if (!m_upColliders) return;

    const auto& internal_colliders = m_upColliders->GetInternalColliders();

    for (const auto& collider_ptr : internal_colliders)
    {
        const ColliderBase* current_collider = collider_ptr.get();

        if ((current_collider->GetMyMask() & eCollisionGroup::Enemy_Damage) == eCollisionGroup::None) {
            continue;
        }

        for (const CollisionInfo& info : current_collider->GetCollisionEvents())
        {
            if (!info.IsHit) continue;
            const ColliderBase* otherCollider = info.ColliderB;
            if (!otherCollider) { continue; }

            eCollisionGroup other_group = otherCollider->GetMyMask();

            if ((other_group & eCollisionGroup::Player_Attack) != eCollisionGroup::None)
            {
                // ダメージを適用 
                ApplyDamage(info.AttackAmount);

                Time::GetInstance().SetWorldTimeScale(0.1f, 0.016f * 8);
                CameraManager::GetInstance().ShakeCamera(0.1f, 2.5f); // カメラを少し揺らす.

                // 1フレームに1回.
                return;
            }
        }
    }
}

void Boss::HandleAttackDetection()
{
    if (!m_upColliders) return;

    const auto& internal_colliders = m_upColliders->GetInternalColliders();

    for (const auto& collider_ptr : internal_colliders)
    {
        const ColliderBase* current_collider = collider_ptr.get();

        if ((current_collider->GetMyMask() & eCollisionGroup::Enemy_Attack) == eCollisionGroup::None) {
            continue;
        }

        for (const CollisionInfo& info : current_collider->GetCollisionEvents())
        {
            if (!info.IsHit) continue;
            const ColliderBase* otherCollider = info.ColliderB;
            if (!otherCollider) { continue; }

            eCollisionGroup other_group = otherCollider->GetMyMask();

            if ((other_group & eCollisionGroup::Player_Damage) != eCollisionGroup::None)
            {
                // disable all colliders belonging to current attack type
                SetAttackCollidersActive(m_CurrentAttackType, false);

                // 1フレームに1回.
                return;
            }
        }
    }
}

void Boss::HandleDodgeDetection()
{
    if (!m_upColliders) return;

    const auto& internal_colliders = m_upColliders->GetInternalColliders();

    for (const auto& collider_ptr : internal_colliders)
    {
        const ColliderBase* current_collider = collider_ptr.get();

        if ((current_collider->GetMyMask() & eCollisionGroup::Player_JustDodge) == eCollisionGroup::None) {
            continue;
        }

        for (const CollisionInfo& info : current_collider->GetCollisionEvents())
        {
            if (!info.IsHit) continue;
            const ColliderBase* otherCollider = info.ColliderB;
            if (!otherCollider) { continue; }

            eCollisionGroup other_group = otherCollider->GetMyMask();

            if ((other_group & eCollisionGroup::Player_Attack) != eCollisionGroup::None)
            {
                //Parry();
                // 1フレームに1回.
                return;
            }
        }
    }
}

void Boss::HandleParryDetection()
{
    if (!m_upColliders) return;

    const auto& internal_colliders = m_upColliders->GetInternalColliders();

    for (const auto& collider_ptr : internal_colliders)
    {
        const ColliderBase* current_collider = collider_ptr.get();

        if ((current_collider->GetMyMask() & eCollisionGroup::Enemy_Attack) == eCollisionGroup::None) {
            continue;
        }

        for (const CollisionInfo& info : current_collider->GetCollisionEvents())
        {
            if (!info.IsHit) continue;
            const ColliderBase* otherCollider = info.ColliderB;
            if (!otherCollider) { continue; }

            eCollisionGroup other_group = otherCollider->GetMyMask();

            if ((other_group & eCollisionGroup::Player_Parry) != eCollisionGroup::None)
            {
                auto* currentState = m_State->m_pOwner; // 現在の状態を取得
                if (BossAttackStateBase* attackState = dynamic_cast<BossAttackStateBase*>(currentState))
                {
                    // 現在のステートが BossAttackStateBase だった場合のみ実行される
                    attackState->ParryTime();
                    return;
                }

                // disable all colliders of current attack type
                SetAttackCollidersActive(m_CurrentAttackType, false);

                // 一フレーム1回.
                return;
            }
        }
    }
}

// UpdateSlashColliderTransform: set offsets for all slash colliders
void Boss::UpdateSlashColliderTransform()
{
    if (GetAttachMesh().expired()) return;
    auto skinMesh = std::dynamic_pointer_cast<SkinMesh>(GetAttachMesh().lock());
    if (!skinMesh) return;

    // gather slash-related colliders (Normal, Charge, Special, Throwing)
    std::vector<ColliderBase*> slashColls;
    auto appendType = [&](AttackType t){
        auto it = m_AttackColliders.find(static_cast<Character::AttackTypeId>(t));
        if (it != m_AttackColliders.end()) {
            for (ColliderBase* c : it->second) if (c) slashColls.push_back(c);
        }
    };
    appendType(AttackType::Normal);
    appendType(AttackType::Charge);
    appendType(AttackType::Special);
    appendType(AttackType::Throwing);

    if (slashColls.empty()) return;

    DirectX::XMMATRIX bone_local_matrix;
    bool haveLocal = false;

    if (m_hasSlashBoneMatrix)
    {
        bone_local_matrix = m_slashBoneMatrix;
        haveLocal = true;
    }
    else
    {
        const std::string targetBoneName = "boss_Hand_R";
        if (!skinMesh->GetMatrixFromBone(targetBoneName.c_str(), &bone_local_matrix))
            return;
    }

    DirectX::XMMATRIX boss_world_matrix = m_spTransform->GetWorldMatrix();
    DirectX::XMMATRIX bone_world_matrix = bone_local_matrix * boss_world_matrix;

    DirectX::XMVECTOR v_final_pos, v_final_quat, v_final_scale;
    DirectX::XMMatrixDecompose(&v_final_scale, &v_final_quat, &v_final_pos, bone_world_matrix);

    DirectX::XMVECTOR b_pos, b_quat, b_scale;
    DirectX::XMMatrixDecompose(&b_scale, &b_quat, &b_pos, boss_world_matrix);

    DirectX::XMVECTOR relative_pos = DirectX::XMVectorSubtract(v_final_pos, b_pos);
    DirectX::XMFLOAT3 f_relative_pos;
    DirectX::XMStoreFloat3(&f_relative_pos, relative_pos);

    for (size_t i = 0; i < slashColls.size(); ++i)
    {
        ColliderBase* col = slashColls[i];
        if (!col) continue;
        float extraY = static_cast<float>(i) * 5.0f;
        float extraZ = static_cast<float>(i) * -5.0f;
        col->SetPositionOffset(f_relative_pos.x, f_relative_pos.y + extraY, f_relative_pos.z + extraZ);
    }
}

void Boss::UpdateStompColliderTransform()
{
    if (GetAttachMesh().expired()) return;
    auto skinMesh = std::dynamic_pointer_cast<SkinMesh>(GetAttachMesh().lock());
    if (!skinMesh) return;

    // gather stomp-related colliders (Jump, Stomp)
    std::vector<ColliderBase*> stompColls;
    auto appendType = [&](AttackType t){
        auto it = m_AttackColliders.find(static_cast<Character::AttackTypeId>(t));
        if (it != m_AttackColliders.end()) {
            for (ColliderBase* c : it->second) if (c) stompColls.push_back(c);
        }
    };
    appendType(AttackType::Jump);
    appendType(AttackType::Stomp);

    if (stompColls.empty()) return;

    DirectX::XMMATRIX bone_local_matrix;
    bool haveLocal = false;

    if (m_hasStompBoneMatrix)
    {
        bone_local_matrix = m_stompBoneMatrix;
        haveLocal = true;
    }

    if (haveLocal)
    {
        DirectX::XMMATRIX boss_world_matrix = m_spTransform->GetWorldMatrix();
        DirectX::XMMATRIX bone_world_matrix = bone_local_matrix * boss_world_matrix;

        DirectX::XMVECTOR v_final_pos, v_final_quat, v_final_scale;
        DirectX::XMMatrixDecompose(&v_final_scale, &v_final_quat, &v_final_pos, bone_world_matrix);

        DirectX::XMVECTOR b_pos, b_quat, b_scale;
        DirectX::XMMatrixDecompose(&b_scale, &b_quat, &b_pos, boss_world_matrix);

        DirectX::XMVECTOR relative_pos = DirectX::XMVectorSubtract(v_final_pos, b_pos);
        DirectX::XMFLOAT3 f_relative_pos;
        DirectX::XMStoreFloat3(&f_relative_pos, relative_pos);

        for (size_t i = 0; i < stompColls.size(); ++i)
        {
            ColliderBase* col = stompColls[i];
            if (!col) continue;
            col->SetPositionOffset(f_relative_pos.x, f_relative_pos.y, f_relative_pos.z);
        }
        return;
    }

    // Fallback using pos-from-bone for single stomp collider
    const std::string TargetBoneName = "boss_pSphere28";
    DirectX::XMFLOAT3 boneWorldPos{};
    if (skinMesh->GetPosFromBone(TargetBoneName.c_str(), &boneWorldPos))
    {
        DirectX::XMMATRIX bossWorldMatrix = m_spTransform->GetWorldMatrix();
        DirectX::XMVECTOR bossPosVec = bossWorldMatrix.r[3];
        DirectX::XMFLOAT3 bossWorldPos;
        DirectX::XMStoreFloat3(&bossWorldPos, bossPosVec);

        float offsetX = boneWorldPos.x - bossWorldPos.x;
        float offsetY = boneWorldPos.y - bossWorldPos.y;
        float offsetZ = boneWorldPos.z - bossWorldPos.z;

        for (size_t i = 0; i < stompColls.size(); ++i)
        {
            ColliderBase* col = stompColls[i];
            if (!col) continue;
            col->SetPositionOffset(offsetX, offsetY, offsetZ);
        }
    }
}

// Update other functions to use m_AttackColliders where appropriate
ColliderBase* Boss::GetSlashCollider() const
{
    // return first available from (Normal, Charge, Special, Throwing)
    const AttackType types[] = { AttackType::Normal, AttackType::Charge, AttackType::Special, AttackType::Throwing };
    for (AttackType t : types)
    {
        auto it = m_AttackColliders.find(static_cast<Character::AttackTypeId>(t));
        if (it != m_AttackColliders.end() && !it->second.empty()) return it->second.front();
    }
    return nullptr;
}

ColliderBase* Boss::GetStompCollider() const
{
    const AttackType types[] = { AttackType::Jump, AttackType::Stomp };
    for (AttackType t : types)
    {
        auto it = m_AttackColliders.find(static_cast<Character::AttackTypeId>(t));
        if (it != m_AttackColliders.end() && !it->second.empty()) return it->second.front();
    }
    return nullptr;
}

ColliderBase* Boss::GetShoutCollider() const
{
    auto it = m_AttackColliders.find(static_cast<Character::AttackTypeId>(AttackType::Shout));
    if (it != m_AttackColliders.end() && !it->second.empty()) return it->second.front();
    it = m_AttackColliders.find(static_cast<Character::AttackTypeId>(AttackType::Laser));
    if (it != m_AttackColliders.end() && !it->second.empty()) return it->second.front();
    return nullptr;
}
