#include <cctype>
#include <iostream>

using namespace std;

// We will be representing the position on the chess board using a 12-bitboard strategy

class board_state {
    private: 

    unsigned long long whitePawns = 0ULL;
    unsigned long long whiteKnights = 0ULL;
    unsigned long long whiteBishops = 0ULL;
    unsigned long long whiteRooks = 0ULL;
    unsigned long long whiteQueens = 0ULL;
    unsigned long long whiteKing = 0ULL;
    unsigned long long blackPawns = 0ULL;
    unsigned long long blackKnights = 0ULL;
    unsigned long long blackBishops = 0ULL;
    unsigned long long blackRooks = 0ULL;
    unsigned long long blackQueens = 0ULL;
    unsigned long long blackKing = 0ULL;

    // this method will be called when the user is placing the pieces onto the board during the puzzle. 
    void set_bit(unsigned long long& bitboard, int square_index) {
        bitboard |= (1ULL << square_index);
    }

    // void clear all bitboards -- for when a puzzle is reset 
    void clear_all_bitboards() {
        whitePawns = 0ULL;
        whiteKnights = 0ULL;
        whiteBishops = 0ULL;
        whiteRooks = 0ULL;
        whiteQueens = 0ULL;
        whiteKing = 0ULL;
        blackPawns = 0ULL;
        blackKnights = 0ULL;
        blackBishops = 0ULL;
        blackRooks = 0ULL;
        blackQueens = 0ULL;
        blackKing = 0ULL;
    }

    // this method is a helper that directly checks the bitboards to see if the piece is there
    // we are not modifying any of the bitboards here within this class
    bool is_set(unsigned long long bitboard, int square_index)  const{
        /*
        if ((bitboard >> square_index) & 1ULL == 1ULL) {
            return true;
        }
        else {
            return false; 
        }
        Note: the == takes higher precendence over &, so it would be evaluated first
        */
        return (bitboard >> square_index) & 1ULL;
        // above is a faster way since this returns uint64_t value 1, which is set as true, or 0, if false; 
    }

    void clear_square(int square_index) {
        // 1ULL : (63 zeroes)1
        // left shift it by the square index, lets say it is 3, so we are on d8 or smth
        // make every digit in the bit string equal to its opposite value, so we have all 1s and then a 0 in the 3rd position
        // then we &= every bitboard with this mask, so all the values not in position 3 will retain their original value but position 3 will become 0 
        unsigned long long mask = ~(1ULL << square_index);
        whitePawns &= mask;
        whiteKnights &= mask;
        whiteBishops &= mask;
        whiteRooks &= mask;
        whiteQueens &= mask;
        whiteKing &= mask;
        blackPawns &= mask;
        blackKnights &= mask;
        blackBishops &= mask;
        blackRooks &= mask;
        blackQueens &= mask;
        blackKing &= mask;
    }

    public: 

    board_state() = default; 

    void populate_from_FEN(string& FEN_string) {

        clear_all_bitboards();

        int square_number = 0; 

        for (char c : FEN_string) {

            if (c == ' ') {
                break;
            }

            if (c == '/') {
                continue; 
            }

            // handling the digits representing empty squares. we convert to integer by subtracting ASCII values of both characters. 
            if (isdigit(c)) {
                square_number += (c - '0');
            }

            else {
                // black rook 
                if (c == 'r') {
                    set_bit(blackRooks, square_number);
                    square_number++;
                }
                // black knight
                if (c == 'n') {
                    set_bit(blackKnights, square_number);
                    square_number++;
                }
                // black bishop 
                if (c == 'b') {
                    set_bit(blackBishops, square_number);
                    square_number++;
                }
                // black queen
                if (c == 'q') {
                    set_bit(blackQueens, square_number);
                    square_number++;
                }
                // black king
                if (c == 'k') {
                    set_bit(blackKing, square_number);
                    square_number++;
                }
                // white rook 
                if (c == 'R') {
                    set_bit(whiteRooks, square_number);
                    square_number++;
                }
                // white knight
                if (c == 'N') {
                    set_bit(whiteKnights, square_number);
                    square_number++;
                }
                // white bishop
                if (c == 'B') {
                    set_bit(whiteBishops, square_number);
                    square_number++;
                }
                // white queen 
                if (c == 'Q') {
                    set_bit(whiteQueens, square_number);
                    square_number++;
                }
                // white king 
                if (c == 'K') {
                    set_bit(whiteKing, square_number);
                    square_number++;
                }
                // black pawn 
                if (c == 'p') {
                    set_bit(blackPawns, square_number);
                    square_number++;
                }
                // white pawn 
                if (c == 'P') {
                    set_bit(whitePawns, square_number);
                    square_number++;
                }
            }
        }
    }

    char get_piece_at(uint32_t square_index) const {
        // check black pieces 
        if (is_set(blackRooks, square_index)) return 'r';
        if (is_set(blackKnights, square_index)) return 'n';
        if (is_set(blackBishops, square_index)) return 'b';
        if (is_set(blackQueens, square_index)) return 'q';
        if (is_set(blackKing, square_index)) return 'k';
        if (is_set(blackPawns, square_index)) return 'p';
        
        // check white pieces
        if (is_set(whiteRooks, square_index)) return 'R';
        if (is_set(whiteKnights, square_index)) return 'N';
        if (is_set(whiteBishops, square_index)) return 'B';
        if (is_set(whiteQueens, square_index)) return 'Q';
        if (is_set(whiteKing, square_index)) return 'K';
        if (is_set(whitePawns, square_index)) return 'P';
        
        // empty square
        return ' ';
    }

    // both boards are to remain constant when this operator is used
    // now we can compare two entire chess boards directly
    bool operator==(const board_state&  other) const {
        return (whitePawns == other.whitePawns) &&
        (whiteKnights == other.whiteKnights) &&
        (whiteBishops == other.whiteBishops) &&
        (whiteRooks == other.whiteRooks) &&
        (whiteQueens == other.whiteQueens) &&
        (whiteKing == other.whiteKing) &&
        (blackPawns == other.blackPawns) &&
        (blackKnights == other.blackKnights) &&
        (blackBishops == other.blackBishops) &&
        (blackRooks == other.blackRooks) &&
        (blackQueens == other.blackQueens) &&
        (blackKing == other.blackKing);
    }

    uint32_t how_many_squares_correct(const board_state& target) const {
        int correct = 0;

        for (int i = 0; i < 64; i++) {
            if (get_piece_at(i) == target.get_piece_at(i)) {
                correct += 1;
            }
        }
        return correct; 
    }

    // the purpose of this method is to clear all 12 bit boards and place the piece on the correct one
    // high level view of adding a piece to a square 
    void set_piece_at_square(int square_index, char piece) {
        clear_square(square_index);
        
        switch (piece) {
            case 'P': set_bit(whitePawns, square_index); break;
            case 'N': set_bit(whiteKnights, square_index); break;
            case 'B': set_bit(whiteBishops, square_index); break;
            case 'R': set_bit(whiteRooks, square_index); break;
            case 'Q': set_bit(whiteQueens, square_index); break;
            case 'K': set_bit(whiteKing, square_index); break;
            case 'p': set_bit(blackPawns, square_index); break;
            case 'n': set_bit(blackKnights, square_index); break;
            case 'b': set_bit(blackBishops, square_index); break;
            case 'r': set_bit(blackRooks, square_index); break;
            case 'q': set_bit(blackQueens, square_index); break;
            case 'k': set_bit(blackKing, square_index); break;
            case ' ': break;
        }
    }

};