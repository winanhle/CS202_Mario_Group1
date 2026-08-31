#pragma once

#include "MapData.h"

/**
 * @struct TileEdit
 * @brief Represents one atomic tile change.
 *
 * Used as the Command in the map editor undo/redo stack.
 * Each editTile() call creates one TileEdit and pushes it onto m_undoStack.
 *
 * OOP: Command Pattern — encapsulates the "what changed" so it can be
 *      reversed (undo) or re-applied (redo) without re-running game logic.
 *
 * Fields:
 *   gx/gy    — grid coordinates of the changed tile
 *   oldType  — logical TileType before the edit (for undo logic)
 *   newType  — logical TileType after the edit  (for redo logic)
 *   oldGid   — raw GID before the edit (for visual restoration on undo)
 *   newGid   — raw GID after  the edit (for visual restoration on redo)
 */
struct TileEdit
{
    int      gx      = 0;
    int      gy      = 0;
    TileType oldType = TileType::EMPTY;
    TileType newType = TileType::EMPTY;
    int      oldGid  = 0;
    int      newGid  = 0;
};
