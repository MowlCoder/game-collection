#pragma once

#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game {
private:
    sf::RenderWindow _window;
    EntityManager _entityManager;
    sf::Font _font;
    sf::Text _scoreText;
    sf::Text _specialWeaponChargesText;
    PlayerConfig _playerConfig;
    EnemyConfig _enemyConfig;
    BulletConfig _bulletConfig;
    sf::Clock _deltaClock;

    int _score = 0;
    int _specialWeaponCharges = 5;
    const int _pointToSpecialCharge = 5000;
    int _pointsToSpecialChargeLeft = _pointToSpecialCharge;
    int _currentFrame = 0;
    int _lastEnemySpawnTime = 0;

    bool _running = true;
    bool _paused = false;
    bool _enableLifeSpanSystem = true;
    bool _enableSpawnSystem = true;
    bool _enableCollissionSystem = true;
    bool _enableMovementSystem = true;

    std::shared_ptr<Entity> _player;

    void init(const std::string& path);

    void setPaused(bool pause);
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> e);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target);
    void spawnSpecialWeapon();
    void handlePlayerMovement();
    void reflectBorderCollidingEntity(std::shared_ptr<Entity> entity);
    bool isColliding(std::shared_ptr<Entity> lhs, std::shared_ptr<Entity> rhs);
    void destroyEnemy(std::shared_ptr<Entity> enemy);
    void getScoreForKill(std::shared_ptr<Entity> enemy);

    void sMovement();
    void sLifespan();
    void sCollision();
    void sEnemySpawner();
    void sGUI();
    void sRender();
    void sUserInput();
public:
    Game(const std::string& configPath);

    void run();
};
