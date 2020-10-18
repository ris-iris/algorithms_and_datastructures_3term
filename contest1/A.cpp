#include <iostream>
#include <vector>

std::vector<int> z(const std::string &s, int pat_length) {
  std::vector<int> z(pat_length, 0);
  std::vector<int> ans(0);
  int text_length = s.length();
  int left = 0;
  int right = 0;

  int next_z = 0;
  for (int i = 0; i < text_length; ++i) {
    next_z = 0;
    if (i <= right)
      next_z = std::min(right - i + 1, z[i - left]);
    while (i + next_z < ((i < pat_length)?pat_length:text_length) && s[next_z] == s[next_z + i])
      ++next_z;
    if (i + next_z - 1 > right) {
      left = i;
      right = i + next_z - 1;
    }
    if (i < pat_length) {
      z[i] = next_z;
    } else if (next_z == pat_length) {
      ans.push_back(i);
    }
  }
  return ans;
}

int main() {
  std::string pattern, text;
  std::getline(std::cin, pattern);
  std::getline(std::cin, text);
  std::vector<int> ans = std::move(z(pattern + "@" + text, pattern.length()));
  for (auto v : ans) {
    std::cout << v - pattern.length() - 1 << " ";
  }
  return 0;
}