#include "ChaseDecoder.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <utility>
#include <limits>


ChaseDecoder::ChaseDecoder(std::unique_ptr<IHardDecoder> hard_decoder) :
    hard_decoder_(std::move(hard_decoder)) {}


std::vector<uint8_t> ChaseDecoder::Decode(const std::vector<float>& received_bits_approx) {
    bits_array demodulated_bits(received_bits_approx.size());
    std::transform(received_bits_approx.begin(), received_bits_approx.end(),
            demodulated_bits.begin(), [] (float val) {
        return static_cast<uint8_t>(val > 0);
    });

    const int error_correcting_number = hard_decoder_->GetCorrectedErrorsNumber();

    const std::vector<int> weakest_positions = GetWeakestPositions(received_bits_approx,
                                                                   error_correcting_number);

    bits_array current_best;
    double best_distance = std::numeric_limits<double>::max();

    for (uint8_t subset = 0; subset < (1 << error_correcting_number); ++subset) {
        bits_array probable_bits = GetProbableInput(demodulated_bits, weakest_positions, subset);

        try {
            hard_decoder_->Decode(&probable_bits);
        } catch (const std::runtime_error& ex) {
            continue;
        }

        double dist = CalculateDistanceToReceived(received_bits_approx, probable_bits);

        if (dist < best_distance) {
            best_distance = dist;
            current_best = probable_bits;
        }
    }

    return bits_array(current_best.begin(), current_best.begin() + hard_decoder_->GetInfoSize());
}


std::vector<uint8_t> ChaseDecoder::GetSubsetFromInteger(uint8_t subset, uint8_t size) const {
    bits_array res(size);

    for (uint8_t i = 0; i < size; ++i) {
        res[i] = subset & (1 << (size - i - 1));
    }

    return res;
}

std::vector<int> ChaseDecoder::GetWeakestPositions(const std::vector<float>& received_bits_approx, int num) const {
    std::priority_queue<std::pair<float, size_t>> heap;
    for (int i = 0; i < received_bits_approx.size(); ++i) {
        if (heap.size() < num) {
            heap.push({fabs(received_bits_approx[i]), i});
        } else if (heap.top().first > fabs(received_bits_approx[i])) {
            heap.pop();
            heap.push({fabs(received_bits_approx[i]), i});
        }
    }

    std::vector<int> res(num);

    for (int i = 0; i < num; ++i) {
        res[i] = heap.top().second;
        heap.pop();
    }

    std::sort(res.begin(), res.end());

    return res;
}

bits_array ChaseDecoder::GetProbableInput(const bits_array& demodulated_bits,
                                          const std::vector<int>& positions,
                                          uint8_t subset) const {
    bits_array data = demodulated_bits;

    for (int i = 0; i < positions.size(); ++i) {
        data[positions[i]] ^=  (subset & (1 << (positions.size() - 1 - i)));
    }

    return data;
}

double ChaseDecoder::CalculateDistanceToReceived(const std::vector<float>& received_bits_approx,
                                                 const bits_array& probable_bits) const {
    double res = 0;

    for (int i = 0; i < received_bits_approx.size(); ++i) {
        double val = probable_bits[i] * 2 - 1;
        res += (received_bits_approx[i] - val) * (received_bits_approx[i] - val);
    }

    return res;
}
