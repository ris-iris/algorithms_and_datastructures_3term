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

struct Point {
  int id;
  double x, y, z;
  Point *prev;
  Point *next;

  Point() = default;
  Point(int _id, double _x, double _y, double _z, Point *p, Point *n)
      : id(_id), x(_x), y(_y), z(_z), prev(p), next(n) {}

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

bool IsRightTurn(const Point *a, const Point *b, const Point *c) {
  if (a == nullptr || b == nullptr || c == nullptr) {
    return false;
  }
  return (b->x - a->x) * (c->y - b->y) - (b->y - a->y) * (c->x - b->x) <= 0;
}

double TurnTime(Point *a, Point *b, Point *c) {
  if (a == nullptr || b == nullptr || c == nullptr) {
    return INFTY;
  }
  double help = (b->x - a->x) * (c->y - b->y) - (b->y - a->y) * (c->x - b->x);
  return help == 0 ? INFTY : ((b->x - a->x) * (c->z - b->z) - (b->z - a->z) * (c->x - b->x)) / help;
}

std::pair<Point *, Point *> FindBridge(Point *first, Point *second) {
  std::pair<Point *, Point *> bridge(first, second);
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

void FixLinks(std::pair<Point *, Point *> &bridge, std::vector<Point *> &res, double divider_x) {
  bridge.first->next = bridge.second;
  bridge.second->prev = bridge.first;
  for (int i = res.size() - 1; i > -1; --i) {
    Point *current = res[i];
    if (current->x > bridge.first->x && current->x < bridge.second->x) {
      bridge.first->next = current;
      bridge.second->prev = current;
      current->prev = bridge.first;
      current->next = bridge.second;
      if (current->x <= divider_x) {
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

std::vector<Point *> MergeHulls(const std::vector<Point *> &first,
                                const std::vector<Point *> &second,
                                Point *first_bridge,
                                Point *second_bridge) {
  std::vector<Point *> res;
  std::pair<Point *, Point *> bridge = move(FindBridge(first_bridge, second_bridge));

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
        if (first[iter1]->x < bridge.first->x)
          res.push_back(first[iter1]);
        first[iter1]->GetLinksValid();
        ++iter1;
        break;
      case SECOND_PART:
        if (second[iter2]->x > bridge.second->x)
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
  FixLinks(bridge, res, first_bridge->x);
  return res;
}

std::vector<Point *> BuildHull(std::vector<Point *> &points, int left, int right) {
  if (right - left == 1) return std::vector<Point *>({points[left]});
  int m = (left + right) / 2;
  std::vector<Point *> help1 = move(BuildHull(points, left, m));
  std::vector<Point *> help2 = move(BuildHull(points, m, right));
  std::vector<Point *> res = move(MergeHulls(help1, help2, points[m - 1], points[m]));
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

void AddLowerHull(std::vector<Point *> &points, std::vector<Face> &result) {
  for (Point *p : points) {
    p->next = nullptr;
    p->prev = nullptr;
    p->z *= -1;
  }
  std::sort(points.begin(), points.end(), [](Point *a, Point *b) {
    return a->x < b->x || a->x == b->x && a->z < b->z;
  });
  std::vector<Point *> point_seq = move(BuildHull(points, 0, points.size()));
  for (Point *p : point_seq) {
    Face current(p->prev->id, p->id, p->next->id);
    if (!p->GetLinksValid()) {
      std::swap(current.first_vertex, current.second_vertex);
    }
    result.push_back(current);
  }
}

void AddUpperHull(std::vector<Point *> &points, std::vector<Face> &result) {
  for (Point *p : points) {
    p->next = nullptr;
    p->prev = nullptr;
    p->z *= -1;
  }
  std::sort(points.begin(), points.end(), [](Point *a, Point *b) {
    return a->x < b->x || a->x == b->x && a->z < b->z;
  });
  std::vector<Point *> point_seq = BuildHull(points, 0, points.size());
  for (Point *p : point_seq) {
    Face current(p->prev->id, p->id, p->next->id);
    if (p->GetLinksValid()) {
      std::swap(current.first_vertex, current.second_vertex);
    }
    result.push_back(current);
  }
}

std::vector<Face> GetFaces(std::vector<Point *> points) {
  std::vector<Face> result;
  AddUpperHull(points, result);
  AddLowerHull(points, result);

  return result;
}

int main() {
  int n;
  std::cin >> n;
  for (int i = 0; i < n; i++) {
    int m;
    std::vector<Point *> points;

    std::cin >> m;
    for (int j = 0; j < m; j++) {
      int x, y, z;
      std::cin >> x >> y >> z;
      Point *p = new Point(j, x, y, z, nullptr, nullptr);
      p->rotate(0.05);
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
      std::cout << 3 << " " << f.first_vertex << " " << f.second_vertex << " " << f.third_vertex << "\n";
    }
  }
  return 0;
}