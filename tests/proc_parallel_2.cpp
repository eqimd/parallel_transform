#include "transform/transform.h"
#include <vector>
#include <iostream>
#include <unistd.h>
#include <chrono>


int main() {
    std::vector<int> v(20, 1);

    auto begin = std::chrono::steady_clock::now();
    auto v_trans = TransformWithProcesses(v, [](int x) {sleep(1); return x + 1;}, 5, 4);
    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    if (diff < 7000) {
        return 0;
    }

    return 1;
}
