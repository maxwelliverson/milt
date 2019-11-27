//
// Created by maxwe on 2019-11-19.
//

#ifndef MILT_NODE_H
#define MILT_NODE_H

#ifdef DEBUG
#define NODE_DEBUG(...) __VA_ARGS__
#else
#define NODE_DEBUG(...)
#endif

#include <cmath>

template <typename T>
struct Node
{
    T x, y, z;
    Node() : x(0), y(0), z(0) {};
    Node(T ex, T why, T zed) : x(ex), y(why), z(zed) {};

    Node<T> operator +(const Node<T>* b) const;
    Node<T> operator +(T b) const;
    Node<T> operator -(const Node<T>* b) const;
    Node<T> operator -(T b) const;
    Node<T> operator *(const Node<T>* b) const;

    Node<T>& operator +=(const Node<T>* b);
    Node<T>& operator +=(Node<T> b);
    Node<T>& operator -=(const Node<T>* b);
    Node<T>& operator -=(Node<T> b);

    T dot(const Node<T>* b) const;
    T norm() const;
    T norm_squared() const;
};

template <typename T>
Node<T> operator *(const Node<T>* a, T k);
template <typename T>
Node<T> operator *(Node<T> a, T k);
template <typename T>
Node<T> operator *(T k, const Node<T>* a);
template <typename T>
Node<T> operator *(T k, Node<T> a);

//DEFINITIONS
template <typename T>
Node<T> Node<T>::operator +(const Node<T>* b) const {
    return Node<T>(x + b->x, y + b->y, z + b->z);
}

template <typename T>
Node<T> Node<T>::operator+(T b) const {
    return Node<T>(x + b, y + b, z + b);
}

template <typename T>
Node<T> Node<T>::operator -(const Node<T>* b) const {
    return Node<T>(x - b->x, y - b->y, z - b->z);
}

template <typename T>
Node<T> Node<T>::operator-(T b) const {
    return Node<T>(x - b, y - b, z - b);
}

template <typename T>
Node<T> Node<T>::operator *(const Node<T>* b) const {
    return Node<T>(x * b->x, y * b->y, z * b->z);
}

template <typename T>
Node<T> operator *(const Node<T>* a, T k) {
    return Node<T>(k * a->x, k * a->y, k * a->z);
}

template <typename T>
Node<T> operator *(T k, const Node<T>* a) {
    return a * k;
}

template <typename T>
Node<T> operator *(T k, Node<T> a){
    return Node<T>(k * a.x, k * a.y, k * a.z);
}

template <typename T>
Node<T> operator *(Node<T> a, T k){
    return k * a;
}

template <typename T>
Node<T>& Node<T>::operator+=(const Node<T> *b){
    x += b->x;
    y += b->y;
    z += b->z;
    return *this;
}

template <typename T>
Node<T>& Node<T>::operator+=(const Node<T> b){
    x += b.x;
    y += b.y;
    z += b.z;
    return *this;
}

template <typename T>
Node<T>& Node<T>::operator-=(const Node<T> *b){
    x -= b->x;
    y -= b->y;
    z -= b->z;
    return *this;
}

template <typename T>
Node<T>& Node<T>::operator-=(const Node<T> b){
    x -= b.x;
    y -= b.y;
    z -= b.z;
    return *this;
}

template <typename T>
T Node<T>::dot(const Node<T>* b) const {
    return (x * b->x) + (y * b->y) + (z * b->z);
}

template <typename T>
Node<T> max(Node<T>* a, Node<T>* b){
    return Node<T>(std::max(a->x, b->x), std::max(a->y, b->y), std::max(a->z, b->z));
}

template <typename T>
Node<T> max(Node<T>* a, T t){
    return Node<T>(std::max(a->x, t), std::max(a->y, t), std::max(a->z, t));
}

template <typename T>
Node<T> min(Node<T>* a, Node<T>* b){
    return Node<T>(std::min(a->x, b->x), std::min(a->y, b->y), std::min(a->z, b->z));
}

template <typename T>
Node<T> min(Node<T>* a, T t){
    return Node<T>(std::min(a->x, t), std::min(a->y, t), std::min(a->z, t));
}

template <typename T>
Node<T> abs(Node<T>* a){
    return Node<T>(std::abs(a->x), std::abs(a->y), std::abs(a->z));
}

template <typename T>
T dist_squared(Node<T>* n1, Node<T>* n2) {
    return ((n1->x - n2->x) * (n1->x - n2->x) + (n1->y - n2->y) * (n1->y - n2->y) + (n1->z - n2->z) * (n1->z - n2->z));
}

template <typename T>
T Node<T>::norm() const {
    return sqrt(norm_squared());
}

template <typename T>
T Node<T>::norm_squared() const {
    return ((x * x) + (y * y) + (z * z));
}

template <typename T>
void normalize(Node<T>* n) {
    T norm_inv = (T)1 / n->norm();
    n->x *= norm_inv;
    n->y *= norm_inv;
    n->z *= norm_inv;
}

#endif //MILT_NODE_H
