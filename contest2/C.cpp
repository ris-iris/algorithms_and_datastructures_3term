#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

#define INFTY 1e9

enum state {
  FIRST_PART = 0,
  SECOND_PART = 1,
  PREV_FIRST_BRIDGE = 2,
  NEXT_FIRST_BRIDGE = 3,
  PREV_SECOND_BRIDGE = 4,
  NEXT_SECOND_BRIDGE = 5
};

enum hullMode {
  UPPER = false,
  LOWER = true
};

struct Point {
  double x, y, z;

  void rotate(double angle) {
    double new_z = z * cos(angle) + y * sin(angle);
    double new_y = -z * sin(angle) + y * cos(angle);
    z = new_z;
    y = new_y;

    double new_x = x * cos(angle) + z * sin(angle);
    new_z = -x * sin(angle) + z * cos(angle);
    x = new_x;
    z = new_z;

    new_x = x * cos(angle) + y * sin(angle);
    new_y = -x * sin(angle) + y * cos(angle);
    x = new_x;
    y = new_y;
  }
};

bool operator<(Point a, Point b) {
  return a.x < b.x || a.x == b.x && a.z < b.z;
}

std::istream &operator>>(std::istream &is, Point &p) {
  is >> p.x >> p.y >> p.z;
  return is;
}

double CrossProduct(Point a, Point b, Point c, char mode) {
  if (mode == 'y') {
    return (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
  }
  if (mode == 'z') {
    return ((b.x - a.x) * (c.z - b.z) - (b.z - a.z) * (c.x - b.x));
  }
}

struct Node {
  int id;
  Point point;
  Node *prev;
  Node *next;

  Node() = default;
  Node(int _id, Point p, Node *pr, Node *n)
      : id(_id), point(p), prev(pr), next(n) {}

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

bool IsRightTurn(const Node *a, const Node *b, const Node *c) {
  if (a == nullptr || b == nullptr || c == nullptr) {
    return false;
  }
  return CrossProduct(a->point, b->point, c->point, 'y') <= 0;
}

double TurnTime(Node *a, Node *b, Node *c) {
  if (a == nullptr || b == nullptr || c == nullptr) {
    return INFTY;
  }
  double help = CrossProduct(a->point, b->point, c->point, 'y');
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

void FixLinks(std::pair<Node *, Node *> &bridge, std::vector<Node *> &res, double divider_x) {
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
  double current_time = -INFTY;
  while (true) {
    double min_time = INFTY;
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

  void GetRightOrder() {
    if (second_vertex < first_vertex && second_vertex < third_vertex) {
      std::swap(first_vertex, second_vertex);
      std::swap(second_vertex, third_vertex);
    } else if (third_vertex < first_vertex && third_vertex < second_vertex) {
      std::swap(second_vertex, third_vertex);
      std::swap(first_vertex, second_vertex);
    }
  }
};

std::ostream &operator<<(std::ostream &os, Face &f) {
  os << f.first_vertex << " " << f.second_vertex << " " << f.third_vertex << "\n";
  return os;
}

void AddHullPart(std::vector<Node *> &points, std::vector<Face> &result, bool mode) {
  for (Node *p : points) {
    p->next = nullptr;
    p->prev = nullptr;
    p->point.z *= -1;
  }
  std::sort(points.begin(), points.end(), [](Node *a, Node *b) {
    return a->point < b->point;
  });
  std::vector<Node *> point_seq = BuildHull(points, 0, points.size());
  for (Node *p : point_seq) {
    Face current(p->prev->id, p->id, p->next->id);
    if (mode ^ p->GetLinksValid()) {
      std::swap(current.first_vertex, current.second_vertex);
    }
    result.push_back(current);
  }
}

std::vector<Face> GetFaces(std::vector<Node *> points) {
  std::vector<Face> result;
  AddHullPart(points, result, UPPER);
  AddHullPart(points, result, LOWER);

  return result;
}

int main() {
  int n;
  std::cin >> n;
  for (int i = 0; i < n; i++) {
    int m;
    std::vector<Node *> points;

    std::cin >> m;
    for (int j = 0; j < m; j++) {
      Point point;
      std::cin >> point;
      point.rotate(0.05);
      Node *p = new Node(j, point, nullptr, nullptr);
      points.push_back(p);
    }

    std::vector<Face> hull = GetFaces(points);
    for (Face &f : hull) {
      f.GetRightOrder();
    }

    std::sort(hull.begin(), hull.end(), [](Face a, Face b) {
      return a.first_vertex < b.first_vertex || a.first_vertex == b.first_vertex && (a.second_vertex < b.second_vertex
          || a.second_vertex == b.second_vertex && a.third_vertex < b.third_vertex);
    });
    std::cout << hull.size() << "\n";
    for (Face &f : hull) {
      std::cout << 3 << " " << f;
    }
  }
  return 0;
}