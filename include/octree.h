//
// Created by maxwe on 2019-11-19.
//

#ifndef MILT_OCTREE_H
#define MILT_OCTREE_H

#include "node.h"
#include <atomic>
using std::atomic;

template <typename T, int N = 20>
class Octree
{
    static unsigned overflow_lim;
    unsigned node_count = 0;

    Node<T>** values = new Node<T> * [overflow_lim];
    Octree<T>** branches = nullptr;

    Octree<T>* parent;
    unsigned char index;

    Node<T>* center;
    T sidelength;

public:
    Octree(Node<T>* c, T len);
    Octree(T x, T y, T z, T len);
    Octree(Node<T>* c, T len, Octree<T>* p, char i);
    ~Octree();

    void addNode(Node<T>* val);
    void addNodes(Node<T>* val, int n);
    bool remove(Node<T>* val);
    Node<T>** getNearestNeighbours(Node<T>* ref_node, int k, int& n);
    Node<T>** getNodes(int &n);
    bool isCorrectBox(Node<T>* node);
    void reset();
    void reset(unsigned overflow);

private:
    char get_index(Node<T>* val);
    void trickle_down(Node<T>* val);
    void subdivide();
    void addToArr(Node<T>** nodes, Node<T>* new_node, Node<T>* ref, T& max_dist, int& max_ind, int n, int& curr_n);
    void addAllInBranch(Octree<T, N>* oct, Node<T>** nodes, Node<T>* ref, T& max_dist, int& max_ind, int n, int& curr_n);
    void gatherNodes(Node<T>** nodes, int z, int y, int x, int depth, Node<T>* ref, T& max_dist, int& max_ind, int n, int& curr_n, Octree<T>* searched[], int& searched_n);
};

template <typename T, int N> inline unsigned Octree<T, N>::overflow_lim = N;

#include <cmath>
#include <limits>
#include "neighbours.h"

template <typename T, int N>
Octree<T, N>::Octree(Node<T>* c, T len) : parent(nullptr), index(0xff), center(c), sidelength(len) {};

template <typename T, int N>
Octree<T, N>::Octree(T x, T y, T z, T len) : parent(nullptr), index(0xff), sidelength(len)
{
    center = new Node<T>(x, y, z);
};

template <typename T, int N>
Octree<T, N>::Octree(Node<T>* c, T len, Octree<T>* p, char i) : parent(p), index(i)
{
    sidelength = len / T(2.0);
    center = new Node<T>(
            c->x + (index & 0x01 ? sidelength : -sidelength),
            c->y + (index & 0x02 ? sidelength : -sidelength),
            c->z + (index & 0x04 ? sidelength : -sidelength));
};

template <typename T, int N>
Octree<T, N>::~Octree() {
    delete center;
    if (branches != nullptr) {
        for (int i = 0; i < 8; i++) {
            delete branches[i];
        }
        delete[] branches;
    }
    else {
        delete[] values;
    }
}

template <typename T, int N>
void Octree<T, N>::addNode(Node<T>* val) {
    if (node_count < overflow_lim) {
        values[node_count] = val;
    }
    else if (node_count == overflow_lim) {
        subdivide();
        trickle_down(val);
    }
    else
        trickle_down(val);
    node_count += 1;
}

template <typename T, int N>
void Octree<T, N>::addNodes(Node<T>* vals, int n) {
    for (int i = 0; i < n; ++i)
        addNode(vals + i);
}

template <typename T, int N>
bool Octree<T, N>::remove(Node<T>* val) {
    if (node_count > overflow_lim) {
        bool success = branches[(int)get_index(val)]->remove(val);
        if (node_count == overflow_lim + 1) {
            int k;
            values = getNodes(k);
            for (int i = 0; i < 8; ++i) {
                delete branches[i];
            }
            delete[] branches;
            branches = nullptr;
        }
		return success && --node_count;
    }
    else {
        unsigned i;
        for (i = 0; i < node_count; ++i) {
            if (values[i] == val)
                break;
        }
        if (i == node_count)
            return false;

        values[i] = values[node_count - 1];
        values[node_count - 1] = nullptr;
        node_count -= 1;
		return true;
    }
}

template <typename T, int N>
void Octree<T, N>::reset() {
    if (branches != nullptr) {
        for (int i = 0; i < 8; i++) {
            delete branches[i];
        }
        delete[] branches;
        branches = nullptr;
    }
    else {
        delete[] values;
    }

    node_count = 0;
    values = new Node<T> * [overflow_lim];
}

template <typename T, int N>
void Octree<T, N>::reset(unsigned overflow) {
    overflow_lim = overflow;
    reset();
}

template <typename T, int N>
Node<T>** Octree<T, N>::getNearestNeighbours(Node<T>* ref_node, int k, int& n) {
    if (node_count > overflow_lim) {
        char i = get_index(ref_node);
        return branches[i]->getNearestNeighbours(ref_node, k, n);
    }
    auto working_arr = new Node<T> * [k];
    T max_dist = std::numeric_limits<T>::min();
    int max_ind = -1;
    if (parent != nullptr) {
        addAllInBranch(parent, working_arr, ref_node, max_dist, max_ind, k, n);
    }
    else {
        addAllInBranch(this, working_arr, ref_node, max_dist, max_ind, k, n);
        return working_arr;
    }

    int x = 0;
    int y = 0;
    int z = 0;
    int depth = 0;
    Octree<T>* current_branch = this;

    while (current_branch != nullptr && current_branch->parent != nullptr) {
        x += (current_branch->index & 0b001) << depth;
        y += ((current_branch->index & 0b010) >> 1) << depth;
        z += ((current_branch->index & 0b100) >> 2) << depth;
        current_branch = current_branch->parent;
        ++depth;
    }

    --depth;

    Octree<T>* searched[19];
    int searched_n = 0;

    auto indices = NEIGHBOURS[index];
    for (int ind = 0; ind < 19; ++ind) {
        auto relative_node = indices[ind];
        if ((((z + relative_node[0]) >> depth) & 0xfffe) != 0 || (((y + relative_node[1]) >> depth) & 0xfffe) != 0 || (((x + relative_node[2]) >> depth) & 0xfffe) != 0) {
            continue;
        }
        current_branch->gatherNodes(working_arr, z + relative_node[0], y + relative_node[1], x + relative_node[2],
                                    depth, ref_node, max_dist, max_ind, k, n, searched, searched_n);
    }

    return working_arr;
}

template <typename T, int N>
Node<T>** Octree<T, N>::getNodes(int &n) {

    n = node_count;
    Node<T>** node_list = new Node<T> * [n];

    if (node_count > overflow_lim) {
        int i = 0;
        int sub_n;
        for (int sub_index = 0; sub_index < 8; ++sub_index) {
            Node<T>** sub_nodes = branches[sub_index]->getNodes(sub_n);
            for (int k = 0; k < sub_n; ++k) {
                node_list[i] = sub_nodes[k];
                i += 1;
            }
            delete[] sub_nodes;
        }
    }
    else {
        for (int i = 0; i < n; ++i) {
            node_list[i] = values[i];
        }
    }
    return node_list;
}

template <typename T, int N>
bool Octree<T, N>::isCorrectBox(Node<T>* node) {
    Octree<T, N>* tree = this;
    while (tree->node_count > overflow_lim) {
        tree = tree->branches[tree->get_index(node)];
    }
    for (int i = 0; i < tree->node_count; ++i) {
        if (tree->values[i] == node) {
            return true;
        }
    }
    return false;
}

template <typename T, int N>
char Octree<T, N>::get_index(Node<T>* val) {
	char ind = (val->x >= center->x) |
		((val->y >= center->y) << 1) |
		((val->z >= center->z) << 2);
	/*if (int(val->x) == 169 && int(val->y) == 391) {
		printf("New Index: %d\n", ind);
	}*/
	return ind;
}

template <typename T, int N>
void Octree<T, N>::trickle_down(Node<T>* val) {
    char i = 0x7 & get_index(val);
    branches[i]->addNode(val);
}

template <typename T, int N>
void Octree<T, N>::subdivide() {
    branches = new Octree<T, N> * [8];
    for (int i = 0; i < 8; i++) {
        branches[i] = new Octree<T, N>(center, sidelength, this, i);
    }
    for (unsigned i = 0; i < node_count; i++) {
        trickle_down(values[i]);
    }
    delete[] values;
	values = nullptr;
}

template <typename T, int N>
void Octree<T, N>::addToArr(Node<T>** nodes, Node<T>* new_node, Node<T>* ref, T& max_dist, int& max_ind, int n, int& curr_n) {
    if (curr_n < n && new_node != ref) {
        nodes[curr_n] = new_node;
        T d_s = dist_squared(new_node, ref);
        if (d_s > max_dist) {
            max_dist = d_s;
            max_ind = curr_n;
        }
        curr_n++;
        return;
    }
    else {
        if (dist_squared(new_node, ref) < max_dist && new_node != ref) {
            nodes[max_ind] = new_node;
            max_dist = dist_squared(new_node, ref);
            for (int i = 0; i < curr_n; i++) {
                T d_s = dist_squared(nodes[i], ref);
                if (d_s > max_dist) {
                    max_dist = d_s;
                    max_ind = i;
                }
            }
        }

    }
}

template <typename T, int N>
void Octree<T, N>::addAllInBranch(Octree<T, N>* oct, Node<T>** nodes, Node<T>* ref, T& max_dist, int& max_ind, int n, int& curr_n) {
    if (oct->node_count > overflow_lim) {
        for (int i = 0; i < 8; i++) {
            addAllInBranch(oct->branches[i], nodes, ref, max_dist, max_ind, n, curr_n);
        }
    }
    else {
        for (unsigned i = 0; i < oct->node_count; i++)
            addToArr(nodes, oct->values[i], ref, max_dist, max_ind, n, curr_n);
    }
}

template <typename T, int N>
void Octree<T, N>::gatherNodes(Node<T>** nodes, int z, int y, int x, int depth, Node<T>* ref, T& max_dist, int& max_ind, int n, int& curr_n, Octree<T>* searched[], int& searched_n) {
    if (node_count <= overflow_lim || depth < 0) {
        for (int i = 0; i < searched_n; ++i) {
            if (this == searched[i])
                return;
        }
        addAllInBranch(this, nodes, ref, max_dist, max_ind, n, curr_n);
        searched[searched_n] = this;
        ++searched_n;
        return;
    }
    const bool zbit = 0x01U & (z >> depth);
    const bool ybit = 0x01U & (y >> depth);
    const bool xbit = 0x01U & (x >> depth);
    int next_index = (zbit << 2) | (ybit << 1) | (xbit << 0);
    branches[next_index]->gatherNodes(nodes, z, y, x, depth - 1, ref, max_dist, max_ind, n, curr_n, searched, searched_n);
}

template <typename T>
T dist_squared(Node<T>* n1, Node<T>* n2);

#endif //MILT_OCTREE_H
