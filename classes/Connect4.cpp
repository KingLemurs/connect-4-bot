#include "Connect4.h"
#include "Logger.h"

Connect4::Connect4() : Game() {
    _grid = new Grid(7, 6);
    _redPieces = 0;
    _yellowPieces = 0;
}

Connect4::~Connect4() {
    delete _grid;
}

void Connect4::setUpBoard() {
    setNumberOfPlayers(2);
    setAIPlayer(YELLOW_PLAYER);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _gameOptions.AIMAXDepth = 1;

    // Initialize all squares
    _grid->initializeSquares(80, "square.png");

    // Enable only dark squares and place pieces
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        _grid->setEnabled(x, y, true);
    });

    startGame();
}

Bit* Connect4::createPiece(int pieceType) {
    Bit* bit = new Bit();
    bool isRed = (pieceType == RED_PIECE);
    bit->LoadTextureFromFile(isRed ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(isRed ? RED_PLAYER : YELLOW_PLAYER));
    bit->setGameTag(pieceType);
    return bit;
}

bool Connect4::actionForEmptyHolder(BitHolder &holder) {

    if (holder.getPosition().y > 80) return false;
    if (holder.bit()) return false;

    Bit* piece = createPiece(getCurrentPlayer()->playerNumber() == 0 ? RED_PIECE : YELLOW_PIECE);
    piece->setPosition(holder.getPosition());

    ChessSquare* to = _grid->getSquare(((ChessSquare&) holder).getColumn(), getNextOpenRow(((ChessSquare&) holder).getColumn()));

    piece->moveTo(to->getPosition());
    to->setBit(piece);
    endTurn();

    return true;
}

Player* Connect4::checkForWinner() {
    // check horizontals
    for (int x = 0; x < _gameOptions.rowX; x++) {
        // start at bottom row as is a bit faster
        for (int y = _gameOptions.rowY; y >= 3; y--) {
            Player* first = ownerAt(y,x);

            if (!first) {
                continue;
            }
            
            /*
            if (first == ownerAt(y+1,x)) Logger::GetInstance().LogGameEvent("two");
            if (first == ownerAt(y+2,x)) Logger::GetInstance().LogGameEvent("three");
            if (first == ownerAt(y+3,x)) Logger::GetInstance().LogGameEvent("four??");
            */

            if (first == ownerAt(y-1,x) && 
                first == ownerAt(y-2,x) &&
                first == ownerAt(y-3,x)) {
                Logger::GetInstance().LogGameEvent("win horizontal");
                return first;
            }
        }
    }

    // check verts
    for (int y = 0; y < _gameOptions.rowY; y++) {
        // start at bottom row as is a bit faster
        for (int x = _gameOptions.rowX; x >= 3; x--) {
            Player* first = ownerAt(y,x);

            if (!first) {
                continue;
            }

            if (first == ownerAt(y,x-1) && 
                first == ownerAt(y,x-2) &&
                first == ownerAt(y,x-3)) {
                Logger::GetInstance().LogGameEvent("win vertical");
                return first;
            }
        }
    }

    // check diags
    for (int x = 2; x < _gameOptions.rowX - 1; x++) {
        // start at bottom row as is a bit faster
        for (int y = _gameOptions.rowY - 1; y > 1; y--) {
            Player* first = ownerAt(y,x);

            if (!first) {
                continue;
            }
            bool BL = first == ownerAt(y-1,x-1) && first == ownerAt(y-2,x-2) && first == ownerAt(y-3,x-3);
            bool BR = first == ownerAt(y+1,x-1) && first == ownerAt(y+2,x-2) && first == ownerAt(y+3,x-3);
            bool FL = first == ownerAt(y-1,x+1) && first == ownerAt(y-2,x+2) && first == ownerAt(y-3,x+3);
            bool FR = first == ownerAt(y+1,x+1) && first == ownerAt(y+2,x+2) && first == ownerAt(y+3,x+3);

            if (BL || BR || FL || FR) {
                Logger::GetInstance().LogGameEvent("win diag");
                return first;
            }
        }
    }

    return nullptr;
}

bool Connect4::checkForDraw() {
    for (int x = 0; x < _gameOptions.rowX; x++) {
        if (_grid->getSquare(x, 0)->empty()) {
            return false;
        }
    }

    return true;
}

void Connect4::stopGame() {
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

// check row 0 seperately
int Connect4::getNextOpenRow(int col) {
    // std::string msg = "searching column " + std::to_string(col);
    //Logger::GetInstance().LogGameEvent(msg.c_str());
    for (int i = 0; i < _gameOptions.rowY; i++) {
        if ( !_grid->getSquare(col, i)->empty() ) {
            //msg = std::to_string(i) + ", " +  std::to_string(col);
            //Logger::GetInstance().LogGameEvent(msg.c_str());
            return i - 1 < 0 ? 0: i - 1;
        }
    }
    // Logger::GetInstance().LogGameEvent("NOT FOUND");
    return _gameOptions.rowY - 1;
}

std::string Connect4::initialStateString() {
    return 0;
}

std::string Connect4::stateString() {
    return _grid->getStateString();
}

void Connect4::setStateString(const std::string &s) {
    if (s.length() != 42) return;

    _redPieces = 0;
    _yellowPieces = 0;

    _grid->setStateString(s);

    // Recreate pieces from state
    size_t index = 0;
    _grid->forEachEnabledSquare([&](ChessSquare* square, int x, int y) {
        if (index < s.length()) {
            int pieceType = s[index++] - '0';
            if (pieceType != 0) {
                Bit* piece = createPiece(pieceType);
                piece->setPosition(square->getPosition());
                square->setBit(piece);
                (pieceType == RED_PIECE) ? _redPieces++ : _yellowPieces++;
            }
        }
    });
}

//
// helper function for the winner check
//
Player* Connect4::ownerAt(int index) const
{
    auto square = _grid->getSquare(index % 3, index / 3);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Connect4::ownerAt(int x, int y) const
{
    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

void Connect4::updateAI() {
    int best = -1000;
    ChessSquare* bestMove = nullptr;
    std::string state = stateString();

    // Traverse all cells, evaluate minimax function for all empty cells
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 7 + x;
        // Check if cell is empty
        if (state[index] == '0') {
            // Make the move
            state[index] = '3';
            int moveVal = -negamax(state, 0, HUMAN_PLAYER, -10000, 10000);
            // Undo the move
            state[index] = '0';
            // If the value of the current move is more than the best value, update best
            if (moveVal > best) {
                bestMove = square;
                best = moveVal;
            }
        }
    });

    // Make the best move
    if(bestMove) {
        if (actionForEmptyHolder(*bestMove)) {
        }
    }
}

int Connect4::scoreWindow(int aiPieces, int humanPieces) {
    int score = 0;

    if (aiPieces == 4) {
        score += 10000;
    } else if (aiPieces == 3 && humanPieces == 0) {
        score += 100;
    } else if (aiPieces == 2 && humanPieces == 0) {
        score += 10;
    }

    if (humanPieces == 4) {
        //Logger::GetInstance().LogGameEvent("Threat 4");
        score -= 10000;
    } else if (humanPieces == 3 && aiPieces == 0) {
        //Logger::GetInstance().LogGameEvent("Threat 3");
        score -= 100;
    } else if (humanPieces == 2 && aiPieces == 0) {
        //Logger::GetInstance().LogGameEvent("Threat 2");
        score -= 10;
    }

    return score;
}

bool Connect4::isAIBoardFull(const std::string& state) {
    for (int x = 0; x < _gameOptions.rowX; x++) {
        if (state[x] == '0') {
            return false;
        }
    }

    return true;
}

int Connect4::evalBoard(const std::string& state) {
    int score = 0;
    for (int y = 0; y < _gameOptions.rowY; y++) {
        // start at bottom row as is a bit faster
        for (int x = 0; x < _gameOptions.rowX; x++) {
            int aiPieces = 0;
            int humanPieces = 0;

            for (int off = 0; off < 4; off++) {
                // is valid window
                if (x + 3 >= _gameOptions.rowX) {
                    break;
                }

                if (x + off < _gameOptions.rowX && state[y * 7 + x + off] != '0') {
                    // Logger::GetInstance().LogGameEvent("Found");
                    state[y * 7 + x + off] == '1' ? humanPieces += 1 : aiPieces += 1;
                }
            }

            score += scoreWindow(aiPieces, humanPieces);
            aiPieces = 0; humanPieces = 0;

            for (int off = 0; off < 4; off++) {
                // is valid window
                if (y + 3 >= _gameOptions.rowY) {
                    break;
                }

                if (y + off < _gameOptions.rowY && state[(y + off) * 7 + x] != '0') {
                    // std::string sixSevennnn = "six sevennnn: " + std::to_string(state[(y + off) * 7 + x]);
                    //Logger::GetInstance().LogGameEvent(sixSevennnn.c_str());
                    state[(y + off) * 7 + x] == '1' ? humanPieces += 1 : aiPieces += 1;
                }
            }

            score += scoreWindow(aiPieces, humanPieces);
            aiPieces = 0; humanPieces = 0;

            // TODO ADD DIAG WINDOWS (LEFT N RIGHT)
            /*
            for (int off = 0; off < 4; off++) {
                // is valid window
                if (y + 3 >= _gameOptions.rowY || x + 3 >= _gameOptions.rowX) {
                    break;
                }

                if (state[y * 7 + x + off] != '0') {
                    // Logger::GetInstance().LogGameEvent("Found");
                    state[y * 7 + x + off] == '1' ? humanPieces += 1 : aiPieces += 1;
                }
            }

            */

        }
    }

    return score;
}

int Connect4::negamax(std::string& state, int depth, int pColor, int alpha, int beta) {
    int score = evalBoard(state);

    if (abs(score) >= 10000) {
        Logger::GetInstance().LogGameEvent("HELP ME");
        //Logger::GetInstance().LogGameEvent(std::to_string(score * pColor).c_str());
        return score * pColor;
    }

    // check draw
    if (depth >= _gameOptions.AIMAXDepth || isAIBoardFull(state)) {
        //Logger::GetInstance().LogGameEvent(std::to_string(score * pColor).c_str());
        return score * pColor;
    }

    int bestVal = -1000; // Min value
    for (int i = 0; i < 42; i++) {
            // find empty
            if (state[i] == '0') {
                state[i] = pColor == HUMAN_PLAYER ? '1' : '3';
                bestVal = std::max(bestVal, -negamax(state, depth + 1, -pColor, -beta, -alpha));
                alpha = std::max(alpha, bestVal);

                state[i] = '0';
                
                if (alpha >= beta) {
                    //Logger::GetInstance().LogGameEvent("PRUNE");
                    break;
                }
            }
    }

    return bestVal;
}

bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src) {return false;}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {return false;}