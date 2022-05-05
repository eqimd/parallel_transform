#include "transform/transform.h"
#include <vector>
#include <iostream>
#include <unistd.h>


int main() {
    std::vector<int> v(17, 1);
    std::vector<int> v_res(17, 2);

    auto v_trans = TransformWithProcesses(v, [](int x) {return x + 1;}, 20, 6);

    if (v == v_res) {
        return 0;
    }

    return 1;
}
