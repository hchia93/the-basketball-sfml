#include "TextProcessor.h"
#include <cstdint>

void FTextWidget::Init()
{
	m_Text = std::make_unique<sf::Text>(*m_Font, m_RawString, m_CharacterSize);
	m_Text->setPosition(m_StartLocation);
	m_LifeTime = 0.0f;

	// Restore alpha
	sf::Color DefaultFillColor = m_Text->getFillColor();
	DefaultFillColor.a = 255;
	m_Text->setFillColor(DefaultFillColor);

	sf::Color DefaultOutlineColor = m_Text->getOutlineColor();
	DefaultOutlineColor.a = 255;
	m_Text->setOutlineColor(DefaultOutlineColor);
}

FTextWidget* FTextWidgetProcessor::AddAndInitialize(std::unique_ptr<FTextWidget>& widget)
{
	widget->Init();
	FTextWidget* widgetPtr = widget.get();
	m_TextWidgets.push_back(std::move(widget));
	return widgetPtr;
}

std::vector<std::unique_ptr<FTextWidget>>& FTextWidgetProcessor::GetTextWidgets()
{
	return m_TextWidgets;
}

void FTextWidgetProcessor::Tick()
{
	for (auto&& widget : m_TextWidgets)
	{
		// Lerps
		if (widget->m_FadeTime > 0.0f && widget->m_bIsActive && !widget->m_bIsPaused)
		{
			const float lerpDelta = widget->m_LifeTime / widget->m_FadeTime;
			const sf::Vector2f positionDiff = widget->m_EndLocation - widget->m_StartLocation;
			const sf::Vector2f resultPosition = widget->m_StartLocation + (positionDiff * lerpDelta);
			widget->m_Text->setPosition(resultPosition);

			sf::Color deltaFillColor = widget->m_Text->getFillColor();
			deltaFillColor.a = static_cast<std::uint8_t>((1.f - lerpDelta) * 255);
			widget->m_Text->setFillColor(deltaFillColor);

			sf::Color deltaOutlineColor = widget->m_Text->getOutlineColor();
			deltaOutlineColor.a = static_cast<std::uint8_t>((1.f - lerpDelta) * 255);
			widget->m_Text->setOutlineColor(deltaOutlineColor);

			widget->m_LifeTime += DELTA_TIME_STEP;

			if (widget->m_LifeTime >= widget->m_FadeTime)
			{
				widget->m_bIsActive = false;
			}
		}
	}
}
