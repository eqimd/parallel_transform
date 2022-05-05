#include "transform/transform.h"
#include <vector>
#include <iostream>
#include <unistd.h>


int main(int argc, char* argv[]) {
    std::vector<int> v(10, 1);
    std::vector<int> v_res(10, 2);

    auto v_trans = TransformWithProcesses(v, [](int x) {return x + 1;}, 5, 2);

    if (v == v_res) {
        return 0;
    }

    return 1;
}