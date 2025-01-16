#include "globalinstance.h"
#include "inputhandler.h"
#include "macros/debugMacro.h"
#include <iostream>
#include <qapplication.h>
#include "mygl.h"

InputHandler::InputHandler(MyGL* widget, Player* player) :
    m_mygl(widget),
    m_player(player),
    m_holdingKey()
{
    m_holdingKey.reserve(KEYCOUNT);
    m_newHoldingKey.reserve(10);
    m_newReleasingKey.reserve(10);

    // MyGL will track the mouse's movements even if a mouse button is not pressed
    m_mygl->setMouseTracking(true);
    m_inputs = NULL_INPUT;
    setEnable(false);
}

void InputHandler::onKeyPress(QKeyEvent *e)
{
    if(e->isAutoRepeat()){
        return;
    }
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    }

    if(isKeyPressing(e->key())){
        return;
    }

#if defined JASON_DEBUG
    std::cout << "New pressing: " << e->key() << std::endl;
#endif

    m_holdingKey.insert(e->key());
    m_newHoldingKey.insert(e->key());
}

void InputHandler::onKeyRelease(QKeyEvent *e)
{
    if(e->isAutoRepeat()){
        return;
    }

#if defined JASON_DEBUG
    std::cout << "Release: " << e->key() << std::endl;
#endif

    m_holdingKey.erase(e->key());
    m_newReleasingKey.insert(e->key());
}

void InputHandler::mouseMoveEvent(QMouseEvent *e)
{
    if(isCursorEnable()){
        return;
    }

    QWidget* currentFocused = m_mygl;
    QPoint center = currentFocused->rect().center();
    QPoint position = e->pos();

    m_mouseOffset.x += mouseSensitivity * float(center.x() - position.x());
    m_mouseOffset.y += mouseSensitivity * float(center.y() - position.y());

    QCursor::setPos(currentFocused->mapToGlobal(currentFocused->rect().center()));
}

void InputHandler::mousePressEvent(QMouseEvent *)
{}

bool InputHandler::isKeyPressing(int key) const
{
    return m_holdingKey.find(key) != m_holdingKey.end();
}

bool InputHandler::isModifierTriggered(int key) const
{
    return isKeyReleased(key) ||
           isKeyDown(key);
}

bool InputHandler::isKeyReleased(int key) const
{
    return  m_newReleasingKey.find(key) != m_newReleasingKey.end();
}

bool InputHandler::isKeyDown(int key) const
{
    return  m_newHoldingKey.find(key) != m_newHoldingKey.end();
}


InputBundle& InputHandler::getInputs()
{
    m_inputs = NULL_INPUT;

    if(!this->isEnabled()){
        return m_inputs;
    }

    checkInputSwitcherPress();
    if(isUserInputEnable()){
        // http://doc.qt.io/qt-5/qt.html#Key-enum
        m_inputs.PressedRight               =   isKeyPressing(Qt::Key_D);
        m_inputs.PressedLeft                =   isKeyPressing(Qt::Key_A);
        m_inputs.PressedBackward            =   isKeyPressing(Qt::Key_S);
        m_inputs.PressedForward             =   isKeyPressing(Qt::Key_W);
        m_inputs.PressedUp                  =   isKeyPressing(Qt::Key_E);
        m_inputs.PressedDown                =   isKeyPressing(Qt::Key_Q);
        m_inputs.PressedJump                =   isKeyPressing(Qt::Key_Space);
        m_inputs.SwitchedChangeSpeedModifier=   isModifierTriggered(Qt::Key_Shift);
        m_inputs.SwitchedChangeFlightMode   =   isKeyReleased(Qt::Key_F);
        m_inputs.SwitchedChangePhotoMode    =   isKeyReleased(Qt::Key_C);

        // Shader profiler
        m_inputs.EnableShaderFeature1       =   isKeyReleased(Qt::Key_1);
        m_inputs.EnableShaderFeature2       =   isKeyReleased(Qt::Key_2);
        m_inputs.EnableShaderFeature3       =   isKeyReleased(Qt::Key_3);

        m_inputs.RenderTimeSpeedUp          =   isKeyReleased(Qt::Key_Period);
        m_inputs.RenderTimeSpeedDown        =   isKeyReleased(Qt::Key_Comma);
        m_inputs.RenderTimeSpeedReverse     =   isKeyReleased(Qt::Key_Slash);

        m_inputs.mouseX = m_mouseOffset.x;
        m_inputs.mouseY = m_mouseOffset.y;
    }

    /// Reset cache
    m_newHoldingKey.clear();
    m_newReleasingKey.clear();
    m_mouseOffset = glm::vec2(0);
    return m_inputs;
}

bool InputHandler::isCursorEnable() const
{
    return m_isCursorEnable;
}

void InputHandler::enableCursor(bool val)
{
    m_isCursorEnable = val;
    m_mygl->setCursor(val ? Qt::ArrowCursor : Qt::BlankCursor);
}

bool InputHandler::isUserInputEnable() const
{
    return m_isInputEnable;
}

void InputHandler::enableUserInput(bool val)
{
    m_isInputEnable = val;
}

void InputHandler::checkInputSwitcherPress()
{
    const int altKey = Qt::Key_Alt;
    /// Pressing ALT will make the mouse appear and block other input
    if(isKeyDown(altKey)){
        //std::cout << "ALT DOWN" << std::endl;
        enableCursor(true);
        enableUserInput(false);
    }
    else if(isKeyReleased(altKey)){
        //std::cout << "ALT UP" << std::endl;
        enableCursor(false);
        enableUserInput(true);
    }
}

void InputHandler::start(){
}

void InputHandler::onGameStart()
{
    setEnable(true);
    enableUserInput(true);
    enableCursor(false);
}
