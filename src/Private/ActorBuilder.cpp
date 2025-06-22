#include "ActorBuilder.h"
#include "Actor.h"
#include "Application.h"
#include "AssetLoader.h"

ActorBuilder::ActorBuilder()
    : m_Name("UnnamedActor")
    , m_ShapeType(EActorShapeType::Rectangle)
    , m_BodyType(Eb2ShapeType::Polygon)
    , m_Size(32.0f, 32.0f)
    , m_Location(0.0f, 0.0f)
    , m_Rotation(0.0f)
    , m_IsDynamic(false)
    , m_GenerateOverlaps(false)
    , m_AutoActivate(true)
    , m_Color(sf::Color::White)
    , m_OutlineColor(sf::Color::Black)
    , m_OutlineThickness(-1.0f)
    , m_TexturePath("")
    , m_HasOutline(false)
    , m_HasTexture(false)
{
}

ActorBuilder& ActorBuilder::WithName(const std::string& name)
{
    m_Name = name;
    return *this;
}

ActorBuilder& ActorBuilder::WithShape(EActorShapeType shapeType)
{
    m_ShapeType = shapeType;
    return *this;
}

ActorBuilder& ActorBuilder::WithBodyType(Eb2ShapeType bodyType)
{
    m_BodyType = bodyType;
    return *this;
}

ActorBuilder& ActorBuilder::WithSize(const sf::Vector2f& size)
{
    m_Size = size;
    return *this;
}

ActorBuilder& ActorBuilder::WithLocation(const sf::Vector2f& location)
{
    m_Location = location;
    return *this;
}

ActorBuilder& ActorBuilder::WithRotation(float rotation)
{
    m_Rotation = rotation;
    return *this;
}

ActorBuilder& ActorBuilder::IsDynamic(bool isDynamic)
{
    m_IsDynamic = isDynamic;
    return *this;
}

ActorBuilder& ActorBuilder::GenerateOverlaps(bool generateOverlaps)
{
    m_GenerateOverlaps = generateOverlaps;
    return *this;
}

ActorBuilder& ActorBuilder::AutoActivate(bool autoActivate)
{
    m_AutoActivate = autoActivate;
    return *this;
}

ActorBuilder& ActorBuilder::WithColor(const sf::Color& color)
{
    m_Color = color;
    return *this;
}

ActorBuilder& ActorBuilder::WithOutline(const sf::Color& outlineColor, float thickness)
{
    m_OutlineColor = outlineColor;
    m_OutlineThickness = thickness;
    m_HasOutline = true;
    return *this;
}

ActorBuilder& ActorBuilder::WithTexture(const std::string& texturePath)
{
    m_TexturePath = texturePath;
    m_HasTexture = true;
    return *this;
}

std::unique_ptr<Actor> ActorBuilder::Build(Application* app, void* worldContext)
{
    // Create the actor using the internal FActorSpawnParam structure
    FActorSpawnParam spawnParam;
    spawnParam.package = app;
    spawnParam.worldContext = static_cast<b2World*>(worldContext);
    spawnParam.name = m_Name;
    spawnParam.shapeType = m_ShapeType;
    spawnParam.bodyType = m_BodyType;
    spawnParam.size = m_Size;
    spawnParam.location = m_Location;
    spawnParam.rotation = m_Rotation;
    spawnParam.isDynamicBody = m_IsDynamic;
    spawnParam.generateOverlaps = m_GenerateOverlaps;
    spawnParam.autoActivate = m_AutoActivate;

    auto actor = std::make_unique<Actor>(spawnParam);

    // Apply visual properties
    if (m_HasOutline)
    {
        actor->GetShape()->setOutlineThickness(m_OutlineThickness);
        actor->GetShape()->setOutlineColor(m_OutlineColor);
    }

    actor->GetShape()->setFillColor(m_Color);

    if (m_HasTexture)
    {
        // Get the asset loader from the application to load textures
        auto& assetLoader = app->GetAssetLoader();
        actor->GetShape()->setTexture(assetLoader.FindTexture(m_TexturePath));
    }

    return actor;
}

std::unique_ptr<Actor> ActorBuilder::CreateBorder(Application* app, void* worldContext, const std::string& name, 
                                                 const sf::Vector2f& size, const sf::Vector2f& location, 
                                                 const sf::Color& color)
{
    return ActorBuilder()
        .WithName(name)
        .WithShape(EActorShapeType::Rectangle)
        .WithBodyType(Eb2ShapeType::Polygon)
        .WithSize(size)
        .WithLocation(location)
        .WithColor(color)
        .WithOutline(sf::Color::Black, -1.0f)
        .Build(app, worldContext);
}

std::unique_ptr<Actor> ActorBuilder::CreateSensor(Application* app, void* worldContext, const std::string& name,
                                                 const sf::Vector2f& size, const sf::Vector2f& location,
                                                 const sf::Color& color)
{
    return ActorBuilder()
        .WithName(name)
        .WithShape(EActorShapeType::Circle)
        .WithBodyType(Eb2ShapeType::Circle)
        .WithSize(size)
        .WithLocation(location)
        .WithColor(color)
        .GenerateOverlaps(true)
        .Build(app, worldContext);
}

std::unique_ptr<Actor> ActorBuilder::CreateBall(Application* app, void* worldContext, const std::string& name,
                                               const sf::Vector2f& size, const sf::Vector2f& location,
                                               const sf::Color& color)
{
    return ActorBuilder()
        .WithName(name)
        .WithShape(EActorShapeType::Circle)
        .WithBodyType(Eb2ShapeType::Circle)
        .WithSize(size)
        .WithLocation(location)
        .WithColor(color)
        .IsDynamic(true)
        .WithTexture(RESOURCES_TEXTURE_BASKETBALL)
        .Build(app, worldContext);
} 