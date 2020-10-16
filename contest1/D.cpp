#include <iostream>
#include <vector>

#define ALPHABET 256

class SuffArray{
 public:
  SuffArray(std::string& s1){
    s = s1 + (char)(0);
    length_ = s.length();
    suf_arr_ = std::vector<int>(length_, 0);
    classes_ = std::vector<int>(length_, 0);
    cnt_ = std::vector<int>(ALPHABET, 0);
    lcp_ = std::vector<int>(length_, 0);
    Init();
    count();
    CountLCP();
  }

  int getAns(){
    int ans = 0;
    for (int i = 0; i < length_; ++i) {
      ans += length_ - 1 - suf_arr_[i];
    }
    for (int i = 0; i < length_ - 1; ++i) {
      ans -= lcp_[i];
    }
    return ans;
  }

 private:
  std::string s;
  int length_;
  std::vector<int> suf_arr_;
  std::vector<int> classes_;
  std::vector<int> cnt_;
  std::vector<int> lcp_;
  int tmpClass = 0;

  void Init() {
    for (int i = 0; i < length_; ++i) {
      ++cnt_[s[i]];
    }
    for (int i = 1; i < ALPHABET; ++i) {
      cnt_[i] += cnt_[i - 1];
    }
    for (int i = 0; i < length_; ++i) {
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

  void count(){
    std::vector<int> helpSufArr(length_, 0);
    std::vector<int> helpClasses(length_, 0);

    for (uint64_t pow = 0; (1u << pow) < length_; ++pow) {
      cnt_.assign(tmpClass + 1, 0);
      int mv = (1u << pow);
      for (int i = 0; i < length_; ++i) { // made sorted by second part
        helpSufArr[i] = suf_arr_[i] - (1u << pow);
        if (helpSufArr[i] < 0) helpSufArr[i] += length_;
      }
      //counting to sort the first part
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
      helpClasses.assign(length_, 0);
      helpClasses[suf_arr_[0]] = 0;
      tmpClass = 0;
      for (int i = 1; i < length_; ++i) {
        int secHalf1 = (suf_arr_[i] + (1u << pow)) % length_;
        int secHalf2 = (suf_arr_[i - 1] + (1u << pow)) % length_;
        if(classes_[suf_arr_[i]] != classes_[suf_arr_[i - 1]] || classes_[secHalf1] != classes_[secHalf2])
          ++tmpClass;
        helpClasses[suf_arr_[i]] = tmpClass;
      }
      classes_ = helpClasses;
    }
  }

  void CountLCP(){ //Kasai
    std::vector<int> pos(length_);
    for (int i = 0; i < length_; ++i) {
      pos[suf_arr_[i]] = i;
    }
    int k = 0;
    for (int i = 0; i < length_; ++i) {
      if(k > 0) {
        --k;
      }
      if(pos[i] == length_ - 1){
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
  std::string text;
  getline(std::cin, text);
  SuffArray sa(text);
  std::cout << sa.getAns();
  return 0;
}