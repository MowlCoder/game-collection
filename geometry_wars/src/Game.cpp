#include "Game.h"
#include "Random.h"

#include <iostream>

Game::Game(const std::string& configPath) {
    init(configPath);
}

void Game::init(const std::string& path) {
    _window.create(sf::VideoMode(1920, 1080), "Geometry wars");
    _window.setFramerateLimit(60);
    
    if (!_font.loadFromFile("./assets/fonts/tech.ttf")) {
        std::cerr << "Can not read font file" << std::endl;
        exit(-1);
    }

    _scoreText.setPosition(10.0f, 10.0f);
    _scoreText.setFont(_font);
    _scoreText.setFillColor(sf::Color(255, 255, 255));
    _scoreText.setCharacterSize(24);

    _specialWeaponChargesText.setPosition(10.0f, 50.0f);
    _specialWeaponChargesText.setFont(_font);
    _specialWeaponChargesText.setFillColor(sf::Color(255, 255, 255));
    _specialWeaponChargesText.setCharacterSize(24);

    ImGui::SFML::Init(_window);

    spawnPlayer();
}

void Game::run() {
    while (_running) {
        _entityManager.update();
        ImGui::SFML::Update(_window, _deltaClock.restart());

        if (!_paused) {
            sEnemySpawner();
            sMovement();
            sCollision();
            sLifespan();
        }

        sUserInput();
        sGUI();
        sRender();

        if (!_paused) {
            _currentFrame++;
        }
    }
}

void Game::setPaused(bool paused) {
    _paused = paused;
}

void Game::spawnPlayer() {
    if (_player == nullptr) {
        auto entity = _entityManager.addEntity("player");
        entity->cTransform = std::make_shared<CTransform>(Vec2(0.0f, 0.0f), Vec2(3.0f, 3.0f), 0.0f);
        entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);
        entity->cInput = std::make_shared<CInput>();
        entity->cCollision = std::make_shared<CCollision>(32.0f);

        _player = entity;
    }

    _player->cTransform->pos.x = _window.getSize().x / 2;
    _player->cTransform->pos.y = _window.getSize().y / 2;
}

void Game::spawnEnemy() {
    int pointCount = Random::getInRange(3, 8);
    auto color = sf::Color(Random::get(255), Random::get(255), Random::get(255));
    int score = pointCount * 100;
    int radius = 32;
    Vec2 position = Vec2(Random::getInRange(radius, _window.getSize().x-radius), Random::getInRange(radius, _window.getSize().y-radius));
    Vec2 velocity = Vec2(Random::getInRange(-5, 5), Random::getInRange(-5, 5));

    auto entity = _entityManager.addEntity("enemy");
    entity->cTransform = std::make_shared<CTransform>(position, velocity, 0.0f);
    entity->cShape = std::make_shared<CShape>(static_cast<float>(radius), pointCount, color, sf::Color(0, 0, 0), 0);
    entity->cScore = std::make_shared<CScore>(score);
    entity->cCollision = std::make_shared<CCollision>(static_cast<float>(radius));
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
    size_t pointCount = e->cShape->circle.getPointCount();
    float degreeInverval = 360 / pointCount;

    for (size_t i = 0; i < pointCount; i++) {
        float radians = (degreeInverval * i * M_PI) / 180;
        Vec2 speed = Vec2(3.0f, 3.0f);
        Vec2 velocity = Vec2(cos(radians), sin(radians)) * speed;

        auto entity = _entityManager.addEntity("enemy_particle");
        entity->cTransform = std::make_shared<CTransform>(e->cTransform->pos, velocity, 0.0f);
        entity->cShape = std::make_shared<CShape>(e->cShape->circle.getRadius() / 3, e->cShape->circle.getPointCount(), e->cShape->circle.getFillColor(), e->cShape->circle.getFillColor(), 0);
        entity->cLifespan = std::make_shared<CLifespan>(30);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target) {
    auto bullet = _entityManager.addEntity("bullet");
    auto bulletSpeed = Vec2(10.0f, 10.0f);
    Vec2 velocity = (target - entity->cTransform->pos).normalize() * bulletSpeed;

    bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, 0.0f);
    bullet->cShape = std::make_shared<CShape>(8.0f, 64, sf::Color(255, 255, 255), sf::Color(255, 255, 255), 0.0f);
    bullet->cLifespan = std::make_shared<CLifespan>(90);
    bullet->cCollision = std::make_shared<CCollision>(8.0f);
}

void Game::spawnSpecialWeapon() {
    if (_specialWeaponCharges == 0) {
        return;
    }

    std::vector<Vec2> directions{
        Vec2(1, 0),
        Vec2(1, 1),
        Vec2(1, -1),
        Vec2(-1, 1),
        Vec2(-1, 0),
        Vec2(-1, -1),
        Vec2(0, 1),
        Vec2(0, -1)
    };

    for (const Vec2& direction : directions) {
        spawnBullet(
            _player,
            (_player->cTransform->pos+direction)
        );
    }

    _specialWeaponCharges -= 1;
}

void Game::handlePlayerMovement() {
    if (_player->cInput->up) {
        _player->cTransform->pos.y -= _player->cTransform->velocity.y;
    }

    if (_player->cInput->down) {
        _player->cTransform->pos.y += _player->cTransform->velocity.y;
    }

    if (_player->cInput->left) {
        _player->cTransform->pos.x -= _player->cTransform->velocity.x;
    }

    if (_player->cInput->right) {
        _player->cTransform->pos.x += _player->cTransform->velocity.x;
    }

    auto pTransform = _player->cTransform;
    auto playerRadius = _player->cShape->circle.getRadius();
    auto borderWidth = _player->cShape->circle.getOutlineThickness();
    auto windowSize = _window.getSize();

    if (pTransform->pos.y - playerRadius - borderWidth < 0) {
        pTransform->pos.y = playerRadius + borderWidth;
    } else if (pTransform->pos.y + playerRadius + borderWidth > windowSize.y) {
        pTransform->pos.y = windowSize.y - playerRadius - borderWidth;
    }

    if (pTransform->pos.x - playerRadius - borderWidth < 0) {
        pTransform->pos.x = playerRadius + borderWidth;
    } else if (pTransform->pos.x + playerRadius + borderWidth > windowSize.x) {
        pTransform->pos.x = windowSize.x - playerRadius - borderWidth;
    }
}

void Game::reflectBorderCollidingEntity(std::shared_ptr<Entity> entity) {
    if (entity->cTransform == nullptr) {
        return;
    }

    auto transform = entity->cTransform;
    auto entityRadius = entity->cShape->circle.getRadius();
    auto borderWidth = entity->cShape->circle.getOutlineThickness();
    auto windowSize = _window.getSize();
    
    if (transform->pos.y - entityRadius - borderWidth < 0) {
        transform->velocity.y *= -1;
    } else if (transform->pos.y + entityRadius + borderWidth > windowSize.y) {
        transform->velocity.y *= -1;
    }

    if (transform->pos.x - entityRadius - borderWidth < 0) {
        transform->velocity.x *= -1;
    } else if (transform->pos.x + entityRadius + borderWidth > windowSize.x) {
        transform->velocity.x *= -1;
    }
}

void Game::sMovement() {
    handlePlayerMovement();

    for (auto entity : _entityManager.getEntities()) {
        if (entity->cInput != nullptr || entity->cTransform == nullptr) {
            continue;
        }

        entity->cTransform->pos.x += entity->cTransform->velocity.x;
        entity->cTransform->pos.y += entity->cTransform->velocity.y;

        if (entity->tag() == "enemy") {
            reflectBorderCollidingEntity(entity);
        }
    }
}

void Game::sLifespan() {
    for (auto entity : _entityManager.getEntities()) {
        if (entity->cLifespan == nullptr) {
            continue;
        }

        entity->cLifespan->remaining -= 1;

        if (entity->cLifespan->remaining <= 0) {
            entity->destroy();
        } else {
            auto fillColor = entity->cShape->circle.getFillColor();
            fillColor.a = 255.0f * ((float)entity->cLifespan->remaining / (float)entity->cLifespan->total);
            entity->cShape->circle.setFillColor(fillColor);
        }
    }
}

bool Game::isColliding(std::shared_ptr<Entity> lhs, std::shared_ptr<Entity> rhs) {
    if (lhs->cCollision == nullptr || rhs->cCollision == nullptr) {
        return false;
    }

    float totalRadiusDistance = lhs->cCollision->radius + rhs->cCollision->radius;
    float currentRadiusDistance = lhs->cTransform->pos.dist(rhs->cTransform->pos);

    return totalRadiusDistance > currentRadiusDistance;
}

void Game::getScoreForKill(std::shared_ptr<Entity> enemy) {
    _score += enemy->cScore->score;
    _pointsToSpecialChargeLeft -= enemy->cScore->score;

    if (_pointsToSpecialChargeLeft <= 0) {
        _pointsToSpecialChargeLeft = _pointToSpecialCharge;
        _specialWeaponCharges += 1;
    }
}

void Game::destroyEnemy(std::shared_ptr<Entity> enemy) {
    spawnSmallEnemies(enemy);
    enemy->destroy();
}

void Game::sCollision() {
    for (auto enemy : _entityManager.getEntities("enemy")) {
        if (isColliding(enemy, _player)) {
            destroyEnemy(enemy);
            _player->destroy();
            spawnPlayer();
            break;
        }

        for (auto bullet : _entityManager.getEntities("bullet")) {
            if (isColliding(enemy, bullet)) {
                getScoreForKill(enemy);
                destroyEnemy(enemy);

                bullet->destroy();
                break;
            }
        }
    }
}

void Game::sEnemySpawner() {
    if (_currentFrame - _lastEnemySpawnTime < 60 * 5) {
        return;
    }

    spawnEnemy();

    _lastEnemySpawnTime = _currentFrame;
}

void Game::sGUI() {
    ImGui::Begin("Geometry Wars");
    ImGui::Checkbox("Paused", &_paused);
    ImGui::End();

    std::string scoreText = "Score: ";
    scoreText.append(std::to_string(_score));
    _scoreText.setString(scoreText);

    std::string specialWeaponChargesText = "Special Weapon: ";
    specialWeaponChargesText.append(std::to_string(_specialWeaponCharges));
    _specialWeaponChargesText.setString(specialWeaponChargesText);

    if (_specialWeaponCharges == 0) {
        _specialWeaponChargesText.setColor(sf::Color(255, 0, 0));
    } else {
        _specialWeaponChargesText.setColor(sf::Color(255, 255, 255));
    }
}

void Game::sRender() {
    _window.clear();

    _player->cShape->circle.setPosition(
        _player->cTransform->pos.x,
        _player->cTransform->pos.y
    );
    _player->cTransform->angle += 1.0f;
    _player->cShape->circle.setRotation(_player->cTransform->angle);

    for (auto entity : _entityManager.getEntities()) {
        if (entity->cShape == nullptr || entity->cTransform == nullptr) {
            continue;
        }

        if (entity->tag() == "enemy") {
            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);
        }

        entity->cShape->circle.setPosition(
            entity->cTransform->pos.x,
            entity->cTransform->pos.y
        );
        _window.draw(entity->cShape->circle);
    }

    _window.draw(_player->cShape->circle);
    _window.draw(_scoreText);
    _window.draw(_specialWeaponChargesText);

    ImGui::SFML::Render(_window);
    
    _window.display();
}

void Game::sUserInput() {
    sf::Event event;

    while(_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(_window, event);

        if (event.type == sf::Event::Closed) {
            _running = false;
        }

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                _player->cInput->up = true;
                break;
            case sf::Keyboard::S:
                _player->cInput->down = true;
                break;
            case sf::Keyboard::D:
                _player->cInput->right = true;
                break;
            case sf::Keyboard::A:
                _player->cInput->left = true;
                break;
            case sf::Keyboard::Escape:
                _running = false;
                break;
            case sf::Keyboard::P:
                setPaused(!_paused);
                break;
            default:
                break;
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                _player->cInput->up = false;
                break;
            case sf::Keyboard::S:
                _player->cInput->down = false;
                break;
            case sf::Keyboard::D:
                _player->cInput->right = false;
                break;
            case sf::Keyboard::A:
                _player->cInput->left = false;
                break;
            case sf::Keyboard::Space:
                if (!_paused) {
                    spawnSpecialWeapon();
                }
                break;
            default:
                break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (ImGui::GetIO().WantCaptureMouse) { continue; }

            if (event.mouseButton.button == sf::Mouse::Left) {
                spawnBullet(_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }
        }
    }
}
