#include "BCHDecoder.h"
#include "ChaseDecoder.h"

#include <cassert>
#include <iostream>


template <typename T>
std::vector<T> InitData(const std::vector<T>& msg,
                        const std::vector<T>& ecc) {
    auto data = msg;
    data.insert(data.end(), ecc.begin(), ecc.end());
    return data;
}


void TestBCH() {
    BCHDecoder decoder(5, 2);

    const std::vector<uint8_t> message_part = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0},
        error_correction_part = { 1, 1, 1, 0, 1, 1, 0, 1, 0, 0};

    {
        // 0 errors

        auto data = InitData(message_part, error_correction_part);
        decoder.Decode(&data);
        assert(std::vector<uint8_t>(data.begin(), data.begin() + 21) == message_part);
    }

    {
        // 1 error

        auto data = InitData(message_part, error_correction_part);
        data[0] = 0;
        decoder.Decode(&data);
        assert(std::vector<uint8_t>(data.begin(), data.begin() + 21) == message_part);

        data = InitData(message_part, error_correction_part);
        data[5] = 1;
        decoder.Decode(&data);
        assert(std::vector<uint8_t>(data.begin(), data.begin() + 21) == message_part);

        data = InitData(message_part, error_correction_part);
        data[15] = 1;
        decoder.Decode(&data);
        assert(std::vector<uint8_t>(data.begin(), data.begin() + 21) == message_part);

        data = InitData(message_part, error_correction_part);
        data[25] = 0;
        decoder.Decode(&data);
        assert(std::vector<uint8_t>(data.begin(), data.begin() + 21) == message_part);

        data = InitData(message_part, error_correction_part);
        data[30] = 1;
        decoder.Decode(&data);
        assert(std::vector<uint8_t>(data.begin(), data.begin() + 21) == message_part);
    }

    {
        // 2 errors

        auto data = InitData(message_part, error_correction_part);
        data[0] = 0;
        data[1] = 1;
        decoder.Decode(&data);
        assert(std::vector<uint8_t>(data.begin(), data.begin() + 21) == message_part);

        data = InitData(message_part, error_correction_part);
        data[4] = 1;
        data[8] = 1;
        decoder.Decode(&data);
        assert(std::vector<uint8_t>(data.begin(), data.begin() + 21) == message_part);

        data = InitData(message_part, error_correction_part);
        data[16] = 1;
        data[24] = 1;
        decoder.Decode(&data);
        assert(std::vector<uint8_t>(data.begin(), data.begin() + 21) == message_part);

        data = InitData(message_part, error_correction_part);
        data[24] = 1;
        data[28] = 0;
        decoder.Decode(&data);
        assert(std::vector<uint8_t>(data.begin(), data.begin() + 21) == message_part);
    }

    {
        // 3 errors

        auto data = InitData(message_part, error_correction_part);
        data[5] = 1;
        data[8] = 1;
        data[13] = 1;
        try {
            decoder.Decode(&data);
            assert(false);
        } catch (const std::runtime_error& ex) {

        }
    }
}


void TestChase() {
    ChaseDecoder dec(std::make_unique<BCHDecoder>(5, 2));
    const std::vector<float> message_part_approx =
            {5, -1, -2, -1, -1, -2, -1, -1, -2, -1, -1, -2, -1, -3, -2, -1, -1, -2, -1, -1, -2};
    const std::vector<float> ecc_part_approx =
            {5, 3, 4, -2, 6, 4, -2, 3, -2, -2};

    std::vector<uint8_t> expected = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0};

    {
        // 0 errors

        const auto data = InitData(message_part_approx, ecc_part_approx);
        const auto actual = dec.Decode(data);
        assert(actual == expected);
    }

    {
        // 1 error

        auto data = InitData(message_part_approx, ecc_part_approx);
        data[0] = -2;
        auto actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[5] = 3;
        actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[15] = 1;
        actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[25] = -4;
        actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[30] = 5;
        actual = dec.Decode(data);
        assert(actual == expected);
    }


    {
        // 2 errors

        auto data = InitData(message_part_approx, ecc_part_approx);
        data[0] = -1;
        data[1] =  2;
        auto actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[4] = 2;
        data[8] = 1;
        actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[16] = 1;
        data[24] = 3;
        actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[24] = 4;
        data[28] = -3;
        actual = dec.Decode(data);
        assert(actual == expected);
    }

    {
        // 2 error + bad approx

        auto data = InitData(message_part_approx, ecc_part_approx);
        data[0] = -1;
        data[1] =  1;
        data[4] = -0.2;
        auto actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[4] = 2;
        data[8] = 1;
        data[0] = 0.4;
        actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[16] = 1;
        data[24] = 3;
        data[8] = -0.6;
        actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[24] = 4;
        data[28] = -3;
        data[30] = -0.3;
        actual = dec.Decode(data);
        assert(actual == expected);

        data = InitData(message_part_approx, ecc_part_approx);
        data[24] = 1;
        data[28] = -1;
        data[30] = -0.3;
        data[0] = 0.5;
        actual = dec.Decode(data);
        assert(actual == expected);
    }
}


int main() {
    TestBCH();

    TestChase();

    std::cout << "Tests are passed" << std::endl;

    return 0;
}