#pragma once
#include "../interfaces/IMapManager.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SFML/Graphics.hpp>

enum class TileType {
    EMPTY = 0,
    GROUND = 1,
    PIPE = 2,
    BRICK_NORMAL = 3,
    QUESTION_COIN = 4,
    QUESTION_POWERUP = 5,
    MULTI_COIN = 6,
    HIDDEN_BLOCK = 7,
    COIN = 8,
    DEATH_ZONE = 9,
    FLAGPOLE = 10
};

class MapManager : public IMapManager {
private:
    std::vector<std::vector<TileType>> m_mapData;
    int m_tileSize = 16; 
    
    // Hàm nội bộ để nạp file
    bool loadMapCSV(const std::string& filepath);

public:
    MapManager();
    ~MapManager() override = default;

    // Các hàm được override từ IMapManager
    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    
    // Hàm phục vụ xử lý va chạm cho Player/Enemy
    bool isSolid(float x, float y) const override;
    int getTileSize() const override { return m_tileSize; }
    sf::Vector2u getMapPixelSize() const override {
        if (m_mapData.empty() || m_mapData[0].empty()) return sf::Vector2u(0, 0);
        return sf::Vector2u(
            static_cast<unsigned>(m_mapData[0].size()) * m_tileSize,
            static_cast<unsigned>(m_mapData.size()) * m_tileSize
        );
    }
    
    // Hàm mở rộng: Lấy chính xác loại Tile để xử lý đụng đầu
    TileType getTileType(float x, float y) const;
    void setTileType(float x, float y, TileType newType); // Dùng để biến block thành EMPTY khi vỡ
};