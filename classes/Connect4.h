#pragma once
#include "Game.h"

class Connect4 : public Game {
public:
    Connect4();
    ~Connect4();
    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool canBitMoveFrom(Bit &bit, BitHolder &src) override; 
	bool canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;

	void        updateAI() override;
    bool        gameHasAI() override { return true; }
    Grid* getGrid() override { return _grid; }
private:
    // Constants for piece types
    static const int EMPTY = 0;
    static const int RED_PIECE = 1;
    static const int YELLOW_PIECE = 3;

    // Player constants
    static const int RED_PLAYER = 0;
    static const int YELLOW_PLAYER = 1;

    Bit*        createPiece(int pieceType);
    Bit *       PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int index ) const;
    Player*     ownerAt(int x, int y ) const;
    int         negamax(std::string& state, int depth, int playerColor, int alpha, int beta);
    int         scoreWindow(int aiPieces, int humanPieces);
    bool        isAIBoardFull(const std::string& state);
    void        scoreMove(const char bit, int& aiPieces, int& humanPieces);
    int         getValidAIMoveForCol(const std::string& state, int col);
    int         evalBoard(const std::string& state);

    // helper functions
    int getNextOpenRow(int col);

    // Board representation
    Grid*        _grid;

    // Game state
    int         _redPieces;
    int         _yellowPieces;

};