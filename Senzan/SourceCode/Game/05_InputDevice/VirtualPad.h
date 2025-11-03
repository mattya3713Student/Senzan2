#pragma once
#include "Game/05_InputDevice/Input.h"
#include "System/Singleton/SingletonTemplate.h"

class VirtualPad final 
    : public Singleton<VirtualPad>
{
private:
    friend class Singleton<VirtualPad>;
    VirtualPad();

public:

    // このゲームの離散アクション.
    enum class eGameAction
    {
        None,
        MoveForward,
        MoveBackward,
        MoveRight,
        MoveLeft,
        Jump,
        Attack,
        Parry,
        Dodge,
        Pause,
        SpecialAttack,

        Move_Axis_X,
        Move_Axis_Y,
        Camera_X,
        Camera_Y,
    };

    // このゲームの複合軸アクション.
    enum class eGameAxisAction
    {
        None,
        CameraMove,
        Move,
    };

    // 入力タイプ.
    enum class eActionType
    {
        Button,
        Axis
    };

    // 入力ソースを定義.
    struct InputSource
    {
        enum class eSourceType
        {
            KeyBorad,
            MouseButton,
            MouseMove,
            ControllerButton,
            ControllerStickDir,
            ControllerStickAxis,
            ControllerTriggerAxis
        };

        eSourceType Type;
        int KeyCode = 0;
        XInput::Key ControllerKey = XInput::Key::None;

        XInput::StickState StickState = XInput::StickState::None;

        enum class eStickTarget
        {
            None,
            Left,
            Right,
            LeftTrigger,
            RightTrigger
        };

        eStickTarget StickTarget = eStickTarget::None;
        
        float Scale = 1.0f;
    };

    // アクションごとのバインディング定義
    struct ActionBinding
    {
        eActionType Type = eActionType::Button;
        std::vector<InputSource> Sources;
    };

public:
    // ゲーム内のアクションと実際の入力を関連付けるマップ
    // eGameAction は離散アクションと軸コンポーネントの両方のキーとして使用されます。
    std::map<eGameAction, ActionBinding> m_KeyMap;

public:
    ~VirtualPad() override = default;

    // 押され続けているか.
    bool IsActionPress(eGameAction action) const;

    // 押された瞬間か.
    bool IsActionDown(eGameAction action, float inputBufferTime = 0.0f) const;

    // 離された瞬間か. 
    bool IsActionUp(eGameAction action) const;

    //  複合軸取得.
    DirectX::XMFLOAT2 GetAxisInput(eGameAxisAction axisType) const;

    // キーバインドの初期化.
    void SetupDefaultBindings();

private:
    // 状態チェックの抽象化ヘルパー.
    template <typename KeyCheckFunc, typename ButtonCheckFunc>
    bool checkActionState(eGameAction action,
        KeyCheckFunc&& keyCheck,
        ButtonCheckFunc&& buttonCheck) const;

    // 軸アクションの合計値を取得.
    float GetSingleAxisValue(eGameAction componentAction) const;

private:
    // TODO: バッファ機能未実装
    float m_CoyoteTimeTimer = 0.0f;
};