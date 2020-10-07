#include <iostream>
#include <vector>

std::vector<int> z(const std::string& s, int patLength){
  std::vector<int> z(patLength, 0);
  std::vector<int> ans(0);
  int textLength = s.length();
  int l = 0;
  int r = 0;
  for (int i = 0; i < patLength; ++i){
    if(i <= r)
      z[i] = std::min(r - i + 1, z[i-l]);
    while (i + z[i] < patLength && s[z[i]] == s[z[i] + i])
      ++z[i];
    if(i + z[i] - 1 > r){
      l = i;
      r = i + z[i] - 1;
    }
  }
  int nextZ = 0;
  for (int i = patLength; i < textLength; ++i) {
    nextZ = 0;
    if(i <= r)
      nextZ = std::min(r - i + 1, z[i-l]);
    while (i + nextZ < textLength && s[nextZ] == s[nextZ + i])
      ++nextZ;
    if(i + nextZ - 1 > r){
      l = i;
      r = i + nextZ - 1;
    }
    if (nextZ == patLength)
      ans.push_back(i);
  }
  return ans;
}

int main(){
  std::string pattern, text;
  std::getline(std::cin, pattern);
  std::getline(std::cin, text);
  std::vector<int> ans = std::move(z(pattern+"@"+text, pattern.length()));
  for(auto v = ans.begin(); v != ans.end() ;++v){
    std::cout << (*v) - pattern.length() - 1 << " ";
  }
  return 0;
}