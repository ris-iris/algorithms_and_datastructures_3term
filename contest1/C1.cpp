#include <iostream>
#include <vector>
#include <queue>

template<int ALPHABET, char FIRST_CHAR>
class Trie {
  struct Node {
    Node *children[ALPHABET];
    char value;
    bool is_terminal;
    std::vector<int> str_id;
    Node *parent;
    Node *suf_link;
    Node *short_suf_link;
    Node *go[ALPHABET];

    Node(char v, bool b, Node *p) {
      value = v;
      is_terminal = b;
      parent = p;
      suf_link = nullptr;
      short_suf_link = nullptr;
      for (int i = 0; i < ALPHABET; ++i) {
        children[i] = nullptr;
        go[i] = nullptr;
      }
    }
    ~Node() {
      for (int i = 0; i < ALPHABET; ++i) {
        if (children[i] != nullptr)
          delete children[i];
      }
    }
  };

  Node *root_;

  void AddString(const std::string &s, int num) {
    Node *cur = root_;
    for (int i = 0; i < s.length(); ++i) {
      char curChar = s[i];
      if (cur->children[curChar - FIRST_CHAR] == nullptr) {
        cur->children[curChar - FIRST_CHAR] = new Node(curChar, false, cur);
      }
      cur = cur->children[curChar - FIRST_CHAR];
    }
    cur->is_terminal = true;
    cur->str_id.push_back(num);
  }

  Node *GetSufLink(Node *n) {
    if (n->suf_link == nullptr) {
      if (n == root_ || n->parent == root_) {
        n->suf_link = root_;
      } else {
        n->suf_link = GetGo(GetSufLink(n->parent), n->value);
      }
    }
    return n->suf_link;
  }

  Node *GetGo(Node *n, char c) {
    if (n->go[c - FIRST_CHAR] == nullptr) {
      if (n->children[c - FIRST_CHAR] != nullptr) {
        n->go[c - FIRST_CHAR] = n->children[c - FIRST_CHAR];
      } else if (n == root_) {
        n->go[c - FIRST_CHAR] = root_;
      } else {
        n->go[c - FIRST_CHAR] = GetGo(GetSufLink(n), c);
      }
    }
    return n->go[c - FIRST_CHAR];
  }

  Node *GetShortSufLink(Node *n) {
    if (n->short_suf_link == nullptr) {
      Node *sufLink = GetSufLink(n);
      if (sufLink->is_terminal) {
        n->short_suf_link = sufLink;
      } else if (sufLink == root_) {
        n->short_suf_link = root_;
      } else {
        n->short_suf_link = GetShortSufLink(sufLink);
      }
    }
    return n->short_suf_link;
  }

 public:
  Trie(std::vector<std::string> patterns) {
    root_ = new Node(0, false, nullptr);
    for (int i = 0; i < patterns.size(); ++i) {
      AddString(patterns[i], i);
    }
  }

  ~Trie() {
    delete root_;
  }

  template<int A, char F>
  friend
  class Iter;
};

template<int ALPHABET, char FIRST_CHAR>
class Iter {
  Trie<ALPHABET, FIRST_CHAR>* temp_trie_;
  typename Trie<ALPHABET, FIRST_CHAR>::Node *temp_node_;
 public:
  Iter(Iter const &iter) {
    temp_trie_ = iter.temp_trie_;
    temp_node_ = iter.temp_node_;
  }

  Iter(Trie<ALPHABET, FIRST_CHAR>* t) {
    temp_trie_ = t;
    temp_node_ = t->root_;
  }

  void Go(char c) {
    temp_node_ = temp_trie_->GetGo(temp_node_, c);
  }

  bool IsTerminal() {
    return temp_node_->is_terminal;
  }

  auto PatternsBegin() {
    return temp_node_->str_id.begin();
  }

  auto PatternsEnd() {
    return temp_node_->str_id.end();
  }

  void Next() {
    temp_node_ = temp_trie_->GetShortSufLink(temp_node_);
  }

  bool IsFinal() {
    return temp_node_ == temp_trie_->root_;
  }

  ~Iter() {}

};

template<int ALPHABET, char FIRST_CHAR>
void FindPatterns(Trie<ALPHABET, FIRST_CHAR> *trie,
                  std::vector<int> &c,
                  const std::vector<int> &pattern_pos,
                  const std::string &text) {
  Iter<ALPHABET, FIRST_CHAR> cur = Iter<ALPHABET, FIRST_CHAR>(trie);
  Iter<ALPHABET, FIRST_CHAR> next_terminal = Iter<ALPHABET, FIRST_CHAR>(cur);
  for (int i = 0; i < text.length(); ++i) {
    cur.Go(text[i]);
    next_terminal = cur;
    do {
      if (next_terminal.IsTerminal()) {
        for (auto num = next_terminal.PatternsBegin(); num != next_terminal.PatternsEnd(); ++num) {
          int startInd = i - pattern_pos[*num];
          if (startInd > -1 && startInd + pattern_pos[pattern_pos.size() - 1] <= text.length()) {
            ++c[startInd];
          }
        }
      }
      next_terminal.Next();
    } while (!next_terminal.IsFinal());
  }
}

void Separate(std::string s, std::vector<std::string> &ans, std::vector<int> &ansPos) {
  s = "?" + s;
  int n = -1;
  for (int i = 0; i < s.length(); ++i) {
    while (s[i] == '?') {
      ++i;
    }

    if (i == s.length())
      break;

    if (s[i - 1] == '?') {
      ans.push_back("");
      ++n;
    }
    ans[n] += s[i];
    if (i == s.length() - 1 || s[i + 1] == '?')
      ansPos.push_back(i - 1);
  }
  ansPos.push_back(s.length() - 1);
}

int main(int argc, char *argv[]) {
  std::string pat, text;
  std::getline(std::cin, pat);
  std::getline(std::cin, text);

  std::vector<std::string> patterns;
  std::vector<int> pattern_positions;
  Separate(pat, patterns, pattern_positions);
  int patterns_number = patterns.size();

  Trie<26, 'a'> b(patterns);
  std::vector<int> c(text.length(), 0);
  FindPatterns(&b, c, pattern_positions, text);

  if (patterns_number == 0) {
    for (int i = 0; i < text.length() - pat.size() + 1; ++i) {
      std::cout << i << " ";
    }
  } else {
    for (int i = 0; i < text.length(); ++i) {
      if (c[i] == patterns_number) std::cout << i << " ";
    }
  }

  return 0;
}