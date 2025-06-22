#include "GameInput.h"
#include "Application.h"
#include "GameLogic.h"

GameInput::GameInput(Application* application)
    : m_Application(application)
    , m_RightMousePressed(false)
    , m_MiddleMousePressed(false)
    , m_OnSpacePressed(nullptr)
    , m_OnLeftMousePressed(nullptr)
    , m_OnLeftMouseReleased(nullptr)
    , m_OnRightMousePressed(nullptr)
    , m_OnMiddleMousePressed(nullptr)
    , m_OnMouseMoved(nullptr)
    , m_OnUpdateVisualFeedback(nullptr)
{
}

GameInput::~GameInput()
{
}

void GameInput::HandleInput()
{
    // Handle keyboard input
    HandleKeyboardInput();
    
    // Handle mouse input
    HandleMouseInput();
    
    // Update visual feedback elements
    UpdateVisualFeedback();
}

void GameInput::HandleKeyboardInput()
{
    // Space bar to start game
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
    {
        if (m_OnSpacePressed)
        {
            m_OnSpacePressed();
        }
    }
}

void GameInput::HandleMouseInput()
{
    // Left mouse button - charge projection velocity
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if (m_OnLeftMousePressed)
        {
            m_OnLeftMousePressed();
        }
    }
    else
    {
        if (m_OnLeftMouseReleased)
        {
            m_OnLeftMouseReleased();
        }
    }

    // Right mouse button - spawn ball
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
        if (!m_RightMousePressed)
        {
            if (m_OnRightMousePressed)
            {
                m_OnRightMousePressed();
            }
            m_RightMousePressed = true;
        }
    }
    else
    {
        m_RightMousePressed = false;
    }
    
    // Middle mouse button - reset game
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))
    {
        if (!m_MiddleMousePressed)
        {
            if (m_OnMiddleMousePressed)
            {
                m_OnMiddleMousePressed();
            }
            m_MiddleMousePressed = true;
        }
    }
    else
    {
        m_MiddleMousePressed = false;
    }
}

void GameInput::UpdateVisualFeedback()
{
    // This method will be implemented to handle visual feedback updates
    // like charge gauge and angle indicators
    // For now, it's a placeholder that can be expanded later
    if (m_OnUpdateVisualFeedback)
    {
        m_OnUpdateVisualFeedback();
    }
}

sf::Vector2i GameInput::GetMousePosition() const
{
    return sf::Mouse::getPosition(m_Application->GetAppWindow());
}

// Callback setters
void GameInput::SetOnSpacePressed(std::function<void()> callback)
{
    m_OnSpacePressed = callback;
}

void GameInput::SetOnLeftMousePressed(std::function<void()> callback)
{
    m_OnLeftMousePressed = callback;
}

void GameInput::SetOnLeftMouseReleased(std::function<void()> callback)
{
    m_OnLeftMouseReleased = callback;
}

void GameInput::SetOnRightMousePressed(std::function<void()> callback)
{
    m_OnRightMousePressed = callback;
}

void GameInput::SetOnMiddleMousePressed(std::function<void()> callback)
{
    m_OnMiddleMousePressed = callback;
}

void GameInput::SetOnMouseMoved(std::function<void(const sf::Vector2i&)> callback)
{
    m_OnMouseMoved = callback;
}

void GameInput::SetOnUpdateVisualFeedback(std::function<void()> callback)
{
    m_OnUpdateVisualFeedback = callback;
} 