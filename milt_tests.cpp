//
// Created by maxwe on 2019-11-20.
//

#include <chrono>
#include <random>
#include <cstdio>
#include <iostream>
#include "node.h"
#include "octree.h"

#define F_TYPE float

Node<F_TYPE>* getRandomNodes(int n, F_TYPE min, F_TYPE max, int seed) {
    std::default_random_engine rand(seed);
    std::uniform_real_distribution<F_TYPE> dist(min, max);

    auto nodes = new Node<F_TYPE>[n];

    for (int i = 0; i < n; ++i) {
        nodes[i].x = dist(rand);
        nodes[i].y = dist(rand);
        nodes[i].z = dist(rand);
    }

    return nodes;
}


using namespace std::chrono;
using std::cout;
using std::cin;
using std::endl;

const int n[10] = { 1000, 2000, 4000, 8000, 16000, 32000, 64000, 128000, 256000, 512000 };
const int nearest[10] = {1, 3, 5, 10, 20, 40, 80, 160, 320, 640};
const int overflow[10] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};

const int seed = 123492984;
const int loops = 100;

const F_TYPE minval = (F_TYPE)0;
const F_TYPE maxval = (F_TYPE)1000;
const auto halfway = (minval + maxval) / (F_TYPE)2;

bool timings_run() {
    auto oct = new Octree<F_TYPE>(halfway, halfway, halfway, halfway);

    std::default_random_engine rand(seed);

    for (int over = 0; over < 10; ++over) {

        oct->reset(overflow[over]);

        long double times[10][10]{ {0.0} };

        for (int count = 0; count < 10; ++count) {

            int curr_n = n[count];
            auto nodes = getRandomNodes(curr_n, minval, maxval, seed + count);
            oct->addNodes(nodes, curr_n);

            std::uniform_int_distribution<int> dist(0, curr_n);

            for (int near = 0; near < 10; ++near) {

                int curr_nearest = nearest[near];
                microseconds accumulator(0);

                for (int l = 0; l < loops; ++l) {

                    auto rand_node = nodes + dist(rand);
                    int k = 0;
                    auto start = high_resolution_clock::now();
                    auto nearest_nodes = oct->getNearestNeighbours(rand_node, curr_nearest, k);
                    auto end = high_resolution_clock::now();
                    delete[] nearest_nodes;
                    accumulator += duration_cast<microseconds>(end - start);
                }
                times[count][near] = duration_cast<duration<double, std::micro>>(accumulator).count() / (double)loops;
            }

            oct->reset();
            delete[] nodes;
        }

        printf("				                         Overflow = %d\n", overflow[over]);
        printf("				                        # Nodes Searched\n	    ");
        for (int i : nearest) {
            printf(" %.8d", i);
        }
        printf("\n-------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < 10; ++i) {
            printf("%.6d |", n[i]);
            for (int j = 0; j < 10; ++j) {
                printf(" %8.3Lf", times[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }

    delete oct;
    return true;
}


const int node_count = 40000;
const int new_seed = 1234;
#include "sample_scene.h"

const auto center_point = new Node<F_TYPE>(halfway, halfway, halfway);
const auto test_bounding_cube = new cube<F_TYPE>(center_point, halfway);
const auto test_sphere_1 = new sphere<F_TYPE>(new Node<F_TYPE>(150, 300, 700), 100);
const auto test_sphere_2 = new sphere<F_TYPE>(new Node<F_TYPE>(800, 500, 850), 150);
const auto test_box_1 = new rounded_box<F_TYPE>(new Node<F_TYPE>(400, 800, 300), new Node<F_TYPE>(250, 100, 200), 30);
const auto test_box_2 = new rounded_box<F_TYPE>(new Node<F_TYPE>(600, 100, 100), new Node<F_TYPE>(70, 100, 150), 10);
const int obj_count = 5;

template <typename T>
bool node_optimization(Node<T>* nodes, int node_num){
    auto oct = new Octree<T>(halfway, halfway, halfway, halfway);

    //std::default_random_engine rand(seed);
    auto node_objects = new scene_obj<F_TYPE>*[node_num];

    auto example_scene = new sample_scene<F_TYPE>();
    scene_obj<F_TYPE> *objects[obj_count] = {test_bounding_cube, test_sphere_1, test_sphere_2, test_box_1, test_box_2};
    example_scene->objs = objects;
    example_scene->obj_count = obj_count;

    for(int i = 0; i < node_num; ++i){
        auto dist = example_scene->sdf(nodes + i, node_objects[i]);
        auto dir = node_objects[i]->toObj(nodes + i);
        nodes[i] += dist * dir;
    }

    for(int j = 0; j < 5; ++j){
        for(int i = 0; i < node_num; ++i){
            const auto dist = node_objects[i]->sdf(nodes + i);
            const auto dir = node_objects[i]->toObj(nodes + i);
            nodes[i] += dist * dir;
        }
    }

    oct->addNodes(nodes, node_num);

    for(int j = 0; j < 10; ++j){
        for(int i = 0; i < node_num; ++i){
            oct->remove(nodes + i);
            int size_n = 0;
            auto neighbours = oct->getNearestNeighbours(nodes + i, 5, size_n);

            auto force = Node<F_TYPE>();
            for(int k = 0; k < size_n; ++k){
                force += (nodes[i] - neighbours[k]) * (1 / dist_squared(nodes + i, neighbours[k]));
            }
            nodes[i] += force;

            const auto dist = node_objects[i]->sdf(nodes + i);
            const auto dir = node_objects[i]->toObj(nodes + i);
            nodes[i] += dist * dir;
            oct->addNode(nodes + i);
            delete[] neighbours;
        }
    }

    delete oct;
    delete example_scene;
    delete[] node_objects;
    return true;
}


bool node_optimization() {
    Node<F_TYPE>* nodes = getRandomNodes(node_count, minval, maxval, new_seed);
    bool result = node_optimization(nodes, node_count);
    delete[] nodes;
    return result;
}
