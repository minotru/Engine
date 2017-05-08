#ifndef _ENGINE_H
#define _ENGINE_H

#include <vector>
#include <list>
#include <windows.h>
#include <cmath>

using namespace std;

namespace Engine 
{

const double EPS = 10e-4;
const double PI = 3.14159265;

bool feq(float f1, float f2);

struct Point;
class Matrix;
class Transform;
struct DrawStyle;
class Object;
class ComplexObject;
class Polygon;
class Rectangle;
class Ellipse;
class Line;

float distance(Point, Point);
float radToDeg(float rad);
float degToRad(float deg);
    
enum struct Space {SELF, WORLD};

//class Renderer
//{
//public:
//    Renderer();
//    Renderer& setPenColor(char, char, char);
//
//private:
//    enum {USE_PEN = 0, USE_BRUSH = 1, USE_TEXTURE = 2};
//    int m_flags;     
//    LOGPEN m_pen;
//    LOGBRUSH m_brush;
//};

struct DrawStyle
{
    LOGPEN pen;
    LOGBRUSH brush;
    DrawStyle();
    void setPen(int _style, int _width, COLORREF _color);
    void setBrush(int _style, COLORREF _color, int _hatch = 0);
    void apply(HDC hDC) const;
    void remove(HDC hDC) const;
};

struct Point
{
    Point(float _x = 0, float _y = 0) : x(_x), y(_y) {}
    Point(const POINT& p) : x((float)p.x), y((float)p.y) {}
    operator POINT() const {return toPOINT();}
    POINT toPOINT() const;
    Point& operator=(const Point&);
    bool operator==(const Point&) const;
    bool operator!=(const Point&) const;
    float x, y;
};

struct Vector : public Point
{
    Vector(float _x, float _y) : Point(_x, _y) {}
    Vector operator+(const Vector&) const;
    Vector operator*(float) const;

};

class Matrix
{
public:
    Matrix();
    Matrix(float _m11, float _m12, 
              float _m21, float _m22, 
              float _dx = 0, float _dy = 0);
    Matrix(const Matrix&);
    Matrix& operator=(const Matrix&);
    Matrix operator*(const Matrix&) const;
    Matrix operator*(float) const;
    Matrix& operator*=(const Matrix&);
    Matrix& operator*=(float);
    Matrix transpose() const;
    friend Matrix operator*(float, const Matrix&);
    Point operator*(const Point&) const;
    static Matrix identity();
    static Matrix zero();
    Matrix inverse() const;
    float determinant() const;
    XFORM toXFORM() const;
    float m11, m12, m21, m22, dx, dy;
};

class Transform
{
public:
    Transform(Object & object);
    void addChild(Transform*);
    void removeChild(Transform*);
    void setParent(Transform*);
    Transform* getParent() const {return m_parent;}
    const list<Transform*>& getChildren() const {return m_children;}
    bool hasParent() const {return m_parent != NULL;}

    void rotate(float angle, Space relativeTo = Space::SELF);
    void rotateAround(Point, float);
    void translate(Vector dir, Space relativeTo = Space::SELF);
    Point localToWorld(const Point&) const;
    Point worldToLocal(const Point&) const;
    Matrix getWorldToLocalMatrix() const;
    Matrix getLocalToWorldMatrix() const;
private:
    void update();
    void transformLocal(const Matrix&);
    void transformWorld(const Matrix&);
    static Matrix rotateMatrix(float angle);
    Matrix m_model, m_world;
    list<Transform*> m_children;
    Transform* m_parent;
    Object & m_object;
};

class Object
{
public:
    Object() : m_parent(NULL), transform(*this) {}
    virtual void draw(HDC) const = 0;
    void setParent(Object *parent) {m_parent = parent;}
    bool hasParent() const {return m_parent != NULL;}
    Object* getParent() const {return m_parent;}
    DrawStyle drawStyle;
    Transform transform;
private:    
    Object *m_parent;
};

class ComplexObject : public Object
{
public:
    virtual void draw(HDC) const;
    void addObject(Object *obj);
    void removeObject(Object *obj);
    list<Object*>& getObjects() {return m_objects;}
    const list<Object*>& getObjects() const {return m_objects;}
private:    
    list<Object*> m_objects;
    list<Object*> m_objectPool;
};

class Line : public Object
{
public:
    Line(const Point& _p1 = Point(), const Point& _p2 = Point()) 
        : p1(_p1), p2(_p2) {}
    virtual void draw(HDC) const;
    Point p1, p2;
};

class Polygon : public Object
{
public:
    Polygon(size_t verticesCnt = 0) : vertices(verticesCnt){}
    vector<Point> vertices;
    virtual void draw(HDC) const;
    size_t getVerticesNumber() const {return vertices.size();}

    Polygon& addVertex(float x, float y);
    Polygon& addVertex(const Point& p);
    Polygon& removeVertex(int id);
};

//class Polyline : public Object
//{
//public:
//    Polyline();
//    Polyline(const Polyline&);
//    Line& addLine(const Point& p1, const Point& p2);
//    Line& addLineTo(const Point& p);
//    vector<Line> lines;
//    Point position;        
//};

class Rectangle : public Polygon
{
public:
    Rectangle(Point topLeft = {0, 0}, Point bottomRight = {0, 0});
    Rectangle& set(Point topLeft, Point bottomRight);
};

class Ellipse : public Object
{
public:
    Ellipse(Point center = {0, 0}, int a = 0, int b = 0);
    Ellipse& set(Point center, int a, int b);
    virtual void draw(HDC) const;
private:
    Point m_topLeft, m_bottomRight; 
};

typedef ComplexObject Scene;

} // Engine

#endif _ENGINE_H
