#pragma once

#include <cpputils/debug.hh>

#include <cstdint>
#include <vector>

namespace utils {

namespace gf2 {

uint64_t mul8x8 (uint64_t A, uint64_t B) {
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

uint16_t mul4x4 (uint16_t A, uint16_t B) {
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

uint64_t transpose8x8(uint64_t A) {
    uint64_t C = 0;
    for (int i = 0; i < 8; ++i) {
        C |= (A & (0x0101010101010101ull << (i*8))) >> (i*8);
    }
    return C;
}

struct T4x4 {

    static T4x4 mul(T4x4 A, T4x4 B) {
        return T4x4{mul4x4(A.data, B.data)};
    }

    static T4x4 add(T4x4 A, T4x4 B) {
        return T4x4{static_cast<uint16_t>(A.data ^ B.data)};
    }

    static T4x4 zero() {
        return T4x4{};
    }

    static T4x4 all_ones() {
        return T4x4{0xFFFF};
    }

    uint16_t data{0};
};

struct T8x8 {

    static T8x8 mul(T8x8 A, T8x8 B) {
        return T8x8{mul8x8(A.data, B.data)};
    }

    static T8x8 add(T8x8 A, T8x8 B) {
        return T8x8{A.data ^ B.data};
    }

    static T8x8 zero() {
        return T8x8{};
    }

    static T8x8 all_ones() {
        return T8x8{0xFFFFFFFFFFFFFFFFull};
    }

    static T8x8 transpose(T8x8 A) {

    }

    uint64_t data{0};
};

template<class TBlock>
struct TMatrix {
    // TODO: rows -> blockRows, cols -> blockCols, cut out the unused rightmost and bottommost bits with appropriate masks!!!

    TMatrix& operator*=(const TMatrix& other) {
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

    TMatrix& operator^=(const TMatrix& other) {
    }

    // TODO: add only ^, | and &, no other operations are allowed

    friend TMatrix operator*(const TMatrix& A, const TMatrix& B) {
        auto result{A};
        return result *= B;
    }

    uint64_t rows;
    uint64_t cols;
    std::vector<TBlock> blocks;
};

using TMatrix64bit = TMatrix<T8x8>;
using TMatrix16bit = TMatrix<T4x4>;
}  // namespace utils::gf2

}
