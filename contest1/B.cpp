#include <iostream>
#include <vector>
#include <numeric>

template <int is_even>
uint64_t count_polindroms(const std::string& s){
  int n = s.length();
  std::vector<int> polindroms(n, 0);
  uint64_t ans = 0;
  int left = 0;
  int right = -1;
  for (int i = 0; i < n; ++i){
    if(i <= right)
      polindroms[i] = std::min(right - i + 1, polindroms[left+right-i+is_even]);
    while (i + polindroms[i] < n && i - polindroms[i] - is_even > -1 && s[polindroms[i] + i] == s[i - polindroms[i] - is_even])
      ++polindroms[i];
    if(i + polindroms[i] - 1 > right){
      left = i - polindroms[i] + (1 - is_even);
      right = i + polindroms[i] - 1;
    }
  }
  ans = std::accumulate(polindroms.begin(), polindroms.end(), (uint64_t)0);
  return ans;
}

int main(){
  std::string s;
  std::getline(std::cin, s);
  std::cout << count_polindroms<0>(s) + count_polindroms<1>(s) - s.length();
  return 0;
}