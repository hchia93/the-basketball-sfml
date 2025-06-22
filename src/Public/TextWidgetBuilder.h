#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <string>
#include "TextWidget.h"

// Forward declarations
class FTextWidgetProcessor;

class TextWidgetBuilder
{
public:
    TextWidgetBuilder();

    // Fluent interface methods
    TextWidgetBuilder& WithFont(std::shared_ptr<sf::Font> font);
    TextWidgetBuilder& WithSize(unsigned int characterSize);
    TextWidgetBuilder& AtPosition(const sf::Vector2f& position);
    TextWidgetBuilder& WithPrefix(const std::string& prefix);
    TextWidgetBuilder& WithSuffix(const std::string& suffix);
    TextWidgetBuilder& WithCallback(std::function<std::string()> callback);
    TextWidgetBuilder& WithRawString(const std::string& rawString);
    TextWidgetBuilder& WithEndLocation(const sf::Vector2f& endLocation);
    TextWidgetBuilder& WithFadeTime(float fadeTime);
    TextWidgetBuilder& IsPaused(bool isPaused);
    TextWidgetBuilder& IsActive(bool isActive);

    // Build the widget
    std::unique_ptr<FTextWidget> Build();
    
    // Build and add to processor
    FTextWidget* BuildAndAdd(FTextWidgetProcessor& processor);

    // Factory methods for common widget types
    static FTextWidget* CreateScoreWidget(FTextWidgetProcessor& processor, std::shared_ptr<sf::Font> font, 
                                         const sf::Vector2f& position, const std::string& prefix,
                                         std::function<std::string()> callback);
    
    static FTextWidget* CreateTimerWidget(FTextWidgetProcessor& processor, std::shared_ptr<sf::Font> font,
                                         const sf::Vector2f& position, const std::string& prefix,
                                         std::function<std::string()> callback, const std::string& suffix = " S");
    
    static FTextWidget* CreateMessageWidget(FTextWidgetProcessor& processor, std::shared_ptr<sf::Font> font,
                                           const sf::Vector2f& position, const std::string& message,
                                           float fadeTime = 1.0f, bool isPaused = true);

private:
    std::shared_ptr<sf::Font> m_Font;
    unsigned int m_CharacterSize;
    sf::Vector2f m_Position;
    std::string m_Prefix;
    std::string m_Suffix;
    std::function<std::string()> m_Callback;
    std::string m_RawString;
    sf::Vector2f m_EndLocation;
    float m_FadeTime;
    bool m_IsPaused;
    bool m_IsActive;
}; 