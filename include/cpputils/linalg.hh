#pragma once

#include <cstdint>
#include <vector>

#include "debug.hh"

namespace utils {

uint64_t mul8x8gf2 (uint64_t A, uint64_t B) {
    // https://stackoverflow.com/a/55307540

    static const uint64_t ROW = 0x00000000000000FF;  // lowest row
    static const uint64_t COL = 0x0101010101010101;  // rightmost column

    uint64_t C = 0;

    // TODO: according to godbolt, unroll pragma should unroll all of this
    for (int i=0; i<8; ++i) {
        uint64_t p = COL & (A>>i);
        uint64_t r = ROW & (B>>i*8);
        C ^= (p*r);
    }
    return C;
}

uint16_t mul4x4gf2 (uint16_t A, uint16_t B) {
    static const uint16_t ROW = 0x000F;  // lowest row
    static const uint16_t COL = 0x1111;  // rightmost column

    uint16_t C = 0;

    for (int i = 0; i < 4; ++i) {
        uint16_t p = COL & (A>>i);
        uint16_t r = ROW & (B>>i*4);
        C ^= (p*r);
    }
    return C;
}

uint64_t transpose8x8gf2(uint64_t A) {
    uint64_t C = 0;
    for (int i = 0; i < 8; ++i) {
        C |= (A & (0x0101010101010101ull << (i*8))) >> (i*8);
    }
    return C;
}

struct T4x4GF2 {

    static T4x4GF2 mul(T4x4GF2 A, T4x4GF2 B) {
        return T4x4GF2{mul4x4gf2(A.data, B.data)};
    }

    static T4x4GF2 add(T4x4GF2 A, T4x4GF2 B) {
        return T4x4GF2{static_cast<uint16_t>(A.data ^ B.data)};
    }

    static T4x4GF2 zero() {
        return T4x4GF2{};
    }

    static T4x4GF2 all_ones() {
        return T4x4GF2{0xFFFF};
    }

    uint16_t data{0};
};

struct T8x8GF2 {

    static T8x8GF2 mul(T8x8GF2 A, T8x8GF2 B) {
        return T8x8GF2{mul8x8gf2(A.data, B.data)};
    }

    static T8x8GF2 add(T8x8GF2 A, T8x8GF2 B) {
        return T8x8GF2{A.data ^ B.data};
    }

    static T8x8GF2 zero() {
        return T8x8GF2{};
    }

    static T8x8GF2 all_ones() {
        return T8x8GF2{0xFFFFFFFFFFFFFFFFull};
    }

    static T8x8GF2 transpose(T8x8GF2 A) {
        
    }

    uint64_t data{0};  
};

template<class TBlock>
struct TMatrixGF2 {
    // TODO: rows -> blockRows, cols -> blockCols, cut out the unused rightmost and bottommost bits with appropriate masks!!!
    
    TMatrixGF2& operator*=(const TMatrixGF2& other) {
        // inplace multiplication can be only done if `other` is square
        EXPECT(cols == other.rows, Format("Incompatible matrices: lhs.cols = %, rhs.rows = %", cols, other.cols));
        EXPECT(other.IsSquare(), Format("rhs must be square, but the shape is (%, %)", other.rows, other.cols));
        // TODO: the algorithm is incorrect yet! Because rows and cols are about the blocks, not about actual matrix size
        assert (cols == other.rows);
        for (uint64_t i = 0; i < rows; i++) {
            for (uint64_t j = 0; j < cols; j++) {
                TBlock C = TBlock::zero();
                for (uint64_t k = 0; k < cols; k++) {
                    C = TBlock::add(C, TBlock::mul(blocks[i][k], other.blocks[k][j]));
                }
                blocks[i * cols + j] = C;
            }
        }
        return *this;
    }

    friend TMatrixGF2 operator*(const TMatrixGF2& A, const TMatrixGF2& B) {
        
    }

    uint64_t rows;
    uint64_t cols;
    std::vector<TBlock> blocks;
};

using TMatrix64bitGF2 = TMatrixGF2<T8x8GF2>;
using TMatrix16bitGF2 = TMatrixGF2<T4x4GF2>;

}