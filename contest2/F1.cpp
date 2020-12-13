#include <vector>
#include <algorithm>
#include <iostream>
#include <limits>
#include <set>
#include <iomanip>

#define INFTY std::numeric_limits<long double>::max()

enum state {
  FIRST_PART = 0,
  SECOND_PART = 1,
  PREV_FIRST_BRIDGE = 2,
  NEXT_FIRST_BRIDGE = 3,
  PREV_SECOND_BRIDGE = 4,
  NEXT_SECOND_BRIDGE = 5
};

struct Point {
  long double x, y, z;
  Point() = default;
  Point(long double _x, long double _y)
      : x(_x), y(_y), z(_x * _x + _y * _y) {}
};

std::istream &operator>>(std::istream &is, Point &p) {
  is >> p.x >> p.y;
  p.z = p.x * p.x + p.y * p.y;
  return is;
}

struct Node {
  int id;
  Point point;
  Node *prev;
  Node *next;

  Node() = default;
  Node(int _id, Point ph, Node *p, Node *n)
      : id(_id), point(ph), prev(p), next(n) {}

  bool GetLinksValid() {
    if (prev->next == this) {
      prev->next = next;
      next->prev = prev;
      return false;
    } else {
      prev->next = this;
      next->prev = this;
      return true;
    }
  }
};

long double CrossProduct(Point a, Point b, Point c, char mode) {
  if (mode == 'y') {
    return (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
  }
  if (mode == 'z') {
    return ((b.x - a.x) * (c.z - b.z) - (b.z - a.z) * (c.x - b.x));
  }
}

bool IsRightTurn(const Node *a, const Node *b, const Node *c) {
  if (a == nullptr || b == nullptr || c == nullptr) {
    return false;
  }
  return CrossProduct(a->point, b->point, c->point, 'y') < 0;
}

long double TurnTime(Node *a, Node *b, Node *c) {
  if (a == nullptr || b == nullptr || c == nullptr) {
    return INFTY;
  }
  long double help = CrossProduct(a->point, b->point, c->point, 'y');
  return help == 0 ? INFTY : CrossProduct(a->point, b->point, c->point, 'z') / help;
}

std::pair<Node *, Node *> FindBridge(Node *first, Node *second) {
  std::pair<Node *, Node *> bridge(first, second);
  while (true) {
    if (IsRightTurn(bridge.first, bridge.second, bridge.second->next)) {
      bridge.second = bridge.second->next;
    } else if (IsRightTurn(bridge.first->prev, bridge.first, bridge.second)) {
      bridge.first = bridge.first->prev;
    } else {
      break;
    }
  }
  return bridge;
}

void FixLinks(std::pair<Node *, Node *> &bridge, std::vector<Node *> &res, long double divider_x) {
  bridge.first->next = bridge.second;
  bridge.second->prev = bridge.first;
  for (int i = res.size() - 1; i > -1; --i) {
    Node *current = res[i];
    if (current->point.x > bridge.first->point.x && current->point.x < bridge.second->point.x) {
      bridge.first->next = current;
      bridge.second->prev = current;
      current->prev = bridge.first;
      current->next = bridge.second;
      if (current->point.x <= divider_x) {
        bridge.first = current;
      } else {
        bridge.second = current;
      }
    } else {
      current->GetLinksValid();
      if (current == bridge.first) {
        bridge.first = bridge.first->prev;
      }
      if (current == bridge.second) {
        bridge.second = bridge.second->next;
      }
    }
  }
}

std::vector<Node *> MergeHulls(const std::vector<Node *> &first,
                               const std::vector<Node *> &second,
                               Node *first_bridge,
                               Node *second_bridge) {
  std::vector<Node *> res;
  std::pair<Node *, Node *> bridge = move(FindBridge(first_bridge, second_bridge));

  int iter1 = 0, iter2 = 0;
  long double current_time = -INFTY;
  while (true) {
    long double min_time = INFTY;
    state min_state;
    if (iter1 < first.size() && min_time > TurnTime(first[iter1]->prev, first[iter1], first[iter1]->next)
        && TurnTime(first[iter1]->prev, first[iter1], first[iter1]->next) >= current_time) {
      min_time = TurnTime(first[iter1]->prev, first[iter1], first[iter1]->next);
      min_state = FIRST_PART;
    }
    if (iter2 < second.size()
        && min_time > TurnTime(second[iter2]->prev, second[iter2], second[iter2]->next)
        && TurnTime(second[iter2]->prev, second[iter2], second[iter2]->next) >= current_time) {
      min_time = TurnTime(second[iter2]->prev, second[iter2], second[iter2]->next);
      min_state = SECOND_PART;
    }
    if (min_time > TurnTime(bridge.first->prev, bridge.first, bridge.second)
        && TurnTime(bridge.first->prev, bridge.first, bridge.second) > current_time) {
      min_time = TurnTime(bridge.first->prev, bridge.first, bridge.second);
      min_state = PREV_FIRST_BRIDGE;
    }
    if (min_time > TurnTime(bridge.first, bridge.first->next, bridge.second)
        && TurnTime(bridge.first, bridge.first->next, bridge.second) > current_time) {
      min_time = TurnTime(bridge.first, bridge.first->next, bridge.second);
      min_state = NEXT_FIRST_BRIDGE;
    }
    if (min_time > TurnTime(bridge.first, bridge.second->prev, bridge.second)
        && TurnTime(bridge.first, bridge.second->prev, bridge.second) > current_time) {
      min_time = TurnTime(bridge.first, bridge.second->prev, bridge.second);
      min_state = PREV_SECOND_BRIDGE;
    }
    if (min_time > TurnTime(bridge.first, bridge.second, bridge.second->next)
        && TurnTime(bridge.first, bridge.second, bridge.second->next) > current_time) {
      min_time = TurnTime(bridge.first, bridge.second, bridge.second->next);
      min_state = NEXT_SECOND_BRIDGE;
    }

    if (min_time == INFTY) break;

    current_time = min_time;
    switch (min_state) {
      case FIRST_PART:
        if (first[iter1]->point.x < bridge.first->point.x)
          res.push_back(first[iter1]);
        first[iter1]->GetLinksValid();
        ++iter1;
        break;
      case SECOND_PART:
        if (second[iter2]->point.x > bridge.second->point.x)
          res.push_back(second[iter2]);
        second[iter2]->GetLinksValid();
        ++iter2;
        break;
      case PREV_FIRST_BRIDGE:res.push_back(bridge.first);
        bridge.first = bridge.first->prev;
        break;
      case NEXT_FIRST_BRIDGE:bridge.first = bridge.first->next;
        res.push_back(bridge.first);
        break;
      case PREV_SECOND_BRIDGE:bridge.second = bridge.second->prev;
        res.push_back(bridge.second);
        break;
      case NEXT_SECOND_BRIDGE:res.push_back(bridge.second);
        bridge.second = bridge.second->next;
        break;
    }
  }
  FixLinks(bridge, res, first_bridge->point.x);
  return res;
}

std::vector<Node *> BuildHull(std::vector<Node *> &points, int left, int right) {
  if (right - left == 1) return std::vector<Node *>({points[left]});
  int m = (left + right) / 2;
  std::vector<Node *> help1 = move(BuildHull(points, left, m));
  std::vector<Node *> help2 = move(BuildHull(points, m, right));
  std::vector<Node *> res = move(MergeHulls(help1, help2, points[m - 1], points[m]));
  return res;
}

struct Face {
  int first_vertex;
  int second_vertex;
  int third_vertex;

  Face(int f, int s, int t)
      : first_vertex(f), second_vertex(s), third_vertex(t) {}
};

void AddLowerHull(std::vector<Node *> &points, std::vector<Face> &result) {
  std::sort(points.begin(), points.end(), [](Node *a, Node *b) {
    return a->point.x < b->point.x;
  });
  std::vector<Node *> point_seq = move(BuildHull(points, 0, points.size()));
  for (Node *p : point_seq) {
    Face current(p->prev->id, p->id, p->next->id);
    p->GetLinksValid();
    result.push_back(current);
  }
}

std::vector<Face> GetFaces(std::vector<Node *> points) {
  std::vector<Face> result;
  AddLowerHull(points, result);

  return result;
}

std::pair<int, int> makeEdge(int a, int b) {
  return std::make_pair(std::min(a, b), std::max(a, b));
}

long double MeanVoronoyVertex(std::vector<Node *> &points) {
  long double inf = 20000000;
  Point inf1(-inf, inf);
  Point inf2(inf, -inf);
  Node small = Node(-1, inf1, nullptr, nullptr);
  Node big = Node(-2, inf2, nullptr, nullptr);
  points.push_back(&small);
  points.push_back(&big);
  std::vector<Face> faces = GetFaces(points);
  std::set<int> extreme_points;
  for (auto f : faces) {
    if (f.first_vertex < 0 || f.second_vertex < 0 || f.third_vertex < 0) {
      extreme_points.insert(f.first_vertex);
      extreme_points.insert(f.second_vertex);
      extreme_points.insert(f.third_vertex);
    }
  }
  std::vector<bool> is_extreme(points.size(), false);
  for (auto e : extreme_points) {
    if (e >= 0) is_extreme[e] = true;
  }

  is_extreme[is_extreme.size() - 1] = is_extreme[is_extreme.size() - 2] = true;

  long double polygon_edges = 0;
  for (auto f : faces) {
    if (f.first_vertex >= 0 && !is_extreme[f.first_vertex]) {
      ++polygon_edges;
    }
    if (f.second_vertex >= 0 && !is_extreme[f.second_vertex]) {
      ++polygon_edges;
    }
    if (f.third_vertex >= 0 && !is_extreme[f.third_vertex]) {
      ++polygon_edges;
    }
  }

  long double polygon_num = points.size() - extreme_points.size();
  return polygon_num == 0 ? 0 : polygon_edges / polygon_num;
}

int main() {
  long double x, y;
  std::vector<Node *> points;
  int n = 0;
  Point p{};
  while ((std::cin >> p)) {
    Node *a = new Node(n, p, nullptr, nullptr);
    points.push_back(a);
    ++n;
  }

  std::cout << std::setprecision(7) << MeanVoronoyVertex(points);

  return 0;
}