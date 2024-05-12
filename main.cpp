#include <iostream>

#include "window.h"

int main()
{
    VoxelEngine::Window window(2560, 1440, "Voxel Engine", true);

    while (!window.shouldClose())
    {
        window.clear(255, 70, 70);
        window.swap();
    }
    return 0;
}
