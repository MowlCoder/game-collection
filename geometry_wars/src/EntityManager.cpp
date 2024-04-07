#include "EntityManager.h"

EntityManager::EntityManager() {

}

void EntityManager::update() {
    for (auto& e : _entitiesToAdd) {
        _entities.push_back(e);
        _entityMap[e->tag()].push_back(e);
    }

    _entitiesToAdd.clear();

    removeDeadEntities(_entities);

    for (auto& [tag, entityVec] : _entityMap) {
        removeDeadEntities(entityVec);
    }
}

void EntityManager::removeDeadEntities(EntityVec& vec) {
    auto entitiesToDelete = EntityVec();

    for (auto e : vec) {
        if (!e->isActive()) {
            entitiesToDelete.push_back(e);
        }
    }

    for (auto e : entitiesToDelete) {
        for (int i = 0; i < vec.size(); i++) {
            if (vec.at(i)->id() == e->id()) {
                vec.erase(vec.begin()+i);
            }
        }
    }
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
    auto entity = std::shared_ptr<Entity>(new Entity(++_totalEntities, tag));
    _entitiesToAdd.push_back(entity);

    return entity;
}

const EntityVec& EntityManager::getEntities() {
    return _entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag) {
    return _entityMap[tag];
}

const EntityMap& EntityManager::getEntityMap() {
    return _entityMap;
}
