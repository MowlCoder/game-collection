#pragma once

#include "Components.h"
#include <memory>
#include <string>

class Entity {
private:
    friend class EntityManager;

    bool _active = true;
    size_t _id = 0;
    std::string _tag = "default";

    Entity(const size_t id, const std::string& tag);
public:
    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape> cShape;
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifespan> cLifespan;

    bool isActive() const;
    const std::string& tag() const;
    const size_t id() const;
    void destroy();
};
