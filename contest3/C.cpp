#include <iostream>
#include <vector>
#include <unordered_map>

uint32_t Mex(std::vector<bool> &mex){
  uint32_t res = 0;
  while (mex[res]) ++res;
  return res;
}

bool FirstWins(std::vector<uint32_t> &grandy) {
  grandy[0] = 0;
  grandy[2] = 1;
  std::vector<bool> mex(grandy.size());
  for (int i = 3; i < grandy.size(); ++i) {
    mex.assign(i, false);
    for (int j = 1; j <= i; ++j) {
      uint32_t g = grandy[j - 1] ^ grandy[i - j];
      mex[g] = true;
    }
    grandy[i] = Mex(mex);
  }
  return grandy.back() != 0;
}

int main() {
  int n;
  std::cin >> n;
  std::vector<int> start(n - 1, 0);

  std::vector<uint32_t> grandy(n + 1, 0);

  if (FirstWins(grandy)) {
    std::cout << "Schtirlitz\n";

    for (int j = 1; j <= n; ++j) {
      if((grandy[j - 1] ^ grandy[n - j]) == 0) std::cout << j << '\n';
    }

  } else {
    std::cout << "Mueller\n";
  }
  return 0;
}