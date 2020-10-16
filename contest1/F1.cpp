#include <iostream>
#include <vector>
#include <algorithm>
#include <optional>
#include <sstream>

#define ALPHABET 2560

class SuffArray {
 public:
  SuffArray(std::string s1, int div) : s(std::move(s1)), divider(div) {
    length_ = s.length();
    suf_arr_ = std::vector<int>(length_, 0);
    classes_ = std::vector<int>(length_, 0);
    cnt_ = std::vector<int>(ALPHABET, 0);
    lcp_ = std::vector<int>(length_, 0);
    Init();
    CountSuffArr();
    CountLCP();
  }

  std::string GetKStatistic(int64_t k) {
    int64_t temp_stat = 0;
    int min_lcp = 0;
    for (int i = 0; i < length_ - 1; ++i) {
      if (suf_arr_[i] < divider && suf_arr_[i + 1] > divider || suf_arr_[i] > divider && suf_arr_[i + 1] < divider) {
        temp_stat += std::max(lcp_[i] - min_lcp, 0);

        if (k <= temp_stat) {
          int64_t temp_lcp = lcp_[i] - (temp_stat - k);
          std::string ans = "";
          for (int j = suf_arr_[i]; j < suf_arr_[i] + temp_lcp; ++j) {
            ans += s[j];
          }
          return ans;
        } else {
          min_lcp = lcp_[i];
        }
      } else {
        min_lcp = std::min(lcp_[i], min_lcp);
      }
    }
    return "-1";
  }

 private:
  std::string s;
  int length_;
  std::vector<int> suf_arr_;
  std::vector<int> classes_;
  std::vector<int> cnt_;
  std::vector<int> lcp_;
  int tmpClass = 0;
  int divider;

  void Init() {
    for (int i = 0; i < length_; ++i) {
      ++cnt_[s[i]];
    }
    for (int i = 1; i < ALPHABET; ++i) {
      cnt_[i] += cnt_[i - 1];
    }
    for (int i = length_ - 1; i > -1; --i) {
      --cnt_[s[i]];
      suf_arr_[cnt_[s[i]]] = i;
    }

    for (int i = 1; i < length_; ++i) {
      if (s[suf_arr_[i]] != s[suf_arr_[i - 1]]) {
        ++tmpClass;
      }
      classes_[suf_arr_[i]] = tmpClass;
    }
  }

  void CountSuffArr() {
    std::vector<int> help_suf_arr(length_, 0);
    std::vector<int> help_classes(length_, 0);

    for (uint64_t pow = 0; (1u << pow) < length_; ++pow) {
      cnt_.assign(tmpClass + 1, 0);
      int mv = (1u << pow);
      for (int i = 0; i < length_; ++i) { // made sorted by second part
        help_suf_arr[i] = suf_arr_[i] - (1u << pow);
        if (help_suf_arr[i] < 0) help_suf_arr[i] += length_;
      }
      //counting to sort the first part
      for (int i = 0; i < length_; ++i) {
        ++cnt_[classes_[help_suf_arr[i]]];
      }
      for (int i = 1; i < tmpClass + 1; ++i) {
        cnt_[i] += cnt_[i - 1];
      }
      for (int i = length_ - 1; i > -1; --i) {
        --cnt_[classes_[help_suf_arr[i]]];
        suf_arr_[cnt_[classes_[help_suf_arr[i]]]] = help_suf_arr[i];
      }
      help_classes.assign(length_, 0);
      help_classes[suf_arr_[0]] = 0;
      tmpClass = 0;
      for (int i = 1; i < length_; ++i) {
        int secHalf1 = (suf_arr_[i] + (1u << pow)) % length_;
        int secHalf2 = (suf_arr_[i - 1] + (1u << pow)) % length_;
        if (classes_[suf_arr_[i]] != classes_[suf_arr_[i - 1]] || classes_[secHalf1] != classes_[secHalf2])
          ++tmpClass;
        help_classes[suf_arr_[i]] = tmpClass;
      }
      classes_ = help_classes;
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

int main() {
  std::string s, t;
  getline(std::cin, s);
  getline(std::cin, t);
  int64_t k;
  std::cin >> k;
  std::string text = s + (char) 0 + t + (char) 1;
  SuffArray sa(text, s.length());
  std::cout << sa.GetKStatistic(k);

  return 0;
}