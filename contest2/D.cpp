#include <iostream>
#include <algorithm>
#include <iomanip>
#include <list>
#include <vector>

struct Point {
  double x, y;
  Point() = default;
  Point(double start, double end) : x(start), y(end) {}
};

bool operator<(const Point &a, const Point &b) {
  return a.x < b.x || a.x == b.x && a.y < b.y;
}

bool operator==(const Point &a, const Point &b) {
  return a.x == b.x && a.y == b.y;
}

bool operator!=(const Point &a, const Point &b) {
  return !(a == b);
}

struct Vector {
  double x, y;

  Vector() = default;
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

bool operator>(const Vector &a, const Vector &b) {
  return CrossProductSign(a, b) < 0;
}

std::vector<Vector> GetPolygonsVector(std::vector<Point> &polygon, int min_point) {
  std::vector<Vector> res(0);
  if (polygon.size() == 1) return res;
  int n = polygon.size();
  for (int i = 0; i < n; ++i) {
    res.emplace_back(Vector(polygon[(i + min_point) % n], polygon[(i + 1 + min_point) % n]));
  }
  return res;
}

int GetMinPointIndex(std::vector<Point> &polygon) {
  int min_point = 0;
  for (int i = 0; i < polygon.size(); ++i) {
    if (polygon[i] < polygon[min_point]) min_point = i;
  }
  return min_point;
}

bool HaveCrossing(std::vector<Point> polygon1, std::vector<Point> polygon2) {
  Point zero(0, 0);

  std::vector<Point> neg_polygon2;
  for (auto p : polygon2) {
    neg_polygon2.emplace_back(Point(-p.x, -p.y));
  }

  int min_point1 = GetMinPointIndex(polygon1);
  int min_point2 = GetMinPointIndex(neg_polygon2);

  std::vector<Vector> vec1(GetPolygonsVector(polygon1, min_point1));
  std::vector<Vector> vec2(GetPolygonsVector(neg_polygon2, min_point2));
  std::vector<Vector> mink_vec(vec1.size() + vec2.size());
  std::merge(vec1.begin(),
             vec1.end(),
             vec2.begin(),
             vec2.end(),
             mink_vec.begin(),
             [](const Vector &a, const Vector &b) { return a > b; });

  Point temp_point
      (polygon1[min_point1].x + neg_polygon2[min_point2].x, polygon1[min_point1].y + neg_polygon2[min_point2].y);

  for (auto v : mink_vec) {
    if (Vector(temp_point, zero) > v) return false;
    temp_point.x += v.x;
    temp_point.y += v.y;
  }
  return true;
}

int main(int argc, char *argv[]) {
  int n;
  std::vector<Point> polygon1, polygon2;
  Point p;
  std::cin >> n;
  for (int i = 0; i < n; ++i) {
    std::cin >> p.x >> p.y;
    polygon1.push_back(p);
  }
  std::cin >> n;
  for (int i = 0; i < n; ++i) {
    std::cin >> p.x >> p.y;
    polygon2.push_back(p);
  }
  std::cout
      << (HaveCrossing(polygon1, polygon2) ? "YES" : "NO");
  return 0;
}


