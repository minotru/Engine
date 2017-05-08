#include "Engine.h"
#include <algorithm>

namespace Engine
{

bool feq(float f1, float f2)
{
    return (fabs(f1 - f2) <= EPS);
}

DrawStyle::DrawStyle()
{
    pen = { 0 };
    pen.lopnColor = RGB(255, 255, 255);
    pen.lopnStyle = PS_NULL;
    pen.lopnWidth = { 6, 0 };
    brush = { 0 };
    brush.lbColor = RGB(0, 0, 0);
    brush.lbStyle = BS_HOLLOW;
}

void DrawStyle::setPen(int _style, int _width, COLORREF _color)
{
    pen.lopnStyle = _style;
    pen.lopnWidth = { _width, 0 };
    pen.lopnColor = _color;
}

void DrawStyle::setBrush(int _style, COLORREF _color, int _hatch)
{
    brush.lbStyle = _style;
    brush.lbColor = _color;
    brush.lbHatch = _hatch;
}

void DrawStyle::apply(HDC hDC) const
{
    SelectObject(hDC, CreateBrushIndirect(&brush));
    SelectObject(hDC, CreatePenIndirect(&pen));
}

void DrawStyle::remove(HDC hDC) const
{
    DeleteObject(SelectObject(hDC, GetStockObject(NULL_BRUSH)));
    DeleteObject(SelectObject(hDC, GetStockObject(NULL_PEN)));
}

POINT Point::toPOINT() const
{
    POINT p;
    p.x = (int)x;
    p.y = (int)y;
    return p;
}

Point& Point::operator=(const Point& p)
{
    x = p.x;
    y = p.y;
    return *this;
}

bool Point::operator==(const Point & p) const
{
    return feq(x, p.x) && feq(y, p.y);
}

bool Point::operator!=(const Point& p) const
{
    return !(*this == p);
}

Vector Vector::operator+(const Vector& v) const
{
    return Vector(x + v.x, y + v.y);
}

Vector Vector::operator*(float f) const
{
    return Vector(x*f, y*f);
}

Matrix::Matrix()
{
    *this = identity();
}

Matrix::Matrix(const Matrix& other)
{
    *this = other;
}

Matrix::Matrix(float _m11, float _m12, 
                     float _m21, float _m22, 
                     float _dx, float _dy)
{
    m11 = _m11;
    m12 = _m12;
    m21 = _m21;
    m22 = _m22;
    dx = _dx;
    dy = _dy; 
}

Matrix& Matrix::operator=(const Matrix& other)
{
    m11 = other.m11;
    m12 = other.m12;
    m21 = other.m21;
    m22 = other.m22;
    dx = other.dx;
    dy = other.dy;
    return *this;
}

Matrix Matrix::operator*(const Matrix & b) const
{
    Matrix res;
    res.m11 = m11*b.m11 + m12*b.m21;
    res.m21 = m21*b.m11 + m22*b.m21;
    res.m12 = m11*b.m12 + m12*b.m22;
    res.m22 = m21*b.m12 + m22*b.m22;
    res.dx  = m11*b.dx + m12*b.dy + dx;
    res.dy  = m21*b.dx + m22*b.dy + dy;
    return res; 
}

Point Matrix::operator*(const Point& p) const
{   
    Point res;
    res.x = m11*p.x + m12*p.y + dx;
    res.y = m21*p.x + m22*p.y + dy;
    return res;
}

Matrix Matrix::operator*(float f) const
{
    return Matrix(m11*f, m12*f, m21*f, m22*f, dx*f, dy*f);
}

Matrix & Matrix::operator*=(const Matrix & m)
{
   *this = *this * m;
   return *this;
}

Matrix & Matrix::operator*=(float f)
{
    *this = *this * f;
    return *this;

}

Matrix Matrix::transpose() const
{
    return Matrix(m11, m21, m12, m22, dx, dy);
}

Matrix operator*(float f, const Matrix& m)
{
    return m*f;
}

Matrix Matrix::identity()
{
    return Matrix(1, 0, 0, 1, 0, 0);
}

Matrix Matrix::zero()
{
    return Matrix(0, 0, 0, 0, 0, 0);
}

Matrix Matrix::inverse() const
{
    Matrix m;
    m.m11 = m22;
    m.m12 = -m12;
    m.m21 = -m21;
    m.m22 = m11;
    m.dx = (m12*dy - m22*dx);// /(m12*m21 - m11*m22);
    m.dy = (m21*dx - m11*dy);// /(m12*m21 - m11*m22);
    m *= 1/determinant();
    return m;

}

float Matrix::determinant() const
{
    return m11*m22 - m12*m21;
}

XFORM Matrix::toXFORM() const
{
    XFORM x;
    x.eDx = 0;
    x.eDy = 0;
    x.eM11 = m11;
    x.eM12 = m12;
    x.eM21 = m21;
    x.eM22 = m22;
    x.eDx = dx;
    x.eDy = dy;
    return x;
}

Transform::Transform(Object & object) : m_object(object)
{
    m_parent = NULL;
}

//Point Transform::transform(const Point& p) const
//{
//    return m_matrix*p;
//}

void Transform::addChild(Transform * t)
{
    if (t->m_parent)
        t->m_parent->m_children.remove(t);
    m_children.push_back(t);
    t->m_parent = this;
    t->update();
}

void Transform::removeChild(Transform * t)
{
    m_children.remove(t);
    t->m_parent = NULL;
    t->update();
}

void Transform::setParent(Transform * newParent)
{
    if (m_parent)
        m_parent->m_children.remove(this);
    if (newParent)
        newParent->m_children.push_back(this);
    m_parent = newParent;
    update();
}


void Transform::rotate(float angle, Space relativeTo)
{
    if (relativeTo == Space::SELF)
        transformLocal(rotateMatrix(angle));
    else
        transformWorld(rotateMatrix(angle));
}

void Transform::rotateAround(Point p, float angle)
{
    Matrix d, t, r;
    //p = worldToLocal(p);
    //translate(Point(-p.x, -p.y), Space::WORLD);
    //rotate(angle, Space::SELF);
    //m_localMatrix *= rotateMatrix(angle);
    //update();
    //translate(Point(p.x, p.y), Space::WORLD);
    //translate(Vector(p.x, p.y), Space::WORLD);
    //translate(Vector(-p.x, -p.y), Space::WORLD);
    d.dx = p.x;
    d.dy = p.y;
    r = rotateMatrix(angle);
    t = d * r * d.inverse();
    transformWorld(t);
}

void Transform::translate(Vector dir, Space relativeTo)
{
    Matrix m;
    m.dx = dir.x;
    m.dy = dir.y;
    if (relativeTo == Space::WORLD)
        transformWorld(m);
    else
        transformLocal(m);
}

Point Transform::localToWorld(const Point & p) const
{
    return m_world * p;
}

Point Transform::worldToLocal(const Point & p) const
{
    return m_world.inverse() * p;
}

Matrix Transform::getWorldToLocalMatrix() const
{
    return m_world.inverse();
}

Matrix Transform::getLocalToWorldMatrix() const
{
    return m_world;
}

void Transform::update()
{   
    if (hasParent())
        m_world = m_parent->m_world;
    else
        m_world = Matrix::identity();
    m_world =  m_world * m_model;
    for (list<Transform*>::iterator 
         it = m_children.begin(); it != m_children.end(); it++)
         (*it)->update();
}

void Transform::transformLocal(const Matrix & m)
{
    m_model =  m_model * m;
    //m_localTransform *= m;
    update();
}

void Transform::transformWorld(const Matrix & m)
{
    m_model =  m * m_model ;
    update();
}

Matrix Transform::rotateMatrix(float angle)
{
    Matrix m;
    angle = degToRad(angle);
    m.m11 = cos(angle);
    m.m12 = -sin(angle);
    m.m21 = sin(angle);
    m.m22 = cos(angle);
    return m;
}

//const Matrix& Transform::matrix() const
//{
//    return m_matrix;
//}


void ComplexObject::addObject(Object *obj)
{
    if (obj->hasParent())
        static_cast<ComplexObject*>(obj->getParent())->removeObject(obj);
    m_objects.push_back(obj);
    obj->setParent(this);
    transform.addChild(&(obj->transform));

}

void ComplexObject::removeObject(Object *obj)
{
    m_objects.remove(obj);
    transform.removeChild(&(obj->transform));
    obj->setParent(NULL);
}

void ComplexObject::draw(HDC hdc) const
{
    for (list<Object*>::const_iterator 
         it = m_objects.begin(); it != m_objects.end(); it++)
        (*it)->draw(hdc);
}

void Polygon::draw(HDC hdc) const
{
    drawStyle.apply(hdc);
    vector<POINT> points(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++)
        points[i] = transform.localToWorld(vertices[i]);
    ::Polygon(hdc, points.data(), points.size());
    drawStyle.remove(hdc);
}

Polygon & Polygon::addVertex(float x, float y)
{
    vertices.push_back(Point(x, y));
    return *this;
}

Polygon & Polygon::addVertex(const Point & p)
{
    vertices.push_back(p);
    return *this;
}

Polygon & Polygon::removeVertex(int id)
{
    vertices.erase(vertices.begin() + id);
    return *this;
}

Rectangle::Rectangle(Point p1, Point p2) : Polygon(4)
{
    set(p1, p2);
}

Rectangle& Rectangle::set(Point p1, Point p2)
{
    vertices[0] = p1;
    vertices[1] = {p2.x, p1.y};
    vertices[2] = p2;
    vertices[3] = {p1.x, p2.y};
    return *this;
}


Ellipse::Ellipse(Point center, int a, int b)
{
    set(center, a, b);
}

Ellipse& Ellipse::set(Point center, int a, int b)
{
    Point topLeft, bottomRight;
    m_topLeft.x = center.x - a;
    m_topLeft.y = center.y + b;
    m_bottomRight.x = center.x + a;
    m_bottomRight.y = center.y - b;
    return *this;
}

void Ellipse::draw(HDC hdc) const
{
    drawStyle.apply(hdc);
 
    XFORM oldTransform;
    GetWorldTransform(hdc, &oldTransform);
    XFORM xf = transform.getLocalToWorldMatrix().toXFORM();
    //int x1, y1, x2, y2;
     
    SetWorldTransform(hdc, &xf);
    ::Ellipse(hdc, (int)m_topLeft.x, (int)m_topLeft.y, 
                  (int)m_bottomRight.x, (int)m_bottomRight.y);
    SetWorldTransform(hdc, &oldTransform);
    drawStyle.remove(hdc);
}



float distance(Point p1, Point p2)
{
    return sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

float radToDeg(float rad)
{
    return rad / PI * 180;
}

float degToRad(float deg)
{
    return PI / 180 * deg;
}

void Line::draw(HDC hdc) const
{
    POINT old;
    Point tp1 = transform.localToWorld(p1), tp2 = transform.localToWorld(p2);
    drawStyle.apply(hdc);
    MoveToEx(hdc, (int)tp1.x, (int)tp1.y, &old);
    LineTo(hdc, (int)tp2.x, (int)tp2.y);
    drawStyle.remove(hdc);
    MoveToEx(hdc, old.x, old.y, NULL);
}

} //Engine