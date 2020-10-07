#include <iostream>
#include <vector>

uint64_t odd(const std::string& s){
  int n = s.length();
  std::vector<int> odd(n, 0);
  uint64_t ans = 0;
  int l = 0;
  int r = -1;
  for (int i = 0; i < n; ++i){
    if(i <= r)
      odd[i] = std::min(r - i + 1, odd[l+r-i]);
    while (i + odd[i] < n && i - odd[i] > -1 && s[i - odd[i]] == s[odd[i] + i])
      ++odd[i];
    if(i + odd[i] - 1 > r){
      l = i - odd[i] + 1;
      r = i + odd[i] - 1;
    }
  }
  for(auto v = odd.begin(); v != odd.end(); ++v){
    ans += *v;
  }
  return ans;
}

uint64_t even(const std::string& s){
  int n = s.length();
  std::vector<int> even(n, 0);
  uint64_t ans = 0;
  int l = 0;
  int r = -1;
  for (int i = 0; i < n; ++i){
    if(i <= r)
      even[i] = std::min(r - i + 1, even[l+r-i+1]);
    while (i + even[i] < n && i - even[i] -1 > -1 && s[even[i] + i] == s[i - even[i] - 1])
      ++even[i];
    if(i + even[i] - 1 > r){
      l = i - even[i];
      r = i + even[i] - 1;
    }
  }
  for(auto v = even.begin(); v != even.end(); ++v){
    ans += *v;
  }
  return ans;
}

int main(){
  std::string s;
  std::getline(std::cin, s);
  std::cout << even(s) + odd(s) - s.length();
  return 0;
}