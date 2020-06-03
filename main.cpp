#include "BCHDecoder.h"
#include "ChaseDecoder.h"

#include <fstream>

std::vector<float> ReceiveApproximateData() {
    std::ifstream fin("input_file.txt");
    std::vector<float> data(255);
    for (int i = 0; i < 255; ++i)
        fin >> data[i];
    return data;
}

void SaveDecodedMessage(const bits_array& message) {
    std::ofstream fout("output_file.txt");
    for (auto& bit_val : message)
        fout << bit_val + '0';
}

int main() {
    ChaseDecoder decoder(std::make_unique<BCHDecoder>(8, 2));

    const auto data = ReceiveApproximateData();
    const auto message = decoder.Decode(data);
    SaveDecodedMessage(message);

    return 0;
}