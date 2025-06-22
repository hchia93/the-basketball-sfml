#include "TextWidgetBuilder.h"
#include "TextWidgetProcessor.h"

TextWidgetBuilder::TextWidgetBuilder()
    : m_Font(nullptr)
    , m_CharacterSize(30)
    , m_Position(0.0f, 0.0f)
    , m_Prefix("")
    , m_Suffix("")
    , m_Callback(nullptr)
    , m_RawString("NO TEXT")
    , m_EndLocation(0.0f, 0.0f)
    , m_FadeTime(0.0f)
    , m_IsPaused(false)
    , m_IsActive(true)
{
}

TextWidgetBuilder& TextWidgetBuilder::WithFont(std::shared_ptr<sf::Font> font)
{
    m_Font = font;
    return *this;
}

TextWidgetBuilder& TextWidgetBuilder::WithSize(unsigned int characterSize)
{
    m_CharacterSize = characterSize;
    return *this;
}

TextWidgetBuilder& TextWidgetBuilder::AtPosition(const sf::Vector2f& position)
{
    m_Position = position;
    return *this;
}

TextWidgetBuilder& TextWidgetBuilder::WithPrefix(const std::string& prefix)
{
    m_Prefix = prefix;
    return *this;
}

TextWidgetBuilder& TextWidgetBuilder::WithSuffix(const std::string& suffix)
{
    m_Suffix = suffix;
    return *this;
}

TextWidgetBuilder& TextWidgetBuilder::WithCallback(std::function<std::string()> callback)
{
    m_Callback = callback;
    return *this;
}

TextWidgetBuilder& TextWidgetBuilder::WithRawString(const std::string& rawString)
{
    m_RawString = rawString;
    return *this;
}

TextWidgetBuilder& TextWidgetBuilder::WithEndLocation(const sf::Vector2f& endLocation)
{
    m_EndLocation = endLocation;
    return *this;
}

TextWidgetBuilder& TextWidgetBuilder::WithFadeTime(float fadeTime)
{
    m_FadeTime = fadeTime;
    return *this;
}

TextWidgetBuilder& TextWidgetBuilder::IsPaused(bool isPaused)
{
    m_IsPaused = isPaused;
    return *this;
}

TextWidgetBuilder& TextWidgetBuilder::IsActive(bool isActive)
{
    m_IsActive = isActive;
    return *this;
}

std::unique_ptr<FTextWidget> TextWidgetBuilder::Build()
{
    auto widget = std::make_unique<FTextWidget>(m_Font, m_CharacterSize, m_Position);
    widget->m_Prefix = m_Prefix;
    widget->m_Suffix = m_Suffix;
    widget->m_UpdateCallback = m_Callback;
    widget->m_RawString = m_RawString;
    widget->m_EndLocation = m_EndLocation;
    widget->m_FadeTime = m_FadeTime;
    widget->m_bIsPaused = m_IsPaused;
    widget->m_bIsActive = m_IsActive;
    return widget;
}

FTextWidget* TextWidgetBuilder::BuildAndAdd(FTextWidgetProcessor& processor)
{
    auto widget = Build();
    return processor.AddAndInitialize(widget);
}

FTextWidget* TextWidgetBuilder::CreateScoreWidget(FTextWidgetProcessor& processor, std::shared_ptr<sf::Font> font, 
                                                 const sf::Vector2f& position, const std::string& prefix,
                                                 std::function<std::string()> callback)
{
    return TextWidgetBuilder()
        .WithFont(font)
        .WithSize(30)
        .AtPosition(position)
        .WithPrefix(prefix)
        .WithCallback(callback)
        .BuildAndAdd(processor);
}

FTextWidget* TextWidgetBuilder::CreateTimerWidget(FTextWidgetProcessor& processor, std::shared_ptr<sf::Font> font,
                                                 const sf::Vector2f& position, const std::string& prefix,
                                                 std::function<std::string()> callback, const std::string& suffix)
{
    return TextWidgetBuilder()
        .WithFont(font)
        .WithSize(30)
        .AtPosition(position)
        .WithPrefix(prefix)
        .WithSuffix(suffix)
        .WithCallback(callback)
        .BuildAndAdd(processor);
}

FTextWidget* TextWidgetBuilder::CreateMessageWidget(FTextWidgetProcessor& processor, std::shared_ptr<sf::Font> font,
                                                   const sf::Vector2f& position, const std::string& message,
                                                   float fadeTime, bool isPaused)
{
    return TextWidgetBuilder()
        .WithFont(font)
        .WithSize(30)
        .AtPosition(position)
        .WithRawString(message)
        .WithEndLocation(sf::Vector2f(position.x, -16))
        .WithFadeTime(fadeTime)
        .IsPaused(isPaused)
        .BuildAndAdd(processor);
} 