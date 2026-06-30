#include "MapManager.h"

MapManager::MapManager() {
}

void MapManager::initialize() {
    // Đảm bảo đường dẫn này trỏ đúng tới nơi bạn lưu file .csv
    if (!loadMapCSV("assets/test_map.csv")) {
        std::cerr << "Loi: Khong the tai map CSV!" << std::endl;
    }
}

bool MapManager::loadMapCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string line;
    m_mapData.clear();

    while (std::getline(file, line)) {
        std::vector<TileType> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            // Ép kiểu dữ liệu chuỗi (String) sang số nguyên (Int) rồi sang Enum
            int tileID = std::stoi(cell);
            row.push_back(static_cast<TileType>(tileID));
        }
        m_mapData.push_back(row);
    }
    
    std::cout << "Tai map thanh cong! Kich thuoc: " 
              << m_mapData[0].size() << "x" << m_mapData.size() << std::endl;
    return true;
}

void MapManager::update(float deltaTime) {
    // Các logic về tile thay đổi theo thời gian (ví dụ: animation nước chảy, gạch nảy lên)
    // Sẽ được lập trình tại đây trong tương lai.
}

void MapManager::render(sf::RenderWindow& window) const {
    // VẼ MOCKUP ĐỂ TEST: Sử dụng khối màu hình chữ nhật thay cho file ảnh thật
    sf::RectangleShape tileShape(sf::Vector2f(m_tileSize, m_tileSize));

    for (size_t y = 0; y < m_mapData.size(); ++y) {
        for (size_t x = 0; x < m_mapData[y].size(); ++x) {
            TileType type = m_mapData[y][x];
            
            if (type == TileType::EMPTY || type == TileType::HIDDEN_BLOCK) {
                continue; // Không vẽ không khí và gạch tàng hình
            }

            tileShape.setPosition(sf::Vector2f(x * m_tileSize, y * m_tileSize));

            // Gán màu để dễ dàng nhận diện bằng mắt thường
            switch (type) {
                case TileType::GROUND: tileShape.setFillColor(sf::Color(139, 69, 19)); break; // Nâu
                case TileType::PIPE: tileShape.setFillColor(sf::Color::Green); break;
                case TileType::BRICK_NORMAL: 
                case TileType::MULTI_COIN: tileShape.setFillColor(sf::Color(205, 133, 63)); break; // Cam nhạt
                case TileType::QUESTION_COIN: 
                case TileType::QUESTION_POWERUP: tileShape.setFillColor(sf::Color::Yellow); break;
                case TileType::COIN: tileShape.setFillColor(sf::Color::White); break;
                case TileType::DEATH_ZONE: tileShape.setFillColor(sf::Color::Red); break;
                case TileType::FLAGPOLE: tileShape.setFillColor(sf::Color::Cyan); break;
                default: tileShape.setFillColor(sf::Color::Magenta); break; // Lỗi ID sẽ ra màu tím
            }

            window.draw(tileShape);
        }
    }
}

TileType MapManager::getTileType(float x, float y) const {
    int gridX = static_cast<int>(x) / m_tileSize;
    int gridY = static_cast<int>(y) / m_tileSize;

    // Chặn lỗi văng game nếu Player rớt ra ngoài giới hạn mảng
    if (gridY < 0 || gridY >= m_mapData.size() || 
        gridX < 0 || gridX >= m_mapData[0].size()) {
        return TileType::GROUND; // Coi như ngoài rìa map là tường cứng
    }

    return m_mapData[gridY][gridX];
}

bool MapManager::isSolid(float x, float y) const {
    TileType type = getTileType(x, y);
    // Danh sách các khối cản đường đi của Player
    return (type == TileType::GROUND || 
            type == TileType::PIPE || 
            type == TileType::BRICK_NORMAL || 
            type == TileType::QUESTION_COIN || 
            type == TileType::QUESTION_POWERUP || 
            type == TileType::MULTI_COIN);
}

void MapManager::setTileType(float x, float y, TileType newType) {
    int gridX = static_cast<int>(x) / m_tileSize;
    int gridY = static_cast<int>(y) / m_tileSize;
    
    if (gridY >= 0 && gridY < m_mapData.size() && 
        gridX >= 0 && gridX < m_mapData[0].size()) {
        m_mapData[gridY][gridX] = newType;
    }
}