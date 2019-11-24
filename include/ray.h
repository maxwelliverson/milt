//
// Created by maxwe on 2019-11-19.
//

#ifndef MILT_RAY_H
#define MILT_RAY_H

#include "node.h"

template <typename T>
struct ray {
    Node<T>* origin;
    Node<T>* dir;
};

#endif //MILT_RAY_H
