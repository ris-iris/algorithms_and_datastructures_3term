#include <iostream>
#include <vector>
#include <complex>
#include <algorithm>

#define PI 3.14159265358979323846
typedef std::complex<double> number;

uint64_t Normalize(std::vector<number> &a) {
  uint64_t size = a.size();
  uint64_t pow = 1;
  while (pow < size) pow *= 2;
  while (a.size() < pow) {
    a.emplace_back(0);
  }
  return pow;
}

std::vector<number> fft(std::vector<number> &input, bool reverse) {
  int n = Normalize(input);
  if (n == 1) return input;

  std::vector<number> input0(n / 2);
  std::vector<number> input1(n / 2);

  int help_position = 0; // this variable is simple to make copy_if work
  std::copy_if(input.begin(), input.end(), input0.begin(), [&help_position](number x) {
    return help_position++ % 2 == 0;
  });
  std::copy_if(input.begin(), input.end(), input1.begin(), [&help_position](number x) {
    return help_position++ % 2 == 1;
  });

  std::vector<number> output0 = fft(input0, reverse);
  std::vector<number> output1 = fft(input1, reverse);

  std::vector<number> output(n);

  number pow(1), base(std::cos(2 * PI / n), std::sin(2 * PI / n) * (reverse ? -1 : 1));
  for (int i = 0; i < n / 2; ++i) {
    output[i] = output0[i] + pow * output1[i];
    output[i + n / 2] = output0[i] - pow * output1[i];
    pow *= base;
    if (reverse) {
      output[i] /= 2;
      output[i + n / 2] /= 2;
    }
  }

  return output;
}

std::vector<number> FFT(std::vector<number> input) {
  return fft(input, false);
}
std::vector<number> FFTReverse(std::vector<number> input) {
  return fft(input, true);
}

struct WAVHelper{
  struct WAVHeader{
    unsigned char chunk_id[4];
    unsigned int chunk_size;
    unsigned char format[4];
    unsigned char subchunk1_id[4];
    unsigned int subchunk1_size;
    unsigned short audio_format;
    unsigned short num_channels;
    unsigned int sample_rate;
    unsigned int byte_rate;
    unsigned short block_align;
    unsigned short bits_per_sample;
    unsigned char subchunk2_id[4];
    unsigned int data_size;
  };
  WAVHeader header;
  int bytes_per_sample;
  std::vector<number> data;

  uint32_t GetNumber(unsigned char *get_data){
    uint32_t ans = 0;
    for (int k = 1; k <= bytes_per_sample; ++k) {
      ans *= 256;
      ans += get_data[bytes_per_sample - k];
    }
    return ans;
  }

  void ReadWAV(FILE* file){
    fread(&header, sizeof(header), 1, file);
    bytes_per_sample = header.bits_per_sample/8;

    unsigned char get_data[bytes_per_sample];
    for (int i = 0; i < header.data_size; ++i) {
      fread(get_data, bytes_per_sample, 1, file);
      data.emplace_back(GetNumber(get_data));
    }
  }

  void WriteWAV(std::vector<number> input){
    data = input;
  }

  void Print(FILE* file){
    fwrite(&header, sizeof(header), 1, file);
    for (int i = 0; i < header.data_size; i += bytes_per_sample) {
      uint32_t num = data[i/bytes_per_sample].real();
      for (int k = 0; k < bytes_per_sample; ++k) {
        unsigned char a = num % 256;
        fwrite(&a, sizeof(char), 1, file);
        num /= 256;
      }
    }
  }

};

WAVHelper Transformation(WAVHelper& input, int D, FILE *file){
  WAVHelper output = input;
  std::vector<number> res = FFT(input.data);
  for (int i = (100 - D) * res.size() / 100; i < res.size(); ++i) {
    res[i] = 0;
  }
  output.WriteWAV(FFTReverse(res));
  return output;
}

int main() {
  int D = 40; // in per cent

  WAVHelper in_helper;
  FILE *in = fopen("../contest3/example/input.wav", "r");
  FILE *out = fopen("../contest3/example/output.wav", "w");

  in_helper.ReadWAV(in);
  Transformation(in_helper, D, out).Print(out);

  fclose(in);
  fclose(out);
  return 0;
}