#include "EdgeOperationData.hpp"
#include <wmtk/TriMesh.hpp>


namespace wmtk::operations::tri_mesh {


std::vector<std::array<Tuple, 2>> EdgeOperationData::ear_edges(const TriMesh& m) const
{
    std::vector<std::array<Tuple, 2>> ret;
    ret.reserve(incident_face_datas().size());

    for (const auto& ifd : incident_face_datas()) {
        std::array<Tuple, 2>& r = ret.emplace_back();

        for (size_t j = 0; j < 2; ++j) {
            long eid = ifd.ears[j].eid;
            r[j] = tuple_from_id(m, PrimitiveType::Edge, eid);
        }
    }
    return ret;
}
std::array<Tuple, 2> EdgeOperationData::input_endpoints(const TriMesh& m) const
{
    std::array<Tuple, 2> r;
    r[0] = m_operating_tuple;
    r[1] = m.switch_tuple(m_operating_tuple, PrimitiveType::Vertex);
    return r;
}

std::vector<Tuple> EdgeOperationData::collapse_merged_ear_edges(const TriMesh& m) const
{
    std::vector<Tuple> ret;
    ret.reserve(incident_face_datas().size());

    for (const auto& ifd : incident_face_datas()) {
        ret.emplace_back(tuple_from_id(m, PrimitiveType::Edge, ifd.collapse_new_edge_id));
    }
    return ret;
}
} // namespace wmtk::operations::tri_mesh
