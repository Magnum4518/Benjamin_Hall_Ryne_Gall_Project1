#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
using namespace std;

enum Color { WHITE, BLACK, NONE };

struct Piece {
    char type; // K, Q, R, B, N, P
    Color color;
    bool hasMoved;
    Piece(char t = ' ', Color c = NONE) : type(t), color(c), hasMoved(false) {}
};

class Board {
    vector<vector<Piece>> board;
    pair<int, int> enPassantTarget;

public:
    Board() : board(8, vector<Piece>(8)), enPassantTarget(-1, -1) {
        setupBoard();
    }

    void setupBoard() {
        // Set up pieces
        string backRank = "RNBQKBNR";
        for (int i = 0; i < 8; ++i) {
            board[0][i] = Piece(backRank[i], BLACK);
            board[1][i] = Piece('P', BLACK);
            board[6][i] = Piece('P', WHITE);
            board[7][i] = Piece(backRank[i], WHITE);
        }
    }

    void display() {
        cout << "   A  B  C  D  E  F  G  H\n";
        for (int i = 0; i < 8; ++i) {
            cout << 8 - i << " ";
            for (int j = 0; j < 8; ++j) {
                Piece &p = board[i][j];
                if (p.color == WHITE)
                    cout << "[" << p.type << "]";
                else if (p.color == BLACK)
                    cout << "(" << p.type << ")";
                else
                    cout << " . ";
            }
            cout << " " << 8 - i << "\n";
        }
        cout << "   A  B  C  D  E  F  G  H\n";
    }

    bool isInsideBoard(int x, int y) {
        return x >= 0 && x < 8 && y >= 0 && y < 8;
    }

    bool movePiece(int sx, int sy, int ex, int ey, Color turn) {
        Piece &p = board[sx][sy];
        if (p.color != turn) return false;

        Piece &target = board[ex][ey];
        int dx = ex - sx, dy = ey - sy;

        if (!isInsideBoard(ex, ey)) return false;
        if (target.color == turn) return false;

        // PAWN
        if (p.type == 'P') {
            int dir = (p.color == WHITE) ? -1 : 1;
            if (dy == 0 && target.type == ' ' && dx == dir) {}
            else if (dy == 0 && sx == (p.color == WHITE ? 6 : 1) && dx == 2 * dir && board[sx + dir][sy].type == ' ' && target.type == ' ') {
                enPassantTarget = {sx + dir, sy};
            }
            else if (abs(dy) == 1 && dx == dir && (target.color == (p.color == WHITE ? BLACK : WHITE) || (ex == enPassantTarget.first && ey == enPassantTarget.second))) {
                if (ex == enPassantTarget.first && ey == enPassantTarget.second)
                    board[sx][ey] = Piece(); // capture en passant
            }
            else return false;

            // Promotion
            if (ex == 0 || ex == 7) {
                char promote;
                cout << "Promote to (Q, R, B, N): ";
                cin >> promote;
                p.type = toupper(promote);
            }
        }

        // KNIGHT
        else if (p.type == 'N') {
            if (!(abs(dx) == 2 && abs(dy) == 1) && !(abs(dx) == 1 && abs(dy) == 2)) return false;
        }

        // KING
        else if (p.type == 'K') {
            if (abs(dy) == 2 && dx == 0 && !p.hasMoved) {
                // Castling
                int rookY = (dy == 2) ? 7 : 0;
                int dir = (dy > 0) ? 1 : -1;
                for (int i = sy + dir; i != rookY; i += dir) if (board[sx][i].type != ' ') return false;
                if (board[sx][rookY].type == 'R' && !board[sx][rookY].hasMoved) {
                    board[sx][sy + dir] = board[sx][rookY];
                    board[sx][rookY] = Piece();
                } else return false;
            } else if (abs(dx) > 1 || abs(dy) > 1) return false;
        }

        // ROOK
        else if (p.type == 'R') {
            if (dx != 0 && dy != 0) return false;
            int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
            int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
            for (int x = sx + stepX, y = sy + stepY; x != ex || y != ey; x += stepX, y += stepY) {
                if (board[x][y].type != ' ') return false;
            }
        }

        // BISHOP
        else if (p.type == 'B') {
            if (abs(dx) != abs(dy)) return false;
            int stepX = (dx > 0) ? 1 : -1;
            int stepY = (dy > 0) ? 1 : -1;
            for (int x = sx + stepX, y = sy + stepY; x != ex; x += stepX, y += stepY) {
                if (board[x][y].type != ' ') return false;
            }
        }

        // QUEEN
        else if (p.type == 'Q') {
            if (dx == 0 || dy == 0) {
                int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
                int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
                for (int x = sx + stepX, y = sy + stepY; x != ex || y != ey; x += stepX, y += stepY) {
                    if (board[x][y].type != ' ') return false;
                }
            } else if (abs(dx) == abs(dy)) {
                int stepX = (dx > 0) ? 1 : -1;
                int stepY = (dy > 0) ? 1 : -1;
                for (int x = sx + stepX, y = sy + stepY; x != ex; x += stepX, y += stepY) {
                    if (board[x][y].type != ' ') return false;
                }
            } else return false;
        }

        // Execute move
        board[ex][ey] = p;
        board[ex][ey].hasMoved = true;
        board[sx][sy] = Piece();
        enPassantTarget = {-1, -1};
        return true;
    }
};

int main() {
    Board chessboard;
    Color turn = WHITE;
    string input;

    while (true) {
        chessboard.display();
        cout << (turn == WHITE ? "White" : "Black") << " to move (e.g., A2 A4): ";
        getline(cin, input);
        if (input.length() != 5 || input[2] != ' ') {
            cout << "Invalid input format.\n";
            continue;
        }

        int sx = 8 - (input[1] - '0');
        int sy = toupper(input[0]) - 'A';
        int ex = 8 - (input[4] - '0');
        int ey = toupper(input[3]) - 'A';

        if (!chessboard.movePiece(sx, sy, ex, ey, turn)) {
            cout << "Invalid move, try again.\n";
        } else {
            turn = (turn == WHITE ? BLACK : WHITE);
        }
    }
    return 0;
}
