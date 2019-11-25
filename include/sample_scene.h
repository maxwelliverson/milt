//
// Created by maxwe on 2019-11-20.
//

#ifndef MILT_SAMPLE_SCENE_H
#define MILT_SAMPLE_SCENE_H

#include <cmath>

#include "scene.h"

template <typename T>
class sphere : public scene_obj<T>{
    const Node<T>* center;
    const T radius;
public:
    sphere() = default;
    sphere(const Node<T>* c, const T r) : center(c), radius(r) {};
    T sdf(const Node<T>* n);
    Node<T> toObj(const Node<T>* node);
    //T sdf(Node<T>* n, Node<T>* dir) = 0;
    ~sphere(){delete center;};
};

template <typename T>
class rounded_box : public scene_obj<T> {
    const Node<T>* center;
    const Node<T>* lengths_to_sides;
    const T round;
public:
    rounded_box() = default;
    rounded_box(const Node<T>* c, const Node<T>* len, T r) : center(c), lengths_to_sides(len), round(r) {};
    T sdf(const Node<T>* node);
    Node<T> toObj(const Node<T>* node);
    //T sdf(Node<T>* n, Node<T>* dir) = 0;
    ~rounded_box(){delete center, delete lengths_to_sides;};
};

template <typename T>
class cube : public scene_obj<T> {
    const Node<T>* center;
    const T len_to_sides;
public:
    cube() = default;
    cube(const Node<T>* c, const T len) : center(c), len_to_sides(len) {};
    T sdf(const Node<T>* n);
    Node<T> toObj(const Node<T>* n);
    //T sdf(Node<T>* n, Node<T>* dir) = 0;
    ~cube(){delete center;};
};

template <typename T>
struct sample_scene : scene<T> {
    scene_obj<T>** objs;
    int obj_count;
    ~sample_scene() = default;
    T sdf(const Node<T>* n) override;
    T sdf(const Node<T>* node, scene_obj<T>* &obj);
};

//definitions

template <typename T>
T sphere<T>::sdf(const Node<T>* node) {
    return (*center - node).norm() - radius;
}

template <typename T>
Node<T> sphere<T>::toObj(const Node<T>* node){
    Node<T> new_n = *center - node;
    normalize(&new_n);
    return new_n;
}

template <typename T>
T rounded_box<T>::sdf(const Node<T>* node) {
    Node<T> tmp = *node - center;
    Node<T> q = abs(&tmp) - lengths_to_sides;
    return (max(&q, (T)0) + std::min(std::max(std::max(q.y, q.z),q.x), (T)0)).norm() - round;
}

template <typename T>
Node<T> rounded_box<T>::toObj(const Node<T>* node){
    Node<T> new_n = *center - node;
    normalize(&new_n);
    return new_n;
}

template <typename T>
T cube<T>::sdf(const Node<T>* node) {
    Node<T> tmp = *node - center;
    Node<T> q = abs(&tmp) - len_to_sides;
    return (max(&q, (T)0) + std::min(std::max(std::max(q.y, q.z),q.x), (T)0)).norm();
}

template <typename T>
Node<T> cube<T>::toObj(const Node<T>* node){
    Node<T> new_n = *center - node;
    normalize(&new_n);
    return new_n;
}

template <typename T>
T sample_scene<T>::sdf(const Node<T> *node) {
    T min_dist = std::numeric_limits<T>::max();
    for(int i = 0; i < obj_count; ++i){
        if(std::abs(objs[i]->sdf(node)) < std::abs(min_dist)){
            min_dist = objs[i]->sdf(node);
        }
    }
    return min_dist;
}

template <typename T>
T sample_scene<T>::sdf(const Node<T> *node, scene_obj<T>* &obj) {
    T min_dist = std::numeric_limits<T>::max();
    obj = nullptr;
    for(int i = 0; i < obj_count; ++i){
        if(std::abs(objs[i]->sdf(node)) < std::abs(min_dist)){
            min_dist = objs[i]->sdf(node);
            obj = objs[i];
        }
    }
    return min_dist;
}

#endif //MILT_SAMPLE_SCENE_H
