#pragma once

#include <vector>


typedef std::vector<uint8_t> bits_array;


class IHardDecoder {
public:
    // Decode received bits array inplace
    virtual void Decode(bits_array* received_bits) = 0;

    virtual int GetCorrectedErrorsNumber() const = 0;

    virtual int GetInfoSize() const = 0;

    virtual ~IHardDecoder() = default;
};


class ISoftDecoder {
public:
    // Decode received approximate bit values and return the result
    virtual bits_array Decode(const std::vector<float>& received_bits_approx) = 0;

    virtual ~ISoftDecoder() = default;
};
