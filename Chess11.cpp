#include <iostream>
#include <vector>

using namespace std;

const int BOARD_SIZE = 8;

// Class representing a chess piece
class Piece {
public:
    char type; // 'P' for Pawn, 'R' for Rook, etc.
    bool isWhite; // true for white, false for black

    Piece(char type, bool isWhite) : type(type), isWhite(isWhite) {}

    virtual bool isValidMove(int startX, int startY, int endX, int endY) = 0; // Pure virtual function for validation
};

// Rook class
class Rook : public Piece {
public:
    Rook(bool isWhite) : Piece('R', isWhite) {}

    bool isValidMove(int startX, int startY, int endX, int endY) override {
        // Rooks move horizontally or vertically
        return (startX == endX || startY == endY);
    }
};

// Pawn class
class Pawn : public Piece {
public:
    Pawn(bool isWhite) : Piece('P', isWhite) {}

    bool isValidMove(int startX, int startY, int endX, int endY) override {
        // Pawns can move forward one square (two squares on their first move)
        int direction = isWhite ? 1 : -1; // White pawns move up, black pawns move down
        if (startY == endY && (endX == startX + direction || (startX == 1 && endX == startX + 2 * direction))) {
            return true;
        }
        return false;
    }
};

// Chessboard class
class Chessboard {
public:
    Piece* board[BOARD_SIZE][BOARD_SIZE]; // Board is a 2D array of Piece pointers

    Chessboard() {
        // Initialize the board
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                board[i][j] = nullptr;
            }
        }

        // Set up the board with pieces
        for (int i = 0; i < BOARD_SIZE; ++i) {
            board[1][i] = new Pawn(true);   // White pawns on the second row
            board[6][i] = new Pawn(false);  // Black pawns on the second to last row
        }

        board[0][0] = board[0][7] = new Rook(true);  // White rooks
        board[7][0] = board[7][7] = new Rook(false); // Black rooks
    }

    void display() {
        // Print the board
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] == nullptr) {
                    cout << ". ";
                } else {
                    cout << board[i][j]->type << " ";
                }
            }
            cout << endl;
        }
    }

    bool movePiece(int startX, int startY, int endX, int endY) {
        if (startX < 0 || startX >= BOARD_SIZE || startY < 0 || startY >= BOARD_SIZE || endX < 0 || endX >= BOARD_SIZE || endY < 0 || endY >= BOARD_SIZE)
            return false;

        Piece* piece = board[startX][startY];
        if (piece == nullptr)
            return false;

        if (board[endX][endY] != nullptr && board[endX][endY]->isWhite == piece->isWhite) {
            return false; // Can't capture your own piece
        }

        if (piece->isValidMove(startX, startY, endX, endY)) {
            board[endX][endY] = piece;
            board[startX][startY] = nullptr;
            return true;
        }

        return false;
    }
};

int main() {
    Chessboard chessboard;
    chessboard.display();

    int startX, startY, endX, endY;
    bool whiteTurn = true;

    while (true) {
        cout << (whiteTurn ? "White's turn" : "Black's turn") << endl;
        cout << "Enter move (startX startY endX endY): ";
        cin >> startX >> startY >> endX >> endY;

        // Try to move the piece
        if (chessboard.movePiece(startX, startY, endX, endY)) {
            chessboard.display();
            whiteTurn = !whiteTurn; // Switch turns
        } else {
            cout << "Invalid move, try again." << endl;
        }
    }

    return 0;
}
