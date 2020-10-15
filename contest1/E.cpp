#include <iostream>
#include <vector>

template<int ALPHABET, char FIRST_CHARACTER>
class SuffTree {
  int temp_pos_, remainder, active_go, active_len, size;

  std::vector<std::string> text;

  struct Node {
    int start, end, str_id;
    Node *suf_link;
    Node *go[ALPHABET];

    Node(int s, int e, int sid) : start(s), end(e), str_id(sid), suf_link(nullptr) {
      for (int i = 0; i < ALPHABET; ++i) {
        go[i] = nullptr;
      }
    }

    ~Node() {
      for (int i = 0; i < ALPHABET; ++i) {
        if (go[i] != nullptr) delete go[i];
      }
    }

    int edge_length(int pos, int sid, int edge_sid) {
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
    return text[sid][active_go] - FIRST_CHARACTER;
  }

  bool GetDown(int sid, Node* temp){
    if (active_len >= temp->edge_length(temp_pos_, sid, temp->str_id)) {
      active_go += temp->edge_length(temp_pos_, sid, temp->str_id);
      active_len -= temp->edge_length(temp_pos_, sid, temp->str_id);
      active_node_ = temp;
      return true;
    }
    return false;
  }

  void Extend(int sid) {
    ++remainder;
    node_to_add_suf_link_ = root_;
    while (remainder > 0) {
      if (active_len == 0) active_go = temp_pos_;
      if (active_node_->go[WhereToGo(sid)] == nullptr) {
        Node *temp_node_ = new Node(temp_pos_, text[sid].length(), sid);
        ++size;
        active_node_->go[WhereToGo(sid)] = temp_node_;
        CreateSufLinkFromPrev(active_node_);
      } else {
        Node *temp_node_ = active_node_->go[WhereToGo(sid)];
        if (GetDown(sid, temp_node_)) continue;
        if (text[temp_node_->str_id][temp_node_->start + active_len] == text[sid][temp_pos_]) {
          ++active_len;
          CreateSufLinkFromPrev(active_node_);
          break;
        }
        Node *new_inner_node_ = new Node(temp_node_->start, temp_node_->start + active_len, temp_node_->str_id);
        active_node_->go[WhereToGo(sid)] = new_inner_node_;
        temp_node_->start += active_len;
        new_inner_node_->go[text[temp_node_->str_id][temp_node_->start] - FIRST_CHARACTER] = temp_node_;
        Node *new_leaf_ = new Node(temp_pos_, text[sid].length(), sid);
        new_inner_node_->go[text[sid][temp_pos_] - FIRST_CHARACTER] = new_leaf_;
        size += 2;
        CreateSufLinkFromPrev(new_inner_node_);
      }
      --remainder;
      if (active_node_ == root_ && active_len > 0) {
        --active_len;
        active_go = temp_pos_ - remainder + 1;
      } else {
        if (active_node_->suf_link != nullptr) {
          active_node_ = active_node_->suf_link;
        } else {
          active_node_ = root_;
        }
      }
    }
  }

  void Init() {
    for (int i = 0; i < text.size(); ++i) {
      node_to_add_suf_link_ = root_;
      active_node_ = root_;
      remainder = 0;
      active_len = 0;
      active_go = 0;
      for (temp_pos_ = 0; temp_pos_ < text[i].length(); ++temp_pos_) {
        Extend(i);
      }
    }
  }

  void dfs(Node *n, int& node_id, int par_node_id) {
    for (int i = 0; i < ALPHABET; ++i) {
      if (n->go[i] != nullptr) {
        ++node_id;
        std::cout << par_node_id << " " << n->go[i]->str_id << " " << n->go[i]->start << " " << std::min(n->go[i]->end, (int) text[n->go[i]->str_id].length()) << '\n';
        dfs(n->go[i], node_id, node_id);
      }
    }
  }


 public:
  SuffTree(std::vector<std::string> v) : text(std::move(v)){
    size = 1;
    root_ = new Node(-1, -1, 0);
    Init();
  }

  void Print() {
    std::cout << size << std::endl;
    int id = 0;
    dfs(root_, id, 0);
  }

  ~SuffTree() {
    delete root_;
  }
};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  std::string s, t;
  getline(std::cin, s);
  getline(std::cin, t);

  t[t.length() - 1] = '_';
  s[s.length() - 1] = '`';

  SuffTree<28, '_'> st = SuffTree<28, '_'>(std::vector<std::string>({s, t}));
  st.Print();
  return 0;
}