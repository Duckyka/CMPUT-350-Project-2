#include <fstream>

#include "CameraSystem.h"
#include "CollisionComponent.h"
#include "CollisionLateSystem.h"
#include "CollisionSystem.h"
#include "ECSEngine.h"
#include "EntityManager.h"
#include "GravitySystem.h"
#include "InputComponent.h"
#include "InputSystem.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "MovementSystem.h"
#include "ScoreComponent.h"
#include "ScoreSystem.h"
#include "SpawnComponent.h"
#include "SpawnSystem.h"
#include "SpriteComponent.h"
#include "SpriteManager.h"
#include "SpriteSystem.h"

std::string gResourcePath = "../assets/";
#define COMPONENTS                                                                             \
    ECSEngine::LocationComponent, ECSEngine::MovementComponent, ECSEngine::CollisionComponent, \
        ECSEngine::SpriteComponent, ECSEngine::SpawnComponent, ECSEngine::CameraComponent,     \
        ECSEngine::CameraFollower, ECSEngine::InputComponent, ECSEngine::GravityComponent,     \
        ECSEngine::CameraShake, ECSEngine::ScoreComponent, ECSEngine::RespawnComponent
using Engine = ECSEngine::ECSEngine<COMPONENTS>;

/**
 * @brief Loads map data and creates the corresponding ECS entities.
 *
 * Supports both required dictionary formats. Registers tile sprites,
 * creates collision components, and configures star spawners.
 * AI disclosure: ChatGPT-assisted code sections:
 * Lines 40-159: Map loading and entity generation
 * Lines 172-181: Sound registration and map loading
 * Lines 183-199: Player initialization
 * Lines 201-214: Extra collectible star
 * Lines 216-225: Score setup
 * Lines 227-236: Camera setup
 * The prompt and evaluation are documented in ABOUT.TXT.
 */
void LoadMap(Engine& engine, const std::string& path,  
             const std::string& file) {
    std::ifstream input(path + file);
    if (!input)
        throw std::runtime_error("Missing map: " + path + file);

    std::string word;
    int dictionary = 0;
    if (!(input >> word >> dictionary) ||
        word != "dictionary" ||
        (dictionary != 1 && dictionary != 2))
        throw std::runtime_error("Invalid map header: " + file);

    struct Tile {
        ECSEngine::SpriteID sprite;
        ECSEngine::Rect hit;
    };
    std::unordered_map<char, Tile> tiles;

    int originX = 0, originY = 0, tileW = 0, tileH = 0;
    int gridW = 0, gridH = 0;

    while (input >> word) {
        if (word == "map") {
            std::string origin, tile, size;
            input >> origin >> originX >> originY
                  >> tile >> tileW >> tileH
                  >> size >> gridW >> gridH;
            if (!input || origin != "origin" ||
                tile != "tile" || size != "size" ||
                tileW <= 0 || tileH <= 0 ||
                gridW <= 0 || gridH <= 0)
                throw std::runtime_error("Invalid map metadata");
            break;
        }

        if (word.size() != 1)
            throw std::runtime_error("Invalid map symbol");

        std::string image;
        int x, y, w, h, bx=0, by=0, bw=0, bh=0;
        input >> image >> x >> y >> w >> h;
        if (dictionary == 2)
            input >> bx >> by >> bw >> bh;
        if (!input)
            throw std::runtime_error("Invalid dictionary entry");

        auto sprite = engine.GetSpriteManager().RegisterTexture(
            path + image, {{float(x),float(y)},float(w),float(h)}
        );
        tiles.emplace(word[0], Tile{
            sprite, {{float(bx),float(by)},float(bw),float(bh)}
        });
    }

    if (!input || gridW <= 0)
        throw std::runtime_error("Missing map grid");

    std::string row;
    std::getline(input, row);
    auto& entities = engine.GetEntityManager();

    for (int y = 0; y < gridH; ++y) {
        if (!std::getline(input, row))
            throw std::runtime_error("Short map grid");
        if (!row.empty() && row.back() == '\r')
            row.pop_back();
        if (row.size() != static_cast<size_t>(gridW))
            throw std::runtime_error("Invalid map row width");

        for (int x = 0; x < gridW; ++x) {
            if (row[x] == '.') continue;
            auto found = tiles.find(row[x]);
            if (found == tiles.end())
                throw std::runtime_error("Unknown map tile");

            const auto entity =
                entities.CreateEntity(std::string(1,row[x]));
            ECSEngine::Point2D position{
                float(originX+x*tileW),
                float(originY+y*tileH)
            };

            entities.AddComponent(
                entity, ECSEngine::LocationComponent{position});
            entities.AddComponent(
                entity, ECSEngine::SpriteComponent{
                    found->second.sprite,
                    {{0.f,0.f},float(tileW),float(tileH)},true
                });

            if (dictionary == 2 &&
                found->second.hit.width > 0.f &&
                found->second.hit.height > 0.f) {
                ECSEngine::CollisionComponent collision{};
                collision.staticEntity = 1;
                collision.doesDamage = (row[x] == 'x');
                collision.collisionBox = found->second.hit;
                collision.previousCollisionBox = {
                    {position.x + collision.collisionBox.topLeft.x,
                     position.y + collision.collisionBox.topLeft.y},
                    collision.collisionBox.width,
                    collision.collisionBox.height
                };
                entities.AddComponent(entity, collision);
            }

            if (dictionary == 2 && row[x] == 'S') {
                const auto star =
                    engine.GetSpriteManager().RegisterTexture(
                        path + "spritesheet-tiles-default.png",
                        {{640.f,320.f},64.f,64.f});
                entities.AddComponent(
                    entity, ECSEngine::SpawnComponent{
                        entity,"star",star,60,90,100
                    });
            }
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc >= 3 && argv[1] == std::string("-path")) {
        gResourcePath = argv[2];
    }
    std::cout << "Usage: " << argv[0] << " [-path resource_path]\n";
    std::cout << "Using resource path: " << gResourcePath << "\n";

    // Engine test code
    Engine engine(1024, 768, "Test Engine");

    // Load sounds
    auto& sound = engine.GetSoundManager();
    sound.RegisterSound(gResourcePath+"sfx_jump.ogg","jump");
    sound.RegisterSound(gResourcePath+"footstep_grass_003.ogg","land");
    sound.RegisterSound(gResourcePath+"footstep_snow_001.ogg","wall");
    sound.RegisterSound(gResourcePath+"sfx_gem.ogg","star");

    // Load maps
    LoadMap(engine,gResourcePath,"sky.map");
    LoadMap(engine,gResourcePath,"world.map");

    // Place player
    auto& em = engine.GetEntityManager();
    const auto player = em.CreateEntity("player");
    const ECSEngine::Point2D start{800.f,576.f};
    em.AddComponent(player,ECSEngine::LocationComponent{start});
    em.AddComponent(player,ECSEngine::MovementComponent{{0.f,0.f}});
    em.AddComponent(player,ECSEngine::GravityComponent{{0.f,1.25f}});
    em.AddComponent(player,ECSEngine::RespawnComponent{start});
    em.AddComponent(player,ECSEngine::InputComponent{player});

    ECSEngine::CollisionComponent playerHit{};
    playerHit.collisionBox={{12.f,6.f},40.f,58.f};
    playerHit.previousCollisionBox={{start.x+12.f,start.y+6.f},40.f,58.f};
    em.AddComponent(player,playerHit);

    const auto playerSprite = engine.GetSpriteManager().RegisterTexture(gResourcePath+"spritesheet-characters-default.png", {{0.f,0.f},128.f,128.f});
    em.AddComponent(player,ECSEngine::SpriteComponent{playerSprite,{{0.f,0.f},64.f,64.f},true});

    // Extra collectible beyond the first major wall
    const auto starSprite = engine.GetSpriteManager().RegisterTexture(
        gResourcePath + "spritesheet-tiles-default.png",
        {{640.f, 320.f}, 64.f, 64.f}
    );
    const auto extraStar=em.CreateEntity("star");
    em.AddComponent(extraStar, ECSEngine::LocationComponent{{2050.f,125.f}});
    em.AddComponent(extraStar, ECSEngine::MovementComponent{{0.f,0.f}});

    ECSEngine::CollisionComponent starHit{};
    starHit.collisionBox={{0.f,0.f},64.f,64.f};
    starHit.previousCollisionBox={{2050.f,125.f},64.f,64.f};
    em.AddComponent(extraStar,starHit);
    em.AddComponent(extraStar,ECSEngine::SpriteComponent{starSprite,{{0.f,0.f},64.f,64.f},true});

    // Setup score component
    ECSEngine::ScoreComponent score{};
    for (int digit=0; digit<10; ++digit) score.digitTextureID[digit] = engine.GetSpriteManager().RegisterTexture(gResourcePath+"spritesheet-tiles-default.png", {{832.f,float((13-digit)*64)},64.f,64.f});
    for (int i=0; i<3; ++i) {
        const auto id=em.CreateEntity("score digit"); 
        score.digitID[2-i]=id;
        em.AddComponent(id,ECSEngine::SpriteComponent{score.digitTextureID[0], {{16.f+float(i)*32.f,16.f},32.f,32.f},false});
    }
    const auto scoreEntity=em.CreateEntity("score");
    em.AddComponent(scoreEntity,score);

    // Setup Camera
    const auto cameraId=em.CreateEntity("camera");
    ECSEngine::CameraComponent camera{};
    camera.position={800.f,384.f};
    camera.worldUnitsPerPixel=1.f;
    camera.speed=0.15f;
    em.AddComponent(cameraId,camera);
    em.AddComponent(cameraId,ECSEngine::CameraFollower{player});
    em.AddComponent(cameraId,ECSEngine::CameraShake{0,{0.f,0.f}});
    engine.GetWindowManager().SetWorldCenter(camera.position);

    // Add all systems
    auto& sm = engine.GetSystemManager();
    sm.AddSystem(std::make_unique<ECSEngine::CollisionLateSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::InputSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::GravitySystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::MovementSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::CollisionSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::ScoreSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::CameraSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::SpawnSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::SpriteSystem<COMPONENTS>>());

    engine.Run();
    return 0;
}
