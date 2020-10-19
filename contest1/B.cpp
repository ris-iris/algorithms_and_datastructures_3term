#include <iostream>
#include <vector>
#include <numeric>

template<int is_even>
uint64_t CountPalindromes(const std::string &s) {
  int n = s.length();
  std::vector<int> palindromes(n, 0);
  uint64_t ans = 0;
  int left = 0;
  int right = -1;
  for (int i = 0; i < n; ++i) {
    if (i <= right)
      palindromes[i] = std::min(right - i + 1, palindromes[left + right - i + is_even]);
    while (i + palindromes[i] < n && i - palindromes[i] - is_even > -1
        && s[palindromes[i] + i] == s[i - palindromes[i] - is_even])
      ++palindromes[i];
    if (i + palindromes[i] - 1 > right) {
      left = i - palindromes[i] + (1 - is_even);
      right = i + palindromes[i] - 1;
    }
  }
  ans = std::accumulate(palindromes.begin(), palindromes.end(), (uint64_t) 0);
  return ans;
}

int main() {
  std::string s;
  std::getline(std::cin, s);
  std::cout << CountPalindromes<0>(s) + CountPalindromes<1>(s) - s.length();
  return 0;
}