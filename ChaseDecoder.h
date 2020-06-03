#pragma once

#include "BCHDecoder.h"
#include "Decoders.h"

#include <memory>


class ChaseDecoder :public ISoftDecoder {
public:
    ChaseDecoder(std::unique_ptr<IHardDecoder> hard_decoder);

    bits_array Decode(const std::vector<float>& received_bits_approx) override;

private:
    bits_array GetSubsetFromInteger(uint8_t subset, uint8_t size) const;

    std::vector<int> GetWeakestPositions(const std::vector<float>& received_bits_approx, int num) const;

    bits_array GetProbableInput(const bits_array& demodulated_bits,
                                const std::vector<int>& positions,
                                uint8_t subset) const;

    double CalculateDistanceToReceived(const std::vector<float>& received_bits_approx,
                                       const bits_array& probable_bits) const;

private:
    std::unique_ptr<IHardDecoder> hard_decoder_;
};
