#include <iostream>
#include <vector>

#define ALPHABET 26

class Bor {
 public:
  Bor(const std::string &m): mask(m) {
    root = new Node(nullptr, 0);
    root->parent = root;
    root->sufLink = root;
    findSubpatterns();
    for (int i = 0; i < subpatternPos.size(); ++i) {
      addString(subpatternPos[i], i);
    }
  }

  std::vector<int> findPatterns(const std::string &text) {
    std::vector<int> maskPos(text.length(), 0);
    Node* curNode = root;
    Node* nextTerminal;
    std::vector<int> ans;

    if (subpatternPos.empty()){
      for (int i = 0; i < text.length() - mask.length() + 1; ++i) {
          ans.push_back(i);
      }
      return ans;
    }

    for (int i = 0; i < text.length(); ++i) {
      curNode = getGo(curNode, text[i]);
      nextTerminal = curNode;
      do {
        if (nextTerminal->isTerminal){
          for (auto num = nextTerminal->patternIndex.begin(); num != nextTerminal->patternIndex.end(); ++num){
            int startInd = i - subpatternPos[*num].second + 1;
            if(startInd > -1 && startInd + mask.length() - 1 < text.length()) {
              ++maskPos[startInd];
            }
          }
        }
        nextTerminal = getSufLink(nextTerminal);
      } while (nextTerminal != root);
    }

    for (int i = 0; i < text.length(); ++i) {
      if (maskPos[i] == subpatternPos.size()){
        ans.push_back(i);
      }
    }
    return ans;
  }

 private:
  struct Node {
    Node *children[ALPHABET];
    Node *go[ALPHABET];
    Node *sufLink;
    Node *parent;
    char value;
    bool isTerminal;
    std::vector<int> patternIndex;

    Node(Node *p, char c) :
        parent(p),
        value(c),
        sufLink(nullptr),
        isTerminal(false) {
      for (int i = 0; i < ALPHABET; ++i) {
        children[i] = nullptr;
        go[i] = nullptr;
      }
    }

    ~Node(){
      for (int i = 0; i < ALPHABET; ++i) {
        if (children[i] != nullptr)
          delete children[i];
      }
    }
  };

  void addString(const std::pair<int, int> &pat, int patNum) {
    Node* curNode = root;
    for (int i = pat.first; i < pat.second; ++i) {
      char curChar = mask[i];
      if (curNode->children[curChar - 'a'] == nullptr){
        Node* newNode = new Node(curNode, curChar);
        curNode->children[curChar - 'a'] = newNode;
      }
      curNode = curNode->children[curChar - 'a'];
    }
    curNode->isTerminal = true;
    curNode->patternIndex.push_back(patNum);
  }

  Node *getSufLink(Node *n) {
    if (n->sufLink == nullptr) {
      if (n->parent == root) n->sufLink = root;
      else n->sufLink = getGo(getSufLink(n->parent), n->value);
    }
    return n->sufLink;
  }

  Node *getGo(Node *n, char c) {
    if (n->go[c - 'a'] == nullptr) {
      if (n->children[c - 'a'] != nullptr)
        n->go[c - 'a'] = n->children[c - 'a'];
      else if (n == root)
        n->go[c - 'a'] = root;
      else
        n->go[c - 'a'] = getGo(getSufLink(n), c);
    }
    return n->go[c - 'a'];
  }

  void findSubpatterns() { //CHECK
    std::pair<int, int> curSubPos;
    int m = mask.length();
    for (int i = 0; i < m; ++i) {
      while (i < m && mask[i] == '?')
        ++i;
      if(i == m)
        break;
      curSubPos.first = i;
      while (i < m && mask[i] != '?')
        ++i;

      curSubPos.second = i;
      subpatternPos.push_back(curSubPos);
    }
  }

  std::string mask;
  Node *root;
  std::vector<std::pair<int, int> > subpatternPos;
};

int main() {
  std::string pattern, text;
  std::cin >> pattern >> text;

  Bor ahoCorasickMachine(pattern);

  std::vector<int> entries = ahoCorasickMachine.findPatterns(text);
  for (int i = 0; i < entries.size(); i++) {
    std::cout << entries[i] << " ";
  }

  return 0;
}