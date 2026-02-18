#include <iostream>
#include <stdint.h>
#include<vector>
// A structure to hold the three parameters required to
// represent a sparse set.

// NOTE: id_t is usually an entity struct that hold entity id and generation
//       while data_t is usually the component struct
#define SPARSE_SET_NULL_INDEX -1
template<typename T>
struct SparseSet
{
    std::vector<int> sparse;   // To store indexes of actual elements
    std::vector<T>   dense = {};    // To store actual set elements
    int n;         // Current number of elements (in dense)
    int maxValue;  /* Maximum value in set or size of
                     sparse[] */

                     // Initialization
    void Init(int maxV = 100, int cap = 5)
    {
        sparse.assign(maxV, SPARSE_SET_NULL_INDEX);
        maxValue = maxV;
        n = 0;  // No elements initially
    }

    // De-initialization
    void Free()
    {
        // TODO
    }

    // Inserts a new element into set
    void Insert(int id, T data) {
        sparse.insert(sparse.begin() + id, dense.size());
        dense.push_back(data);
    }

    T* Get(int id) {
        int index = sparse[id];
        if (index != SPARSE_SET_NULL_INDEX)
            return &dense[index];
        return nullptr; // no components at that id.
    }

    // Deletes an element
    //FIXME: now working for some reason
    void Delete(int id) {
        int index = sparse[id];

        if (dense.size() <= 1) { Clear(); return; }
        if (index == dense.size() - 1) {
            dense.pop_back();
            sparse[id] = SPARSE_SET_NULL_INDEX;
            return;
        }
        if (index == SPARSE_SET_NULL_INDEX) return;

        int back_entity = dense.size();
        std::swap(dense[index], dense.back());

        sparse[id] = SPARSE_SET_NULL_INDEX;
        sparse[back_entity] = index;

        dense.pop_back();
    }

    // Removes all elements from set
    void Clear() { n = 0; sparse.clear(); dense.clear(); }
};

struct Entity {
    uint32_t id;
    uint32_t generation;
};

struct Component {
    int a;
    int b;
};


void print_vec(std::vector<int> v) {
    // Printing elements of vector
    std::cout << "sparse: ";
    for (auto i : v)
        std::cout << i << " ";
    std::cout << std::endl;
}

void print_vec(std::vector<Component> v) {

    // Printing elements of vector
    std::cout << "dense: ";
    for (auto i : v)
        std::cout << "{" << i.a << "," << i.b << "} ";
    std::cout << std::endl;
}

void print_set(SparseSet<Component> ss) {
    std::cout << "==============SET============" << "\n";
    std::cout << "N:" << ss.n << " maxV:" << ss.maxValue << " dense.size:" << ss.dense.size() << "\n";
    print_vec(ss.dense);
    print_vec(ss.sparse);
    std::cout << "=============================" << "\n";

}

int main()
{
    SparseSet<Component> comp_arr;
    comp_arr.Init(10, 5);

    print_set(comp_arr);

    comp_arr.Insert(int(0), Component{ 0, 0 });
    comp_arr.Insert(int(1), Component{ 1, 1 });
    comp_arr.Insert(int(5), Component{ 5, 1239 });

    print_set(comp_arr);

    std::cout << "Element 3 Get: " << ((nullptr == comp_arr.Get(3)) ? "invalid" : "valid") << std::endl;
    std::cout << "Element 2 Get: " << ((nullptr == comp_arr.Get(2)) ? "invalid" : "valid") << std::endl;
    std::cout << "Element 2 Get: " << ((nullptr == comp_arr.Get(1)) ? "invalid" : "valid") << std::endl;

    print_set(comp_arr);

    std::cout << "======Deleting========" << std::endl;
    comp_arr.Delete(1);
    print_set(comp_arr);
    //comp_arr.Delete(1);
    //print_set(comp_arr);
    //comp_arr.Delete(0);
    //print_set(comp_arr);

    return 0;
}
