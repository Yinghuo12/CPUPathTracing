#include "shape/model.hpp"
#include "util/profile.hpp"
#include <rapidobj/rapidobj.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

// 模型加载
Model::Model(const std::filesystem::path &filename){
    PROFILE("Load model " + filename.string())
    auto result = rapidobj::ParseFile(filename, rapidobj::MaterialLibrary::Ignore());
    std::vector<Triangle> triangles;

    size_t index =0;
    for (const auto &shape : result.shapes) {
        size_t index_offset = 0;
        for (size_t num_face_vertex : shape.mesh.num_face_vertices) {
            if (num_face_vertex == 3) {
                // 获取三个顶点的索引
                auto index = shape.mesh.indices[index_offset];

                // 读取位置（显式转换为 float 确保 glm::vec3 构造成功）
                glm::vec3 pos0 {
                    result.attributes.positions[index.position_index * 3 + 0],
                    result.attributes.positions[index.position_index * 3 + 1],
                    result.attributes.positions[index.position_index * 3 + 2]
                };
                index = shape.mesh.indices[index_offset + 1];
                glm::vec3 pos1 {
                    result.attributes.positions[index.position_index * 3 + 0],
                    result.attributes.positions[index.position_index * 3 + 1],
                    result.attributes.positions[index.position_index * 3 + 2]
                };
                index = shape.mesh.indices[index_offset + 2];
                glm::vec3 pos2 {
                    result.attributes.positions[index.position_index * 3 + 0],
                    result.attributes.positions[index.position_index * 3 + 1],
                    result.attributes.positions[index.position_index * 3 + 2]
                };
                // 检查是否有法线（假如三个顶点都有法线或都没有，取 idx0 判断）
                if (index.normal_index >= 0) {
                    index = shape.mesh.indices[index_offset];
                    glm::vec3 normal0 {
                        result.attributes.normals[index.normal_index * 3 + 0],
                        result.attributes.normals[index.normal_index * 3 + 1],
                        result.attributes.normals[index.normal_index * 3 + 2]
                    };
                    index = shape.mesh.indices[index_offset + 1];
                    glm::vec3 normal1 {
                        result.attributes.normals[index.normal_index * 3 + 0],
                        result.attributes.normals[index.normal_index * 3 + 1],
                        result.attributes.normals[index.normal_index * 3 + 2]
                    };
                    index = shape.mesh.indices[index_offset + 2];
                    glm::vec3 normal2 {
                        result.attributes.normals[index.normal_index * 3 + 0],
                        result.attributes.normals[index.normal_index * 3 + 1],
                        result.attributes.normals[index.normal_index * 3 + 2]
                    };
                    triangles.push_back(Triangle{pos0, pos1, pos2, normal0, normal1, normal2});  // 有法线信息
                } else{
                    triangles.push_back(Triangle{pos0, pos1, pos2});  // 没有法线信息
                }
            }
            index_offset += num_face_vertex;
        }
    }
    bvh.build(std::move(triangles));  // 构建包围盒
}
// Model::Model(const std::filesystem::path &filename){
//     PROFILE("Load model " + filename.string())
    
//     // v 22 12 12
//     std::vector<glm::vec3> positions;
//     // vn 22 12 12
//     std::vector<glm::vec3> normals;
//     // f 0//3 1//2 2//1   顶点索引//法线索引

//     std::ifstream file(filename);
//     if(!file.good()){
//         std::cerr << "Failed to open file: " << filename << std::endl;
//         return;
//     }

//     std::string line;
//     char trash;   // trash用于读取char类型，position用于读取数字
//     while(!file.eof()){
//         std::getline(file, line);
//         std::istringstream iss(line);

//         if (line.compare(0, 2, "v ") == 0){  // 顶点位置信息
//             glm::vec3 position;
//             iss >> trash >> position.x >> position.y >> position.z;  // trash='v'
//             positions.push_back(position);
//         }
//         if (line.compare(0, 3, "vn ") == 0){   // 顶点法线信息
//             glm::vec3 normal;
//             iss >> trash >> trash >> normal.x >> normal.y >> normal.z; // trash='vn'
//             normals.push_back(normal);
//         } else if(line.compare(0, 2, "f ") == 0) { // 三角形面信息
//             // f 0//3 1//2 2//1
//             // T{0,1,2 3,2,1}
//             glm::ivec3 idx_v, idx_vn;
//             iss >> trash;  // trash='f'
//             iss >> idx_v.x >> trash >> trash >> idx_vn.x;
//             iss >> idx_v.y >> trash >> trash >> idx_vn.y;
//             iss >> idx_v.z >> trash >> trash >> idx_vn.z;

//             triangles.push_back(Triangle(
//                 // obj文件的索引从1开始,因此需要减1
//                 positions[idx_v.x - 1], positions[idx_v.y - 1], positions[idx_v.z - 1], 
//                 normals[idx_vn.x - 1], normals[idx_vn.y - 1], normals[idx_vn.z - 1]
//             ));
//         }
//     }
//     build();  // 构建包围盒
// } 

// 相交测试
std::optional<HitInfo> Model::intersect(const Ray &ray, float t_min, float t_max) const {
    return bvh.intersect(ray, t_min, t_max);
}
