#pragma once

/**
 * @enum FireballState
 * @brief Trạng thái vòng đời của cầu lửa.
 *
 * Flying    → đang bay, quay 4 frame fly
 * Exploding → nổ, phát 3 frame explosion
 * Dead      → hoàn tất, FireballManager sẽ xóa
 */
enum class FireballState {
    Flying,
    Exploding,
    Dead
};