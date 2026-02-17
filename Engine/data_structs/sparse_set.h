#include<vector>
// A structure to hold the three parameters required to
// represent a sparse set.

// NOTE: id_t is usually an entity struct that hold entity id and generation
//       while data_t is usually the component struct
template<typename T>
struct SparseSet
{

    // CONSTANTS
    constexpr NULL_INDEX = -1;

    std::vector<size_t> sparse;   // To store indexes of actual elements
    std::vector<T>      dense;    // To store actual set elements
    int n;         // Current number of elements (in dense)
    int maxValue;  /* Maximum value in set or size of
                     sparse[] */

    // Initialization
    void Init(int maxV=100, int cap=5)
    {
        sparse = new vector<size_t>();
        dense  = new vector<T>();
        maxValue = maxV;
        n = 0;  // No elements initially
    }

    // De-initialization
    void Free()
    {
        delete sparse;
        delete dense;
    }

    // Inserts a new element into set
    void Insert(size_t id, T data) {
        sparse[id] = dense.size();
        dense.push_back(data);
    }

    T* Get(size_t id) {
        size_t index = sparse[id];
        if (index != NULL_INDEX)
            return &dense[index];
        return nullptr; // no components at that id.
    }

    // Deletes an element
    void Delete(size_t id) {
        size_t index = sparse[id];

        if (index == NULL_INDEX) return;
    
        size_t back_entity = dense.size();
        std::swap(dense[index], dense.back());

        sparse[id] = NULL_INDEX;
        sparse[back_entity] = index;

        dense.pop_back();
    }

    // Removes all elements from set
    void Clear() { n = 0; sparse.clear(); dense.clear();}
};

