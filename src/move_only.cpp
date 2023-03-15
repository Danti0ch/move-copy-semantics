#include <iostream>
#include <vector>
#include "nstd.h"
#include <utility>

std::vector<std::vector<int>> secret_storage = {
    {0, 0, 0, 0},
    {1, 2, 3, 4}
};

template<class Container, typename T>
void insert_key_wrong(Container& container, T&& value) {
    container.push_back(nstd::move<T>(value));
}

template<class Container, typename T>
void insert_key_correct(Container& container, T&& value) {
    container.push_back(nstd::forward<T>(value));
}

void print_key(const std::vector<int>& key) {
    for(auto i : key)
        std::cout << i << " ";
    std::cout << "\n";
}

int main() { 

    std::vector<int> secret_key1 = {884422, 0, 0, 0};
    print_key(secret_key1);

    insert_key_wrong(secret_storage, secret_key1);
    print_key(secret_key1);

    //--------------------------------------------------------

    std::vector<int> secret_key2 = {0, 0, 0, 5432};
    print_key(secret_key2);

    insert_key_correct(secret_storage, secret_key2);
    print_key(secret_key2);

    return 0;
}
