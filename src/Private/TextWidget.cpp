#include "TextWidget.h"
#include <cstdint>

void FTextWidget::Init()
{
	m_Text = std::make_unique<sf::Text>(*m_Font, m_RawString, m_CharacterSize);
	m_Text->setPosition(m_StartLocation);
	m_LifeTime = 0.0f;

	// Restore alpha
	sf::Color defaultFillColor = m_Text->getFillColor();
	defaultFillColor.a = 255;
	m_Text->setFillColor(defaultFillColor);

	sf::Color defaultOutlineColor = m_Text->getOutlineColor();
	defaultOutlineColor.a = 255;
	m_Text->setOutlineColor(defaultOutlineColor);
}

void FTextWidget::UpdateText()
{
	if (m_UpdateCallback)
	{
		std::string newText = m_Prefix + m_UpdateCallback() + m_Suffix;
		if (m_Text && newText != m_Text->getString())
		{
			m_Text->setString(newText);
		}
	}
} 