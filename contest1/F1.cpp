#include <iostream>
#include <vector>
#include <algorithm>
#include <optional>
#include <sstream>

#define ALPHABET 2560

class SuffArray {
 public:
  SuffArray(std::string &s1) : s(s1),
                               length_(s.length()),
                               suf_arr_(std::vector<int>(length_, 0)),
                               classes_(std::vector<int>(length_, 0)),
                               cnt_( std::vector<int>(ALPHABET, 0)),
                               lcp_(std::vector<int>(length_, 0)){
    Init();
    count();
    CountLCP();
  }

  const std::vector<int> &GetSufArr() const {
    return suf_arr_;
  }
  const std::vector<int> &GetLcp() const {
    return lcp_;
  }

 private:
  std::string s;
  int length_;
  std::vector<int> suf_arr_;
  std::vector<int> classes_;
  std::vector<int> cnt_;
  std::vector<int> lcp_;
  int tmpClass = ALPHABET;
  int divider;

  void Init() {
    std::vector<int> helpSufArr(length_, 0);
    for (int i = 0; i < length_; ++i) {
      classes_[i] = s[i];
      helpSufArr[i] = i;
    }
    countSort(helpSufArr);

    for (int i = 1; i < length_; ++i) {
      if (s[suf_arr_[i]] != s[suf_arr_[i - 1]]) {
        ++tmpClass;
      }
      classes_[suf_arr_[i]] = tmpClass;
    }
  }

  void countSort(std::vector<int> &helpSufArr) {
    for (int i = 0; i < length_; ++i) {
      ++cnt_[classes_[helpSufArr[i]]];
    }
    for (int i = 1; i < tmpClass + 1; ++i) {
      cnt_[i] += cnt_[i - 1];
    }
    for (int i = length_ - 1; i > -1; --i) {
      --cnt_[classes_[helpSufArr[i]]];
      suf_arr_[cnt_[classes_[helpSufArr[i]]]] = helpSufArr[i];
    }
  }

  std::vector<int> GetNewClasses(std::vector<int> &helpSufArr, int pow) {
    std::vector<int> helpClasses(length_, 0);
    countSort(helpSufArr);
    tmpClass = 0;
    for (int i = 1; i < length_; ++i) {
      int secHalf1 = (suf_arr_[i] + (1u << pow)) % length_;
      int secHalf2 = (suf_arr_[i - 1] + (1u << pow)) % length_;
      if (classes_[suf_arr_[i]] != classes_[suf_arr_[i - 1]] || classes_[secHalf1] != classes_[secHalf2])
        ++tmpClass;
      helpClasses[suf_arr_[i]] = tmpClass;
    }
    return helpClasses;
  }

  void count() {
    std::vector<int> helpSufArr(length_, 0);

    for (uint64_t pow = 0; (1u << pow) < length_; ++pow) {
      cnt_.assign(tmpClass + 1, 0);
      for (int i = 0; i < length_; ++i) { // made sorted by second part
        helpSufArr[i] = suf_arr_[i] - (1u << pow);
        if (helpSufArr[i] < 0) helpSufArr[i] += length_;
      }
      classes_ = GetNewClasses(helpSufArr, pow);
    }
  }

  void CountLCP() { //Kasai
    std::vector<int> pos(length_);
    for (int i = 0; i < length_; ++i) {
      pos[suf_arr_[i]] = i;
    }
    int k = 0;
    for (int i = 0; i < length_; ++i) {
      if (k > 0) {
        --k;
      }
      if (pos[i] == length_ - 1) {
        lcp_[pos[i]] = -1;
        k = 0;
      } else {
        int j = suf_arr_[pos[i] + 1];
        while (std::max(i + k, j + k) < length_ && s[i + k] == s[j + k])
          ++k;
        lcp_[pos[i]] = k;
      }
    }
  }
};

std::string GetKStatistic(std::string &s, std::string &t, int64_t k) {
  std::string text = s + (char) 0 + t + (char) 1;
  SuffArray sa(text);
  int divider = s.length();
  int length = text.length();

  std::vector<int> suf_arr = sa.GetSufArr();
  std::vector<int> lcp = sa.GetLcp();

  int64_t temp_stat = 0;
  int min_lcp = 0;
  for (int i = 0; i < length - 1; ++i) {
    if (suf_arr[i] < divider && suf_arr[i + 1] > divider || suf_arr[i] > divider && suf_arr[i + 1] < divider) {
      temp_stat += std::max(lcp[i] - min_lcp, 0);

      if (k <= temp_stat) {
        int64_t temp_lcp = lcp[i] - (temp_stat - k);
        std::string ans = "";
        for (int j = suf_arr[i]; j < suf_arr[i] + temp_lcp; ++j) {
          ans += text[j];
        }
        return ans;
      } else {
        min_lcp = lcp[i];
      }
    } else {
      min_lcp = std::min(lcp[i], min_lcp);
    }
  }
  return "-1";
}


int main() {
  std::string s, t;
  getline(std::cin, s);
  getline(std::cin, t);
  int64_t k;
  std::cin >> k;
  std::cout << GetKStatistic(s, t, k);

  return 0;
}