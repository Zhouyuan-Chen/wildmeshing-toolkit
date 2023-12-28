#include "marching.hpp"

#include <wmtk/TriMesh.hpp>
#include <wmtk/io/HDF5Writer.hpp>
#include <wmtk/io/MeshReader.hpp>

#include "internal/Marching.hpp"
#include "internal/MarchingOptions.hpp"

namespace wmtk {
namespace components {
void marching(const nlohmann::json& j, io::Cache& cache)
{
    using namespace internal;

    MarchingOptions options = j.get<MarchingOptions>();

    // input
    std::shared_ptr<Mesh> mesh_in = cache.read_mesh(options.input);


    Mesh& mesh = static_cast<Mesh&>(*mesh_in);

    const auto& [input_tag_attr_name, input_tag_value_1, input_tag_value_2] = options.input_tags;

    MeshAttributeHandle<long> vertex_tag_handle =
        mesh.get_attribute_handle<long>(input_tag_attr_name, PrimitiveType::Vertex);

    std::tuple<MeshAttributeHandle<long>, long, long> vertex_tags =
        std::make_tuple(vertex_tag_handle, input_tag_value_1, input_tag_value_2);

    std::vector<std::tuple<MeshAttributeHandle<long>, long>> edge_filter_tags;
    for (const auto& [name, value] : options.edge_filter_tags) {
        MeshAttributeHandle<long> handle =
            mesh.get_attribute_handle<long>(name, PrimitiveType::Edge);
        edge_filter_tags.emplace_back(std::make_tuple(handle, value));
    }

    switch (mesh.top_cell_dimension()) {
    case 2:
    case 3: {
        Marching mc(mesh, vertex_tags, options.output_vertex_tag, edge_filter_tags);
        mc.process();
    } break;
    default: throw std::runtime_error("dimension setting error!"); break;
    }

    cache.write_mesh(*mesh_in, options.output);
}
} // namespace components
} // namespace wmtk
