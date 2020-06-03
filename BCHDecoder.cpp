#include "BCHDecoder.h"

#include <stdexcept>


BCHDecoder::BCHDecoder(int order, int errors_to_correct) :
    bch_(init_bch(order, errors_to_correct, 0)),
    errors_to_correct_(errors_to_correct),
    info_size_((1 << order) - 1 - bch_->ecc_bits) {}


void BCHDecoder::Decode(bits_array* received_bits) {
    std::vector<unsigned int> error_locations(errors_to_correct_);

    int error_number = decodebits_bch(bch_,
                                      received_bits->data(),
                                      received_bits->data() + info_size_,
                                      error_locations.data());

    if (error_number > 0 && error_number <= errors_to_correct_) {
        correctbits_bch(bch_, received_bits->data(), error_locations.data(), error_number);
    } else if (error_number != 0) {
        throw std::runtime_error("Failed to decode BCH");
    }
}


int BCHDecoder::GetCorrectedErrorsNumber() const {
    return bch_->t;
}


int BCHDecoder::GetInfoSize() const {
    return info_size_;
}


BCHDecoder::~BCHDecoder() {
    free_bch(bch_);
}
