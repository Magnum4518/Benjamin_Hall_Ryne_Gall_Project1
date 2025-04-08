#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cstdlib>
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
    pair<int, int> enPassantSquare;  // Track the en passant square (row, col)
    Color enPassantColor;  // The color that can perform the en passant

public:
    Board() : board(8, vector<Piece>(8)) {
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
        cout << "\n\n\n\n   A  B  C  D  E  F  G  H\n";
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

    bool canPieceAttack(int sx, int sy, int ex, int ey, Piece &p) {
        // Check if piece can attack the target position (ex, ey)

        // PAWN
        if (p.type == 'P') {
            int dir = (p.color == WHITE) ? -1 : 1;
            if (abs(sx - ex) == 1 && abs(sy - ey) == 1) {
                if (board[ex][ey].color != p.color && board[ex][ey].type != ' ') {
                    return true; // Pawn can capture diagonally
                }
            }
        }

        // KNIGHT
        else if (p.type == 'N') {
            if ((abs(sx - ex) == 2 && abs(sy - ey) == 1) || (abs(sx - ex) == 1 && abs(sy - ey) == 2)) {
                return true; // Knight's L-shaped move
            }
        }

        // KING
        else if (p.type == 'K') {
            if (abs(sx - ex) <= 1 && abs(sy - ey) <= 1) {
                return true; // King moves one square in any direction
            }
        }

        // ROOK
        else if (p.type == 'R') {
            if (sx != ex && sy != ey) return false;  // Rooks move only in straight lines

            int stepX = (ex > sx) ? 1 : (ex < sx) ? -1 : 0;
            int stepY = (ey > sy) ? 1 : (ey < sy) ? -1 : 0;

            for (int x = sx + stepX, y = sy + stepY; x != ex || y != ey; x += stepX, y += stepY) {
                if (board[x][y].type != ' ') return false;  // Blocked by another piece
            }
            return true;
        }

        // BISHOP
        else if (p.type == 'B') {
            if (abs(sx - ex) != abs(sy - ey)) return false;  // Bishops move diagonally

            int stepX = (ex > sx) ? 1 : (ex < sx) ? -1 : 0;
            int stepY = (ey > sy) ? 1 : (ey < sy) ? -1 : 0;

            for (int x = sx + stepX, y = sy + stepY; x != ex || y != ey; x += stepX, y += stepY) {
                if (board[x][y].type != ' ') return false;  // Blocked by another piece
            }
            return true;
        }

        // QUEEN
        else if (p.type == 'Q') {
            if (sx == ex || sy == ey) {
                // Horizontal or vertical movement like rook
                int stepX = (ex > sx) ? 1 : (ex < sx) ? -1 : 0;
                int stepY = (ey > sy) ? 1 : (ey < sy) ? -1 : 0;

                for (int x = sx + stepX, y = sy + stepY; x != ex || y != ey; x += stepX, y += stepY) {
                    if (board[x][y].type != ' ') return false;  // Blocked by another piece
                }
                return true;
            } else if (abs(sx - ex) == abs(sy - ey)) {
                // Diagonal movement like bishop
                int stepX = (ex > sx) ? 1 : (ex < sx) ? -1 : 0;
                int stepY = (ey > sy) ? 1 : (ey < sy) ? -1 : 0;

                for (int x = sx + stepX, y = sy + stepY; x != ex || y != ey; x += stepX, y += stepY) {
                    if (board[x][y].type != ' ') return false;  // Blocked by another piece
                }
                return true;
            }
        }

        return false; // Default return value
    }

    bool isCheck(Color turn) {
        // Find the current player's king
        pair<int, int> kingPos;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (board[i][j].type == 'K' && board[i][j].color == turn) {
                    kingPos = {i, j};
                    break;
                }
            }
        }

        // Check if any opposing piece can attack the king
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                Piece &p = board[i][j];
                if (p.color == (turn == WHITE ? BLACK : WHITE)) {
                    // Try every possible move of the piece and check if it can reach the king
                    if (canPieceAttack(i, j, kingPos.first, kingPos.second, p)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool movePiece(int sx, int sy, int ex, int ey, Color turn) {
        Piece &p = board[sx][sy];
        if (p.color != turn) return false;

        Piece &target = board[ex][ey];
        int dx = ex - sx, dy = ey - sy;

        // En Passant logic: check if en passant capture is possible
        if (p.type == 'P' && abs(sy - ey) == 1 && abs(sx - ex) == 1 && target.type == ' ') {
            // Check if the move is a valid en passant
            if (ex == enPassantSquare.first && ey == enPassantSquare.second &&
                target.color == enPassantColor) {
                // Perform en passant capture
                board[ex][ey] = p;  // Move the pawn
                board[sx][sy] = Piece();  // Remove the original pawn
                board[ex - (turn == WHITE ? -1 : 1)][ey] = Piece();  // Remove the captured pawn
                enPassantSquare = {-1, -1};  // Reset en passant
                enPassantColor = NONE;  // Reset en passant color
                return true;  // Successful en passant move
                }
        }

        // Check if the move puts the king in check
        Board tempBoard = *this;
        tempBoard.board[ex][ey] = p;
        tempBoard.board[sx][sy] = Piece();
        if (tempBoard.isCheck(turn)) {
            return false;  // Move would put the king in check
        }
        if (!isInsideBoard(ex, ey)) {
            return false;
        }
        if (target.color == turn) {
            return false;
        }

        // PAWN
        if (p.type == 'P') {
            int dir = (p.color == WHITE) ? -1 : 1;

            // Standard single forward move
            if (dy == 0 && dx == dir && target.type == ' ') {
                board[ex][ey] = p;
                board[sx][sy] = Piece();
            }

            // Double forward move
            else if (dy == 0 && dx == 2 * dir && sx == (p.color == WHITE ? 6 : 1) &&
                     board[sx + dir][sy].type == ' ' && target.type == ' ') {
                board[ex][ey] = p;
                board[sx][sy] = Piece();
            }

            // Standard diagonal capture
            else if (abs(dy) == 1 && dx == dir && target.color == (p.color == WHITE ? BLACK : WHITE)) {
                board[ex][ey] = p;
                board[sx][sy] = Piece();
            }

            else {
                return false;  // Invalid pawn move
            }

            // Promotion
            if (ex == 0 || ex == 7) {
                char promote;
                cout << "Promote to (Q, R, B, N): ";
                cin >> promote;
                board[ex][ey].type = toupper(promote);
            }

            board[ex][ey].hasMoved = true;
            return true;
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
        return true;
    }

    bool isCheckmate(Color turn) {
        if (!isCheck(turn)) return false;

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                Piece &p = board[i][j];
                if (p.color == turn) {
                    for (int x = 0; x < 8; ++x) {
                        for (int y = 0; y < 8; ++y) {
                            // Skip if trying to move to same square
                            if (i == x && j == y) continue;

                            // Create a temporary board
                            Board temp = *this;

                            // Make the move on the temp board
                            if (temp.movePiece(i, j, x, y, turn)) {
                                // If king is safe after this move, not checkmate
                                if (!temp.isCheck(turn)) {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }

        // No legal moves found that stop check
        return true;
    }

    bool isStalemate(Color turn) {
        if (isCheck(turn)) return false;

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                Piece &p = board[i][j];
                if (p.color == turn) {
                    for (int x = 0; x < 8; ++x) {
                        for (int y = 0; y < 8; ++y) {
                            // Skip if trying to move to same square
                            if (i == x && j == y) continue;

                            // Create a temporary board
                            Board temp = *this;

                            // Make the move on the temp board
                            if (temp.movePiece(i, j, x, y, turn)) {
                                // If king is safe after this move, not stalemate
                                if (!temp.isCheck(turn)) {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }

        // No legal moves left
        return true;
    }
};

int main() {

    Board chessboard;
    Color turn = WHITE;
    string input;

    while (true) {
        chessboard.display();
        if (chessboard.isCheckmate(turn))
        {
            cout << "C H E C K M A T E " << (turn == WHITE ? "...Black" : "...White") << " wins.\n";
            break;
        }
        if (chessboard.isStalemate(turn))
        {
            cout << "S T A L E M A T E ...Game over.\n";
            break;
        }
        cout << (turn == WHITE ? "[ ] White" : "( ) Black") << " to move (e.g., E2 E4): ";
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
