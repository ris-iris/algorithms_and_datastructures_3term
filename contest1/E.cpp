#include <iostream>
#include <vector>

class SuffTree {
  int ALPHABET;
  char FIRST_CHARACTER;
  int temp_pos_;
  int remainder_;
  int active_go_;
  int active_len_;
  int size_;
  std::vector<std::string> text_;

  struct Node {
    int start, end, str_id;
    Node *suf_link;
    std::vector<Node *>go;

    Node(int s, int e, int sid, int alph_size) : start(s), end(e), str_id(sid), suf_link(nullptr) {
      for (int i = 0; i < alph_size; ++i) {
        go.push_back(nullptr);
      }
    }

    ~Node() {
      for (int i = 0; i < go.size(); ++i) {
        if (go[i] != nullptr) delete go[i];
      }
    }

    int EdgeLength(int pos, int sid, int edge_sid) {
      if (sid == edge_sid) {
        return std::min(end, pos + 1) - start;
      } else {
        return end - start;
      }
    }
  };

  Node *root_;
  Node *node_to_add_suf_link_;
  Node *active_node_;

  void CreateSufLinkFromPrev(Node *temp) {
    if (node_to_add_suf_link_ != nullptr && node_to_add_suf_link_ != root_) node_to_add_suf_link_->suf_link = temp;
    node_to_add_suf_link_ = temp;
  }

  int WhereToGo(int sid) {
    return text_[sid][active_go_] - FIRST_CHARACTER;
  }

  bool GetDown(int sid, Node* temp){
    if (active_len_ >= temp->EdgeLength(temp_pos_, sid, temp->str_id)) {
      active_go_ += temp->EdgeLength(temp_pos_, sid, temp->str_id);
      active_len_ -= temp->EdgeLength(temp_pos_, sid, temp->str_id);
      active_node_ = temp;
      return true;
    }
    return false;
  }

  void NewLeaf(int sid){
    Node *temp_node_ = new Node(temp_pos_, text_[sid].length(), sid, ALPHABET);
    ++size_;
    active_node_->go[WhereToGo(sid)] = temp_node_;
    CreateSufLinkFromPrev(active_node_);
  }

  void NewInner(int sid, Node *temp_node_){
    Node *new_inner_node_ = new Node(temp_node_->start, temp_node_->start + active_len_, temp_node_->str_id, ALPHABET);
    active_node_->go[WhereToGo(sid)] = new_inner_node_;
    temp_node_->start += active_len_;
    new_inner_node_->go[text_[temp_node_->str_id][temp_node_->start] - FIRST_CHARACTER] = temp_node_;
    Node *new_leaf_ = new Node(temp_pos_, text_[sid].length(), sid, ALPHABET);
    new_inner_node_->go[text_[sid][temp_pos_] - FIRST_CHARACTER] = new_leaf_;
    size_ += 2;
    CreateSufLinkFromPrev(new_inner_node_);
  }

  void FixParamInLoop(){
    --remainder_;
    if (active_node_ == root_ && active_len_ > 0) {
      --active_len_;
      active_go_ = temp_pos_ - remainder_ + 1;
    } else {
      if (active_node_->suf_link != nullptr) {
        active_node_ = active_node_->suf_link;
      } else {
        active_node_ = root_;
      }
    }
  }

  void Extend(int sid) {
    ++remainder_;
    node_to_add_suf_link_ = root_;
    while (remainder_ > 0) {
      if (active_len_ == 0) active_go_ = temp_pos_;
      if (active_node_->go[WhereToGo(sid)] == nullptr) {
        NewLeaf(sid);
      } else {
        Node *temp_node_ = active_node_->go[WhereToGo(sid)];
        if (GetDown(sid, temp_node_)) continue;
        if (text_[temp_node_->str_id][temp_node_->start + active_len_] == text_[sid][temp_pos_]) {
          ++active_len_;
          CreateSufLinkFromPrev(active_node_);
          break;
        }
        NewInner(sid, temp_node_);
      }
      FixParamInLoop();
    }
  }

  void Init() {
    for (int i = 0; i < text_.size(); ++i) {
      node_to_add_suf_link_ = root_;
      active_node_ = root_;
      remainder_ = 0;
      active_len_ = 0;
      active_go_ = 0;
      for (temp_pos_ = 0; temp_pos_ < text_[i].length(); ++temp_pos_) {
        Extend(i);
      }
    }
  }

  void dfs(Node *n, int& node_id, int par_node_id) {
    for (int i = 0; i < ALPHABET; ++i) {
      if (n->go[i] != nullptr) {
        ++node_id;
        std::cout << par_node_id << " " << n->go[i]->str_id << " " << n->go[i]->start << " " << std::min(n->go[i]->end, (int) text_[n->go[i]->str_id].length()) << '\n';
        dfs(n->go[i], node_id, node_id);
      }
    }
  }


 public:
  SuffTree(const std::vector<std::string>& v, int alph_size, char fc) : text_(v), ALPHABET(alph_size), FIRST_CHARACTER(fc){
    size_ = 1;
    root_ = new Node(-1, -1, 0, ALPHABET);
    Init();
  }

  void Print() {
    std::cout << size_ << std::endl;
    int id = 0;
    dfs(root_, id, 0);
  }

  ~SuffTree() {
    delete root_;
  }
};

int main() {
  char delimiter1 =  '_';
  char delimiter2 =  '`';
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  std::string s, t;
  getline(std::cin, s);
  getline(std::cin, t);

  t[t.length() - 1] = delimiter1;
  s[s.length() - 1] = delimiter2;

  SuffTree st(std::vector<std::string>({s, t}), 28, '_');
  st.Print();
  return 0;
}