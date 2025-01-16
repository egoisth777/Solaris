#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H
#include <qevent.h>
#include <Interface/IUpdateComponent.h>
#include <scene/player.h>
class MyGL;

#define KEYCOUNT 30

class InputHandler : public IUpdateComponent
{
private:
    MyGL* m_mygl;
    Player* m_player;
    InputBundle NULL_INPUT;
    InputBundle m_inputs; // A collection of variables to be updated in keyPressEvent, mouseMoveEvent, mousePressEvent, etc.
    float mouseSensitivity = 0.1f;
    glm::vec2 m_mouseOffset = glm::vec2(0);
    std::unordered_set<int> m_holdingKey;
    std::unordered_set<int> m_newHoldingKey;    // New holding key in this frame
    std::unordered_set<int> m_newReleasingKey;  // Released key in this frame


    /// Note that disable it will block passing input to the player,
    /// the handler itself still receive input
    bool m_isInputEnable = false;
    bool m_isCursorEnable = true;

public:
    InputHandler(MyGL*, Player*);

    void onKeyPress(QKeyEvent *e);
    void onKeyRelease(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);

    /// Check if the key is pressed and not released
    bool isKeyPressing(int) const;

    /// Check if the key is new pressed or new released in this frame
    bool isModifierTriggered(int) const;
    bool isKeyReleased(int) const;
    bool isKeyDown(int) const;
    InputBundle& getInputs();

    bool isCursorEnable() const;
    void enableCursor(bool);

    bool isUserInputEnable() const;
    void enableUserInput(bool);


private:
    /// Check if ALT is pressed
    /// If yes, enable cursor and block input
    void checkInputSwitcherPress();

    // IUpdateComponent interface
public:
    void start() override;
    void onGameStart() override;
};


/// TODO: Inventory / Storage System
/// Inventory MGR
/// Global Player Position
///     On cube destoried: generate drop item
///     Drop item tick to see direction towrad player position
///         If in bound, fly toward the position
///         When distance < epsilon => Inventory mgr catch the item

#endif // INPUTHANDLER_H
