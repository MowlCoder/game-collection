#include <iostream>
#include <memory>
#include <fstream>

#include <SFML/Graphics.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

class Entity {
private:
    std::string name;
    std::shared_ptr<sf::Shape> shape;
    sf::Vector2f velocity;

    bool isDraw = true;

    sf::Text text;
public:
    void move(sf::RenderTarget& target) {
        auto localBounds = shape->getLocalBounds();
        auto position = shape->getPosition();
        auto targetSize = target.getSize();

        if (position.y + localBounds.height >= targetSize.y) {
            velocity.y *= -1;
        }

        if (position.y < 0) {
            velocity.y *= -1;
        }

        if (position.x + localBounds.width >= targetSize.x) {
            velocity.x *= -1;
        }

        if (position.x < 0) {
            velocity.x *= -1;
        }

        shape->move(velocity);
        centerText();
    }

    void draw(sf::RenderTarget& target) const {
        if (!isDraw) {
            return;
        }

        target.draw(*shape);
        target.draw(text);
    }

    const sf::Vector2f& getVelocity() const {
        return velocity;
    }

    const sf::FloatRect getLocalBounds() const {
        return shape->getLocalBounds();
    }

    const std::string& getName() const {
        return name;
    }

    bool getIsDraw() const {
        return isDraw;
    }

    void setIsDraw(bool newVal) {
        isDraw = newVal;
    }

    const sf::Color& getColor() const {
        return shape->getFillColor();
    }

    void setColor(const sf::Color& color) {
        shape->setFillColor(color);
    }

    void setPosition(const sf::Vector2f& pos) {
        shape->setPosition(pos);
    }

    void setVelocity(const sf::Vector2f& v) {
        velocity = v;
    }

    void setTextFont(const sf::Font& font) {
        text.setFont(font);
    }

    void setTextFontSize(const unsigned int size) {
        text.setCharacterSize(size);
    }

    void centerText() {
        sf::FloatRect bounds = shape->getGlobalBounds();
        auto shapeCenter = sf::Vector2f(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);

        text.setPosition(shapeCenter);
    }

    void setTextColor(const sf::Color& color) {
        text.setFillColor(color);
    }

    Entity(const std::string& name, const std::shared_ptr<sf::Shape> shape, const sf::Vector2f& velocity)
        : name(name), shape(shape), velocity(velocity)
    {
        text.setString(name);
    }
    ~Entity() {}
};

class Config {
private:
    int windowWidth = 0;
    int windowHeight = 0;

    sf::Font font;
    int fontSize;
    sf::Color fontColor;

    std::vector<Entity> entities;
public:
    void loadFromFile(const std::string& filename) {
        std::ifstream fin(filename);
        if (fin.fail()) {
            std::cerr << "[Error]: Failed to open " << filename << std::endl;
            exit(-1);
        }

        std::string type;

        while (fin >> type) {
            if (type == "Window") {
                fin >> windowWidth;
                fin >> windowHeight;
            } else if (type == "Font") {
                std::string pathToFont;
                int r,g,b;

                fin >> pathToFont;
                fin >> fontSize;
                fin >> r >> g >> b;

                fontColor = sf::Color(r,g,b);

                font.loadFromFile(pathToFont);
            } else if (type == "Circle") {
                std::string name;
                sf::Vector2f startPos;
                sf::Vector2f velocity;
                int r,g,b;
                float radius;

                fin >> name >> startPos.x >> startPos.y;
                fin >> velocity.x >> velocity.y;
                fin >> r >> g >> b >> radius;

                sf::CircleShape circle(radius);
                circle.setFillColor(sf::Color(r,g,b));
                circle.setPosition(startPos);
                
                Entity e(name, std::make_shared<sf::CircleShape>(circle), velocity);

                e.setTextFont(font);
                e.setTextFontSize(fontSize);
                e.setTextColor(fontColor);

                entities.push_back(e);
            } else if (type == "Rectangle") {
                std::string name;
                sf::Vector2f startPos;
                sf::Vector2f velocity;
                int r,g,b;
                sf::Vector2f size;

                fin >> name >> startPos.x >> startPos.y;
                fin >> velocity.x >> velocity.y;
                fin >> r >> g >> b;
                fin >> size.x >> size.y;

                sf::RectangleShape rect(size);
                rect.setFillColor(sf::Color(r,g,b));
                rect.setPosition(startPos);
                
                Entity e(name, std::make_shared<sf::RectangleShape>(rect), velocity);

                e.setTextFont(font);
                e.setTextFontSize(fontSize);
                e.setTextColor(fontColor);

                entities.push_back(e);
            }
        }
    }

    int getWindowWidth() const {
        return windowWidth;
    }

    int getWindowHeight() const {
        return windowHeight;
    }

    std::vector<Entity>& getEntities() {
        return entities;
    }

    Config(const std::string& filename) {
        loadFromFile(filename);
    }
    Config() {}
    ~Config() {}
};

int main(int argc, char* argv[]) {
    Config config("./config.txt");

    sf::RenderWindow window(sf::VideoMode(config.getWindowWidth(), config.getWindowHeight()), "My Game");
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    ImGui::GetStyle().ScaleAllSizes(2.0f);
    float c[3] = { 0.0f, 1.0f, 1.0f };

    int comboCurrentItem = 0;
    auto entities = config.getEntities();
    const char* comboItems[entities.size()];

    for (int i = 0; i < entities.size(); i++) {
        comboItems[i] = entities.at(i).getName().c_str();
    }

    bool drawShape = true;
    float shapeVelocity[2] = {0.0f, 0.0f};
    float shapeColor[3] = {0.0f, 0.0f, 0.0f};

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("Shape Properties");

        ImGui::Combo("Shape", &comboCurrentItem, comboItems, entities.size());
        drawShape = config.getEntities().at(comboCurrentItem).getIsDraw();

        shapeVelocity[0] = config.getEntities().at(comboCurrentItem).getVelocity().x;
        shapeVelocity[1] = config.getEntities().at(comboCurrentItem).getVelocity().y;

        shapeColor[0] = config.getEntities().at(comboCurrentItem).getColor().r / 255.0f;
        shapeColor[1] = config.getEntities().at(comboCurrentItem).getColor().g / 255.0f;
        shapeColor[2] = config.getEntities().at(comboCurrentItem).getColor().b / 255.0f;

        ImGui::Checkbox("Draw Shape", &drawShape);
        ImGui::SliderFloat2("Velocity", shapeVelocity, -5.0f, 5.0f);
        ImGui::ColorEdit3("Color", shapeColor);
        ImGui::End();

        config.getEntities().at(comboCurrentItem).setIsDraw(drawShape);
        config.getEntities().at(comboCurrentItem).setVelocity(
            sf::Vector2f(shapeVelocity[0], shapeVelocity[1])
        );
        config.getEntities().at(comboCurrentItem).setColor(
            sf::Color(
                shapeColor[0] * 255,
                shapeColor[1] * 255,
                shapeColor[2] * 255
            )
        );

        for (auto& entity : config.getEntities()) {
            entity.move(window);
        }

        window.clear(sf::Color::Black);

        for (auto& entity : config.getEntities()) {
            entity.draw(window);
        }

        ImGui::SFML::Render(window);

        window.display();
    }

    return 0;
}