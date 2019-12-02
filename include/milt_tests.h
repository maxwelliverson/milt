//
// Created by maxwe on 2019-11-20.
//

#include <chrono>
#include <random>
#include <cstdio>
#include <iostream>
#include "node.h"
#include "octree.h"

template <typename T>
Node<T>* getRandomNodes(int n, T min, T max, int seed) {
    std::default_random_engine rand(seed);
    std::uniform_real_distribution<T> dist(min, max);

    auto nodes = new Node<T>[n];

    for (int i = 0; i < n; ++i) {
        nodes[i].x = dist(rand);
        nodes[i].y = dist(rand);
        nodes[i].z = dist(rand);
    }

    return nodes;
}

/*
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
*/
#include "sample_scene.h"
#include "milt_macro.h"


template <typename T>
bool node_optimization(sample_scene<T> *active_scene, Octree<T> *oct, Node<T> *nodes, int node_num, int rounds,
                       int sdf_rounds, int post_sdf_rounds, int nearest_neighbours, int force_factor) {
    auto node_objects = new scene_obj<T>*[node_num];

    for(int i = 0; i < node_num; ++i){
        oct->remove(nodes + i);
        auto dist = active_scene->sdf(nodes + i, node_objects[i]);
        auto dir = node_objects[i]->toObj(nodes + i);
        nodes[i] += dist * dir;
    }

    for(int j = 0; j < sdf_rounds; ++j){
        for(int i = 0; i < node_num; ++i){
            auto dist = node_objects[i]->sdf(nodes + i);
            auto dir = node_objects[i]->toObj(nodes + i);
            nodes[i] += dist * dir;
        }
    }

    oct->addNodes(nodes, node_num);

    for(int j = 0; j < rounds; ++j){
        for(int i = 0; i < node_num; ++i){

			if (oct->remove(nodes + i)) {
				int size_n = 0;
				auto neighbours = oct->getNearestNeighbours(nodes + i, nearest_neighbours, size_n);

				auto force = Node<T>();
				for (int k = 0; k < size_n; ++k) {
				    auto dir = nodes[i] - neighbours[k];
					force += (force_factor / (dir.norm_squared() + 0.0001f)) * dir;
				}
				nodes[i] += force;

				for (int k = 0; k < post_sdf_rounds; ++k) {
					const auto dist = node_objects[i]->sdf(nodes + i);
					const auto dir = node_objects[i]->toObj(nodes + i);
					nodes[i] += dist * dir;
				}

				oct->addNode(nodes + i);
				delete[] neighbours;
			} else {
			    MILT_ERR("Node was unsuccessfully removed from Octree.");
			}
        }
    }

    delete[] node_objects;
    return true;
}