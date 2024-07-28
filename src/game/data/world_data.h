#pragma once
#include <glm/glm.hpp>
#include "utilities/json.h"
#include "utilities/util.h"



namespace voxel_engine
{
    class world_data
    {
    private:
        const char* _path;
        nlohmann::json _data;
    public:
        explicit world_data(const char* _path)
        {
            VE_assert(_path != nullptr, "World data path is `nullptr`.", _path);
            _data = nlohmann::json::parse(util::read_file(_path));
            this->_path = _path;
        }

        template<typename T>
        void set(const char* _name, const T& _value)
        {
            _data[_name] = _value;
        }

        template<typename T>
        T get(const char* _name)
        {
            if (_data[_name] == nullptr)
            {
                return T{};
            }
            return _data[_name];
        }

        void set_vec3(const char* _name, glm::vec3 _vec3)
        {
            _data[_name]["x"] = _vec3.x;
            _data[_name]["y"] = _vec3.y;
            _data[_name]["z"] = _vec3.z;
        }

        glm::vec3 get_vec3(const char* _name)
        {
            if (_data[_name] == nullptr)
            {
                return glm::vec3(0.0);
            }
            return glm::vec3(_data[_name]["x"], _data[_name]["y"], _data[_name]["z"]);
        }

        void save() const
        {
            util::write_file(_path, _data.dump(4));
        }
    };
}
