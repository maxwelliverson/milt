//
// Created by maxwe on 2019-11-19.
//

#ifndef MILT_SCENE_H
#define MILT_SCENE_H

#include "node.h"


template <typename T>
class scene_obj {
public:
    virtual T sdf(Node<T> n) = 0;
    virtual T sdf(const Node<T>* n) = 0;
    virtual Node<T> toObj(const Node<T>* n) = 0;
    virtual const char* name() = 0;
    virtual ~scene_obj() = default;
    //virtual T sdf(Node<T>* n, Node<T>* dir) = 0;
};

template <typename T>
class scene {
    //scene_obj<T>* objects;
public:
    virtual T sdf(const Node<T>* n) = 0;
    virtual T sdf(const Node<T>* n, scene_obj<T>* &obj) = 0;
};

#endif //MILT_SCENE_H
