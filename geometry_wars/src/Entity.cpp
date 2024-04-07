#include "Entity.h"

Entity::Entity(const size_t id, const std::string& tag)
    : _id(id), _tag(tag)
{
}

bool Entity::isActive() const {
    return _active;
}

const std::string& Entity::tag() const {
    return _tag;    
}

const size_t Entity::id() const {
    return _id;
}

void Entity::destroy() {
    _active = false;
}
