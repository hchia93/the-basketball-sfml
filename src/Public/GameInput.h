#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>

// Forward declarations
class Application;

class GameInput
{
public:
    GameInput(Application* application);
    ~GameInput();

    // Input handling methods
    void HandleInput();
    void UpdateVisualFeedback();

    // Callback setters
    void SetOnSpacePressed(std::function<void()> callback);
    void SetOnLeftMousePressed(std::function<void()> callback);
    void SetOnLeftMouseReleased(std::function<void()> callback);
    void SetOnRightMousePressed(std::function<void()> callback);
    void SetOnMiddleMousePressed(std::function<void()> callback);
    void SetOnMouseMoved(std::function<void(const sf::Vector2i&)> callback);
    void SetOnUpdateVisualFeedback(std::function<void()> callback);

    // Getters
    bool IsRightMousePressed() const { return m_RightMousePressed; }
    bool IsMiddleMousePressed() const { return m_MiddleMousePressed; }
    sf::Vector2i GetMousePosition() const;

private:
    Application* m_Application;
    
    // Input state
    bool m_RightMousePressed;
    bool m_MiddleMousePressed;
    
    // Internal input handling methods
    void HandleKeyboardInput();
    void HandleMouseInput();
    
    // Callbacks
    std::function<void()> m_OnSpacePressed;
    std::function<void()> m_OnLeftMousePressed;
    std::function<void()> m_OnLeftMouseReleased;
    std::function<void()> m_OnRightMousePressed;
    std::function<void()> m_OnMiddleMousePressed;
    std::function<void(const sf::Vector2i&)> m_OnMouseMoved;
    std::function<void()> m_OnUpdateVisualFeedback;
}; 