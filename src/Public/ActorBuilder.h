#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "Defines.h"

// Forward declarations
class Actor;
class Application;

class ActorBuilder
{
public:
    ActorBuilder();

    // Fluent interface methods
    ActorBuilder& WithName(const std::string& name);
    ActorBuilder& WithShape(EActorShapeType shapeType);
    ActorBuilder& WithBodyType(Eb2ShapeType bodyType);
    ActorBuilder& WithSize(const sf::Vector2f& size);
    ActorBuilder& WithLocation(const sf::Vector2f& location);
    ActorBuilder& WithRotation(float rotation);
    ActorBuilder& IsDynamic(bool isDynamic);
    ActorBuilder& GenerateOverlaps(bool generateOverlaps);
    ActorBuilder& AutoActivate(bool autoActivate);
    ActorBuilder& WithColor(const sf::Color& color);
    ActorBuilder& WithOutline(const sf::Color& outlineColor, float thickness = -1.0f);
    ActorBuilder& WithTexture(const std::string& texturePath);

    // Build the actor
    std::unique_ptr<Actor> Build(Application* app, void* worldContext);

    // Factory methods for common actor types
    static std::unique_ptr<Actor> CreateBorder(Application* app, void* worldContext, const std::string& name, 
                                              const sf::Vector2f& size, const sf::Vector2f& location, 
                                              const sf::Color& color = sf::Color(100, 100, 100));
    
    static std::unique_ptr<Actor> CreateSensor(Application* app, void* worldContext, const std::string& name,
                                              const sf::Vector2f& size, const sf::Vector2f& location,
                                              const sf::Color& color = sf::Color(255, 255, 0, 100));
    
    static std::unique_ptr<Actor> CreateBall(Application* app, void* worldContext, const std::string& name,
                                            const sf::Vector2f& size, const sf::Vector2f& location,
                                            const sf::Color& color = sf::Color::White);

private:
    std::string m_Name;
    EActorShapeType m_ShapeType;
    Eb2ShapeType m_BodyType;
    sf::Vector2f m_Size;
    sf::Vector2f m_Location;
    float m_Rotation;
    bool m_IsDynamic;
    bool m_GenerateOverlaps;
    bool m_AutoActivate;
    sf::Color m_Color;
    sf::Color m_OutlineColor;
    float m_OutlineThickness;
    std::string m_TexturePath;
    bool m_HasOutline;
    bool m_HasTexture;
}; 