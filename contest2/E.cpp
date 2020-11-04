#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>

enum Type {
  ADD = 0,
  DELETE = 1
};

struct Point {
  int x, y;
  Point(int _x, int _y) : x(_x), y(_y) {}
  Point() = default;
};

struct Vector {
  int x, y;

  Vector(Point a, Point b) {
    x = b.x - a.x;
    y = b.y - a.y;
  }
};

int CrossProductSign(Vector a, Vector b) {
  if (a.x * b.y - a.y * b.x < 0) {
    return -1;
  } else if (a.x * b.y - a.y * b.x == 0) {
    return 0;
  } else {
    return 1;
  }
}

struct Segment {
  int id{};
  Point start{};
  Point end{};

  Segment() = default;

  Segment(int _id, int x1, int y1, int x2, int y2) : id(_id) {
    start = Point(x1, y1);
    end = Point(x2, y2);
    if (x1 > x2) {
      std::swap(start, end);
    } else if (x1 == x2 && y1 > y2) {
      std::swap(start, end);
    }
  }

  Vector GetVector() const {
    return Vector(start, end);
  }

  double GetY(int x) const {
    int len = end.x - start.x;
    if (len == 0) return start.y;
    return start.y + (end.y - start.y) * ((double) (x - start.x) / len);
  }
};

std::pair<int, int> AreCrossing(Segment a, Segment b) {
  if (a.GetVector().x == 0 && b.GetVector().x == 0) {
    if (a.start.y < b.start.y && a.end.y > b.start.y || b.start.y < a.start.y && b.end.y > a.start.y) {
      return std::make_pair(std::min(a.id, b.id), std::max(a.id, b.id));
    } else {
      return std::make_pair(-1, -1);
    }
  }

  bool different_parts_a = CrossProductSign(a.GetVector(), Vector(a.start, b.start))
      != CrossProductSign(a.GetVector(), Vector(a.start, b.end))
      || CrossProductSign(a.GetVector(), Vector(a.start, b.start)) == 0
          && CrossProductSign(a.GetVector(), Vector(a.start, b.start))
              == CrossProductSign(a.GetVector(), Vector(a.start, b.end));
  bool different_parts_b = CrossProductSign(b.GetVector(), Vector(b.start, a.start))
      != CrossProductSign(b.GetVector(), Vector(b.start, a.end))
      || CrossProductSign(b.GetVector(), Vector(b.start, a.start)) == 0
          && CrossProductSign(b.GetVector(), Vector(b.start, a.start))
              == CrossProductSign(b.GetVector(), Vector(b.start, a.end));
  if (!(different_parts_b && different_parts_a)) {
    return std::make_pair(-1, -1);
  } else {
    return std::make_pair(std::min(a.id, b.id), std::max(a.id, b.id));
  }
}

struct Event {
  Type type;
  int seg_id, x;
  Event(int id, Type t, int _x) : seg_id(id), type(t), x(_x) {}
  Event() = default;
};

class Comp {
 public:
  bool operator()(const Segment &a, const Segment &b) {
    int x = std::max(a.start.x, b.start.x);
    return a.GetY(x) < b.GetY(x) || a.GetY(x) == b.GetY(x) && a.start.x < b.start.x;
  }
};

std::pair<int, int> GetCrossingSegments(std::vector<Segment> &segments) {
  std::set<Segment, Comp> line;
  std::vector<Event> events;
  for (auto s : segments) {
    events.emplace_back(s.id, ADD, s.start.x);
    events.emplace_back(s.id, DELETE, s.end.x);
  }
  std::sort(events.begin(), events.end(), [](Event a, Event b) {
    return a.x < b.x || a.x == b.x && a.type < b.type;
  });

  for (auto e : events) {
    if (e.type == ADD) {
      auto next = line.lower_bound(segments[e.seg_id]);
      bool flag = AreCrossing(*next, segments[e.seg_id]).first > -1;
      if (next != line.end() && flag) {
        return AreCrossing(*next, segments[e.seg_id]);
      }
      if (next != line.begin()) {
        auto prev = next;
        --prev;
        flag = AreCrossing(*prev, segments[e.seg_id]).first > -1;
        if (prev != line.end() && flag) {
          return AreCrossing(*prev, segments[e.seg_id]);
        }
      }
      line.insert(segments[e.seg_id]);
    } else {
      auto prev = line.find(segments[e.seg_id]);
      auto next = prev;
      if (prev == line.begin()) {
        prev = line.end();
      } else {
        --prev;
      }
      ++next;
      if (prev != line.end() && next != line.end() && AreCrossing(*prev, *next).first > -1) {
        return AreCrossing(*prev, *next);
      }
      --next;
      line.erase(next);
    }
  }

  return std::make_pair(-1, -1);
}

int main() {
  int n;
  std::vector<Segment> segments;
  Segment s;
  int start_x, start_y, end_x, end_y;
  std::cin >> n;
  for (int i = 0; i < n; ++i) {
    std::cin >> start_x >> start_y >> end_x >> end_y;
    s = Segment(i, start_x, start_y, end_x, end_y);
    segments.push_back(s);
  }
  std::pair<int, int> cross_id = GetCrossingSegments(segments);
  if (cross_id.first == -1) {
    std::cout << "NO";
  } else {
    std::cout << "YES\n" << cross_id.first + 1 << " " << cross_id.second + 1;
  }
  return 0;
}