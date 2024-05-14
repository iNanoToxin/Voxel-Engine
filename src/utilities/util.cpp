#include "util.h"

std::string VoxelEngine::Util::read_file(const char* p_FilePath)
{
    VE_assert(p_FilePath != nullptr, "File path is nullptr.");

    std::stringstream stream;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        file.open(p_FilePath);
        stream << file.rdbuf();
        file.close();
    }
    catch (std::ifstream::failure& err)
    {
        VE_failure("Failed to open file.", err.what(), p_FilePath);
    }
    return stream.str();
}
