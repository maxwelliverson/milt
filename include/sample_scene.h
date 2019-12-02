//
// Created by maxwe on 2019-11-20.
//

#ifndef MILT_SAMPLE_SCENE_H
#define MILT_SAMPLE_SCENE_H

#include <cmath>
#include <string>

#include "scene.h"
//#include <glm/detail/setup.hpp>
//#include <glm/glm.hpp>

const auto xyy = Node(1.f, -1.f, -1.f);
const auto yyx = Node(-1.f, -1.f, 1.f);
const auto yxy = Node(-1.f, 1.f, -1.f);
const auto xxx = Node(1.f, 1.f, 1.f);
const auto eps = 0.0001f;

template <typename T>
class sphere : public scene_obj<T>{
public:
    Node<T>* center;
    T radius;
    sphere() : center(new Node<T>(0, 0, 0)), radius((T)0){}
    sphere(Node<T>* c, T r) : center(c), radius(r) {};
    T sdf(Node<T> n);
    T sdf(const Node<T>* n);
    Node<T> toObj(const Node<T>* node);
    const char* name();
    //T sdf(Node<T>* n, Node<T>* dir) = 0;
    ~sphere(){delete center;};
};

template <typename T>
class rounded_box : public scene_obj<T> {
public:
    Node<T>* center;
    Node<T>* lengths_to_sides;
    T round;
    rounded_box() : center(new Node<T>(0, 0, 0)), lengths_to_sides(new Node<T>(0, 0, 0)), round((T)0){}
    rounded_box(Node<T>* c, Node<T>* len, T r) : center(c), lengths_to_sides(len), round(r) {};
    T sdf(Node<T> n);
    T sdf(const Node<T>* node);
    Node<T> toObj(const Node<T>* node);
    const char* name();
    //T sdf(Node<T>* n, Node<T>* dir) = 0;
    ~rounded_box(){delete center, delete lengths_to_sides;};
};

template <typename T>
class cube : public scene_obj<T> {
public:
    Node<T>* center;
    T len_to_sides;
    cube() : center(new Node<T>(0, 0, 0)), len_to_sides((T)0){}
    cube(Node<T>* c, T len) : center(c), len_to_sides(len) {};
    T sdf(Node<T> n);
    T sdf(const Node<T>* n);
    Node<T> toObj(const Node<T>* n);
    const char* name();
    //T sdf(Node<T>* n, Node<T>* dir) = 0;
    ~cube(){delete center;};
};

template <typename T>
struct sample_scene : scene<T> {
    int overflow = 5;
    scene_obj<T>** objs = new scene_obj<T>*[overflow];
    int obj_count = 0;
    ~sample_scene(){delete[] objs;}
    void addObj(scene_obj<T>* obj);
    scene_obj<T>* removeObj(scene_obj<T>* obj);
    T sdf(const Node<T>* n) override;
    T sdf(const Node<T>* node, scene_obj<T>* &obj) override;
};

//definitions
/*
template <typename T>
glm::vec3 calc_normal(const glm::vec3 &node, const scene_obj<T>* &obj){
    const float eps = 0.0001f;
    const glm::vec2 k = glm::vec2(1, -1);
    return glm::normalize(k.xyy() * obj->sdf(node + k.xyy()*eps) +
                          k.yyx() * obj->sdf(node + k.yyx()*eps) +
                          k.yxy() * obj->sdf(node + k.yxy()*eps) +
                          k.xxx() * obj->sdf(node + k.xxx()*eps));

}
*/
/*
template <typename T>
Node<T> calc_normal(const Node<T>* node, const scene_obj<T>* &obj){
    auto normal = ((xyy * obj->sdf(node + xyy*eps)) +
                   (yyx * obj->sdf(node + yyx*eps)) +
                   (yxy * obj->sdf(node + yxy*eps)) +
                   (xxx * obj->sdf(node + xxx*eps)));
    return normalize(&normal);
}
*/
template <typename T>
T sphere<T>::sdf(const Node<T> node){
    return (*center - node).norm() - radius;
}

template <typename T>
T sphere<T>::sdf(const Node<T>* node) {
    return sdf(*node);
}

template <typename T>
Node<T> sphere<T>::toObj(const Node<T>* node){
    Node<T> new_n = *center - node;
    normalize(&new_n);
    return new_n;
}

template <typename T>
const char* sphere<T>::name(){
    return "Sphere";
}

template <typename T>
T rounded_box<T>::sdf(const Node<T> node){
    Node<T> tmp = node - center;
    Node<T> q = abs(&tmp) - lengths_to_sides;
    return (max(&q, (T)0) + std::min(std::max(std::max(q.y, q.z),q.x), (T)0)).norm() - round;
}

template <typename T>
T rounded_box<T>::sdf(const Node<T>* node) {
    return sdf(*node);
}

template <typename T>
Node<T> rounded_box<T>::toObj(const Node<T>* node){
    auto normal = ((xyy * sdf(node + xyy*eps)) +
                   (yyx * sdf(node + yyx*eps)) +
                   (yxy * sdf(node + yxy*eps)) +
                   (xxx * sdf(node + xxx*eps)));
    return -1.f * normalize(&normal);
}

template <typename T>
const char* rounded_box<T>::name(){
    return "Rounded Box";
}

template <typename T>
T cube<T>::sdf(const Node<T> node){
    Node<T> tmp = node - *center;
    Node<T> q = abs(&tmp) - len_to_sides;
    return (max(&q, (T)0) + std::min(std::max(std::max(q.y, q.z),q.x), (T)0)).norm();
}

template <typename T>
T cube<T>::sdf(const Node<T>* node) {
    return sdf(*node);
}

template <typename T>
Node<T> cube<T>::toObj(const Node<T>* node){
    auto normal = ((xyy * sdf(node + xyy*eps)) +
                   (yyx * sdf(node + yyx*eps)) +
                   (yxy * sdf(node + yxy*eps)) +
                   (xxx * sdf(node + xxx*eps)));
    return -1.f * normalize(&normal);
}

template <typename T>
const char* cube<T>::name(){
    return "Cube";
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
    for(int i = 0; i < obj_count; ++i){
        if(std::abs(objs[i]->sdf(node)) < std::abs(min_dist)){
            min_dist = objs[i]->sdf(node);
            obj = objs[i];
        }
    }
    return min_dist;
}

template <typename T>
void sample_scene<T>::addObj(scene_obj<T> *obj) {
    objs[obj_count] = obj;
    obj_count += 1;
    if(obj_count == overflow){
        overflow *= 2;
        auto new_objs = new scene_obj<T>*[overflow];
        for(int i = 0; i < obj_count; ++i){
            new_objs[i] = objs[i];
        }
        delete[] objs;
        objs = new_objs;
    }
}

template <typename T>
scene_obj<T>* sample_scene<T>::removeObj(scene_obj<T> *obj) {
    for(int i = 0; i < obj_count; ++i){
        if(objs[i] == obj){
            for(int j = i + 1; j < obj_count; ++j){
                objs[j-1] = objs[j];
            }
            obj_count -= 1;
            return obj;
        }
    }
    return nullptr;
}


#endif //MILT_SAMPLE_SCENE_H
