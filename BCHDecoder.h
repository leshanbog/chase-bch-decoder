#pragma once

#include "bch_codec/bch_codec.h"
#include "Decoders.h"


class BCHDecoder : public IHardDecoder {
public:
    BCHDecoder(int order = 8, int errors_to_correct = 2);

    void Decode(bits_array* received_bits) override;

    int GetCorrectedErrorsNumber() const override;

    int GetInfoSize() const override;

    ~BCHDecoder();

private:
    bch_control* bch_;
    const int errors_to_correct_;
    const int info_size_;
};
