#include <iostream>
#include <vector>
#include <queue>


class Trie {
  int ALPHABET;
  char FIRST_CHAR;

  struct Node {
    std::vector<Node*> children;
    char value;
    bool is_terminal;
    std::vector<int> str_id;
    Node *parent;
    Node *suf_link;
    Node *short_suf_link;
    std::vector<Node*> go;

    Node(char v, bool b, Node *p, int alph_size) {
      value = v;
      is_terminal = b;
      parent = p;
      suf_link = nullptr;
      short_suf_link = nullptr;
      for (int i = 0; i < alph_size; ++i) {
        children[i] = nullptr;
        go[i] = nullptr;
      }
    }
    ~Node() {
      for (int i = 0; i < children.size(); ++i) {
        if (children[i] != nullptr)
          delete children[i];
      }
    }
  };

  Node *root_;

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
  void AddString(const std::string &s, int num) {
    Node *cur = root_;
    for (int i = 0; i < s.length(); ++i) {
      char curChar = s[i];
      if (cur->children[curChar - FIRST_CHAR] == nullptr) {
        cur->children[curChar - FIRST_CHAR] = new Node(curChar, false, cur, ALPHABET);
      }
      cur = cur->children[curChar - FIRST_CHAR];
    }
    cur->is_terminal = true;
    cur->str_id.push_back(num);
  }

  Trie(std::vector<std::string> patterns, int alphabet_size, char first_char): ALPHABET(alphabet_size), FIRST_CHAR(first_char){
    root_ = new Node(0, false, nullptr, ALPHABET);
    for (int i = 0; i < patterns.size(); ++i) {
      AddString(patterns[i], i);
    }
  }

  ~Trie() {
    delete root_;
  }

  class Iter {
    Trie* temp_trie_;
    Node *temp_node_;
   public:
    Iter(Iter const &iter) {
      temp_trie_ = iter.temp_trie_;
      temp_node_ = iter.temp_node_;
    }

    Iter(Trie* t) {
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

  Iter root(){
    return Iter(this);
  }
};

void FindPatterns(Trie* trie,
                  std::vector<int> &count_of_entries,
                  const std::vector<int> &pattern_pos,
                  const std::string &text) {
  auto cur = trie->root();
  auto next_terminal = cur;
  for (int i = 0; i < text.length(); ++i) {
    cur.Go(text[i]);
    next_terminal = cur;
    do {
      if (next_terminal.IsTerminal()) {
        for (auto num = next_terminal.PatternsBegin(); num != next_terminal.PatternsEnd(); ++num) {
          int startInd = i - pattern_pos[*num];
          if (startInd > -1 && startInd + pattern_pos[pattern_pos.size() - 1] <= text.length()) {
            ++count_of_entries[startInd];
          }
        }
      }
      next_terminal.Next();
    } while (!next_terminal.IsFinal());
  }
}

void Separate(std::string s, std::vector<std::string> &pattern_str, std::vector<int> &patterns_pos) {
  s = "?" + s;
  int n = -1;
  for (int i = 0; i < s.length(); ++i) {
    while (s[i] == '?') {
      ++i;
    }

    if (i == s.length())
      break;

    if (s[i - 1] == '?') {
      pattern_str.push_back("");
      ++n;
    }
    pattern_str[n] += s[i];
    if (i == s.length() - 1 || s[i + 1] == '?')
      patterns_pos.push_back(i - 1);
  }
  patterns_pos.push_back(s.length() - 1);
}

void Solve(std::string& pat, std::string& text){
  std::vector<std::string> patterns;
  std::vector<int> pattern_positions;
  Separate(pat, patterns, pattern_positions);
  int patterns_number = patterns.size();

  Trie trie(patterns, 26, 'a');
  std::vector<int> count_of_entries(text.length(), 0);
  FindPatterns(&trie, count_of_entries, pattern_positions, text);

  if (patterns_number == 0) {
    for (int i = 0; i < text.length() - pat.size() + 1; ++i) {
      std::cout << i << " ";
    }
  } else {
    for (int i = 0; i < text.length(); ++i) {
      if (count_of_entries[i] == patterns_number) std::cout << i << " ";
    }
  }
}

int main(int argc, char *argv[]) {
  std::string pat, text;
  std::getline(std::cin, pat);
  std::getline(std::cin, text);

  Solve(pat, text);

  return 0;
}