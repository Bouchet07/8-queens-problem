#include <cstdint>
#include <iostream>
#include <array>

constexpr uint64_t get_last_zero(const uint64_t x) {
    if (~x == 0) {
        return 64;
    }
    return __builtin_ctzll(~x);
}

constexpr uint64_t set_bit(const uint64_t x, const uint64_t pos) {
    return x | (1ULL << pos);
}

void print_board(uint64_t board) {
    for (int i = 7; i >= 0; i--) {
        for (int j = 0; j < 8; j++) {
            if (board & (1ULL << (i * 8 + j))) {
                std::cout << "Q ";
            } else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

const std::array<uint64_t, 8> Ranks = {
    0xFFULL,        // Rank 1 (A1-H1)
    0xFF00ULL,      // Rank 2 (A2-H2)
    0xFF0000ULL,    // Rank 3 (A3-H3)
    0xFF000000ULL,  // Rank 4 (A4-H4)
    0xFF00000000ULL,// Rank 5 (A5-H5)
    0xFF0000000000ULL, // Rank 6 (A6-H6)
    0xFF000000000000ULL, // Rank 7 (A7-H7)
    0xFF00000000000000ULL // Rank 8 (A8-H8)
};

const std::array<uint64_t, 8> Files = {
    0x0101010101010101ULL, // File A
    0x0202020202020202ULL, // File B
    0x0404040404040404ULL, // File C
    0x0808080808080808ULL, // File D
    0x1010101010101010ULL, // File E
    0x2020202020202020ULL, // File F
    0x4040404040404040ULL, // File G
    0x8080808080808080ULL  // File H
};

const std::array<uint64_t, 15> DiagonalA8H1 = {
    0x1ULL,                // A1
    0x102ULL,              // A2-B1
    0x10204ULL,            // A3-B2-C1
    0x1020408ULL,          // A4-B3-C2-D1
    0x102040810ULL,        // A5-B4-C3-D2-E1
    0x10204081020ULL,      // A6-B5-C4-D3-E2-F1
    0x1020408102040ULL,    // A7-B6-C5-D4-E3-F2-G1
    0x102040810204080ULL,  // A8-B7-C6-D5-E4-F3-G2-H1 (Main diagonal)
    0x204081020408000ULL,  // B8-C7-D6-E5-F4-G3-H2
    0x408102040800000ULL,  // C8-D7-E6-F5-G4-H3
    0x810204080000000ULL,  // D8-E7-F6-G5-H4
    0x1020408000000000ULL, // E8-F7-G6-H5
    0x2040800000000000ULL, // F8-G7-H6
    0x4080000000000000ULL, // G8-H7
    0x8000000000000000ULL  // H8
};

// Corrected diagonal bitboards for A8-H1 diagonals
const std::array<uint64_t, 15> DiagonalA1H8 = {
    0x80ULL,                        // A8
    0x8040ULL,                      // A7-B8
    0x804020ULL,                    // A6-B7-C8
    0x80402010ULL,                  // A5-B6-C7-D8
    0x8040201008ULL,                // A4-B5-C6-D7-E8
    0x804020100804ULL,              // A3-B4-C5-D6-E7-F8
    0x80402010080402ULL,            // A2-B3-C4-D5-E6-F7-G8
    0x8040201008040201ULL,          // A1-B2-C3-D4-E5-F6-G7-H8 (Main diagonal)
    0x4020100804020100ULL,          // B1-C2-D3-E4-F5-G6-H7
    0x2010080402010000ULL,          // C1-D2-E3-F4-G5-H6
    0x1008040201000000ULL,          // D1-E2-F3-G4-H5
    0x804020100000000ULL,           // E1-F2-G3-H4
    0x402010000000000ULL,           // F1-G2-H3
    0x201000000000000ULL,           // G1-H2
    0x100000000000000ULL            // H1
};

// The function to get queen attacks based on position
uint64_t get_queen_attacks(uint64_t queen_pos) {
    uint64_t attacks = 0;

    // Calculate row and column from the queen's position
    uint64_t row = queen_pos / 8;
    uint64_t col = queen_pos % 8;

    // Add rank and file attacks
    attacks |= Ranks[row];
    attacks |= Files[col];

    // Calculate diagonal index
    int diag_a1h8 = static_cast<int>(row) - static_cast<int>(col) + 7; // Shift to fit 0-14 index
    int diag_a8h1 = static_cast<int>(row) + static_cast<int>(col);     // Already fits 0-14 index

    // Add diagonal attacks
    if (diag_a1h8 >= 0 && diag_a1h8 < 15) {
        attacks |= DiagonalA1H8[diag_a1h8];
    }
    if (diag_a8h1 >= 0 && diag_a8h1 < 15) {
        attacks |= DiagonalA8H1[diag_a8h1];
    }

    return attacks;
}

// The function to solve the 8 queens problem
#include <unordered_set>

void solve_queens(uint64_t attacks, uint64_t deep, uint64_t queens, std::unordered_set<uint64_t>& seen) {
    if (deep == 8) {
        if (seen.count(queens) == 0) {
            seen.insert(queens);
            print_board(queens);
        }
        return;
    }
    uint64_t u_limit = 8*(deep+1);
    uint64_t l_limit = 8*deep;
    for (uint64_t pos = l_limit; pos < u_limit; pos++) {
        if (!(attacks & (1ULL << pos))) {
            uint64_t new_attacks = get_queen_attacks(pos);
            solve_queens(attacks | new_attacks, deep + 1, set_bit(queens, pos), seen);
        }
    }
}

int main() {
    std::unordered_set<uint64_t> seen;
    solve_queens(0, 0, 0, seen);
    std::cout << "Total solutions: " << seen.size() << std::endl;
    return 0;
}