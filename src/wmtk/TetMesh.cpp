#include "TetMesh.hpp"


#include <wmtk/utils/tetmesh_topology_initialization.h>
#include <wmtk/autogen/tet_mesh/autogenerated_tables.hpp>
#include <wmtk/autogen/tet_mesh/is_ccw.hpp>
#include <wmtk/autogen/tet_mesh/local_switch_tuple.hpp>
#include <wmtk/simplex/SimplexCollection.hpp>
#include <wmtk/simplex/cofaces_single_dimension.hpp>
#include <wmtk/simplex/open_star.hpp>
#include <wmtk/utils/Logger.hpp>

namespace wmtk {

using namespace autogen;

TetMesh::TetMesh()
    : Mesh(3)
    , m_vt_handle(register_attribute_typed<int64_t>("m_vt", PrimitiveType::Vertex, 1, false, -1))
    , m_et_handle(register_attribute_typed<int64_t>("m_et", PrimitiveType::Edge, 1, false, -1))
    , m_ft_handle(register_attribute_typed<int64_t>("m_ft", PrimitiveType::Triangle, 1, false, -1))
    , m_tv_handle(
          register_attribute_typed<int64_t>("m_tv", PrimitiveType::Tetrahedron, 4, false, -1))
    , m_te_handle(
          register_attribute_typed<int64_t>("m_te", PrimitiveType::Tetrahedron, 6, false, -1))
    , m_tf_handle(
          register_attribute_typed<int64_t>("m_tf", PrimitiveType::Tetrahedron, 4, false, -1))
    , m_tt_handle(
          register_attribute_typed<int64_t>("m_tt", PrimitiveType::Tetrahedron, 4, false, -1))
{
    make_cached_accessors();
}


TetMesh::TetMesh(TetMesh&& o)
    : Mesh(std::move(o))
{
    m_vt_handle = o.m_vt_handle;
    m_et_handle = o.m_et_handle;
    m_ft_handle = o.m_ft_handle;
    m_tv_handle = o.m_tv_handle;
    m_te_handle = o.m_te_handle;
    m_tf_handle = o.m_tf_handle;
    m_tt_handle = o.m_tt_handle;

    make_cached_accessors();
}
TetMesh& TetMesh::operator=(TetMesh&& o)
{
    Mesh::operator=(std::move(o));
    m_vt_handle = o.m_vt_handle;
    m_et_handle = o.m_et_handle;
    m_ft_handle = o.m_ft_handle;
    m_tv_handle = o.m_tv_handle;
    m_te_handle = o.m_te_handle;
    m_tf_handle = o.m_tf_handle;
    m_tt_handle = o.m_tt_handle;

    make_cached_accessors();
    return *this;
}


void TetMesh::make_cached_accessors()
{
    m_vt_accessor = std::make_unique<attribute::MutableAccessor<int64_t>>(*this, m_vt_handle);
    m_et_accessor = std::make_unique<attribute::MutableAccessor<int64_t>>(*this, m_et_handle);
    m_ft_accessor = std::make_unique<attribute::MutableAccessor<int64_t>>(*this, m_ft_handle);

    m_tv_accessor = std::make_unique<attribute::MutableAccessor<int64_t>>(*this, m_tv_handle);
    m_te_accessor = std::make_unique<attribute::MutableAccessor<int64_t>>(*this, m_te_handle);
    m_tf_accessor = std::make_unique<attribute::MutableAccessor<int64_t>>(*this, m_tf_handle);
    m_tt_accessor = std::make_unique<attribute::MutableAccessor<int64_t>>(*this, m_tt_handle);
}


void TetMesh::initialize(
    Eigen::Ref<const RowVectors4l> TV,
    Eigen::Ref<const RowVectors6l> TE,
    Eigen::Ref<const RowVectors4l> TF,
    Eigen::Ref<const RowVectors4l> TT,
    Eigen::Ref<const VectorXl> VT,
    Eigen::Ref<const VectorXl> ET,
    Eigen::Ref<const VectorXl> FT)

{
    // reserve memory for attributes

    std::vector<int64_t> cap{
        static_cast<int64_t>(VT.rows()),
        static_cast<int64_t>(ET.rows()),
        static_cast<int64_t>(FT.rows()),
        static_cast<int64_t>(TT.rows())};
    set_capacities(cap);

    // get Accessors for topology
    Accessor<int64_t> vt_accessor = create_accessor<int64_t>(m_vt_handle);
    Accessor<int64_t> et_accessor = create_accessor<int64_t>(m_et_handle);
    Accessor<int64_t> ft_accessor = create_accessor<int64_t>(m_ft_handle);

    Accessor<int64_t> tv_accessor = create_accessor<int64_t>(m_tv_handle);
    Accessor<int64_t> te_accessor = create_accessor<int64_t>(m_te_handle);
    Accessor<int64_t> tf_accessor = create_accessor<int64_t>(m_tf_handle);
    Accessor<int64_t> tt_accessor = create_accessor<int64_t>(m_tt_handle);

    Accessor<char> v_flag_accessor = get_flag_accessor(PrimitiveType::Vertex);
    Accessor<char> e_flag_accessor = get_flag_accessor(PrimitiveType::Edge);
    Accessor<char> f_flag_accessor = get_flag_accessor(PrimitiveType::Triangle);
    Accessor<char> t_flag_accessor = get_flag_accessor(PrimitiveType::Tetrahedron);

    // iterate over the matrices and fill attributes
    for (int64_t i = 0; i < capacity(PrimitiveType::Tetrahedron); ++i) {
        tv_accessor.index_access().vector_attribute(i) = TV.row(i).transpose();
        te_accessor.index_access().vector_attribute(i) = TE.row(i).transpose();
        tf_accessor.index_access().vector_attribute(i) = TF.row(i).transpose();
        tt_accessor.index_access().vector_attribute(i) = TT.row(i).transpose();
        t_flag_accessor.index_access().scalar_attribute(i) |= 0x1;
    }
    // m_vt
    for (int64_t i = 0; i < capacity(PrimitiveType::Vertex); ++i) {
        vt_accessor.index_access().scalar_attribute(i) = VT(i);
        v_flag_accessor.index_access().scalar_attribute(i) |= 0x1;
    }
    // m_et
    for (int64_t i = 0; i < capacity(PrimitiveType::Edge); ++i) {
        et_accessor.index_access().scalar_attribute(i) = ET(i);
        e_flag_accessor.index_access().scalar_attribute(i) |= 0x1;
    }
    // m_ft
    for (int64_t i = 0; i < capacity(PrimitiveType::Triangle); ++i) {
        ft_accessor.index_access().scalar_attribute(i) = FT(i);
        f_flag_accessor.index_access().scalar_attribute(i) |= 0x1;
    }
}


void TetMesh::initialize(Eigen::Ref<const RowVectors4l> T)
{
    auto [TE, TF, TT, VT, ET, FT] = tetmesh_topology_initialization(T);
    initialize(T, TE, TF, TT, VT, ET, FT);
}

Tuple TetMesh::vertex_tuple_from_id(int64_t id) const
{
    int64_t t = m_vt_accessor->index_access().const_scalar_attribute(id);
    auto tv = m_tv_accessor->index_access().const_vector_attribute(t);
    int64_t lvid = -1;

    for (int64_t i = 0; i < 4; ++i) {
        if (tv(i) == id) {
            lvid = i;
            break;
        }
    }

    const auto [nlvid, leid, lfid] = autogen::tet_mesh::auto_3d_table_complete_vertex[lvid];
    assert(lvid == nlvid);

    if (lvid < 0 || leid < 0 || lfid < 0) throw std::runtime_error("vertex_tuple_from_id failed");

    ConstAccessor<int64_t> hash_accessor = get_const_cell_hash_accessor();

    Tuple v_tuple = Tuple(lvid, leid, lfid, t, get_cell_hash(t, hash_accessor));
    assert(is_ccw(v_tuple));
    assert(is_valid(v_tuple, hash_accessor));
    return v_tuple;
}

Tuple TetMesh::edge_tuple_from_id(int64_t id) const
{
    int64_t t = m_et_accessor->index_access().const_scalar_attribute(id);
    auto te = m_te_accessor->index_access().const_vector_attribute(t);

    int64_t leid = -1;

    for (int64_t i = 0; i < 6; ++i) {
        if (te(i) == id) {
            leid = i;
            break;
        }
    }
    const auto [lvid, nleid, lfid] = autogen::tet_mesh::auto_3d_table_complete_edge[leid];
    assert(leid == nleid);


    if (lvid < 0 || leid < 0 || lfid < 0) throw std::runtime_error("edge_tuple_from_id failed");

    ConstAccessor<int64_t> hash_accessor = get_const_cell_hash_accessor();

    Tuple e_tuple = Tuple(lvid, leid, lfid, t, get_cell_hash(t, hash_accessor));
    assert(is_ccw(e_tuple));
    assert(is_valid(e_tuple, hash_accessor));
    return e_tuple;
}

Tuple TetMesh::face_tuple_from_id(int64_t id) const
{
    int64_t t = m_ft_accessor->index_access().const_scalar_attribute(id);
    auto tf = m_tf_accessor->index_access().const_vector_attribute(t);

    int64_t lfid = -1;

    for (int64_t i = 0; i < 4; ++i) {
        if (tf(i) == id) {
            lfid = i;
            break;
        }
    }

    const auto [lvid, leid, nlfid] = autogen::tet_mesh::auto_3d_table_complete_face[lfid];
    assert(lfid == nlfid);

    if (lvid < 0 || leid < 0 || lfid < 0) throw std::runtime_error("face_tuple_from_id failed");

    ConstAccessor<int64_t> hash_accessor = get_const_cell_hash_accessor();

    Tuple f_tuple = Tuple(lvid, leid, lfid, t, get_cell_hash(t, hash_accessor));
    assert(is_ccw(f_tuple));
    assert(is_valid(f_tuple, hash_accessor));
    return f_tuple;
}

Tuple TetMesh::tet_tuple_from_id(int64_t id) const
{
    const int64_t lvid = 0;
    const auto [nlvid, leid, lfid] = autogen::tet_mesh::auto_3d_table_complete_vertex[lvid];
    assert(lvid == nlvid);

    ConstAccessor<int64_t> hash_accessor = get_const_cell_hash_accessor();

    Tuple t_tuple = Tuple(lvid, leid, lfid, id, get_cell_hash(id, hash_accessor));
    assert(is_ccw(t_tuple));
    assert(is_valid(t_tuple, hash_accessor));
    return t_tuple;
}

Tuple TetMesh::tuple_from_id(const PrimitiveType type, const int64_t gid) const
{
    switch (type) {
    case PrimitiveType::Vertex: {
        return vertex_tuple_from_id(gid);
        break;
    }
    case PrimitiveType::Edge: {
        return edge_tuple_from_id(gid);
        break;
    }
    case PrimitiveType::Triangle: {
        return face_tuple_from_id(gid);
        break;
    }
    case PrimitiveType::Tetrahedron: {
        return tet_tuple_from_id(gid);
        break;
    }
    default: assert(false); // "Invalid primitive type"
    }

    return Tuple();
}

int64_t TetMesh::id(const Tuple& tuple, PrimitiveType type) const
{
    switch (type) {
    case PrimitiveType::Vertex: {
        auto tv = m_tv_accessor->const_vector_attribute(tuple);
        return tv(tuple.m_local_vid);
        break;
    }
    case PrimitiveType::Edge: {
        auto te = m_te_accessor->const_vector_attribute(tuple);
        return te(tuple.m_local_eid);
        break;
    }
    case PrimitiveType::Triangle: {
        auto tf = m_tf_accessor->const_vector_attribute(tuple);
        return tf(tuple.m_local_fid);
        break;
    }
    case PrimitiveType::Tetrahedron: {
        return tuple.m_global_cid;
        break;
    }
    default: assert(false); // "Tuple id: Invalid primitive type"
    }

    return -1;
}

Tuple TetMesh::switch_tuple(const Tuple& tuple, PrimitiveType type) const
{
    assert(is_valid_slow(tuple));
    switch (type) {
    // bool ccw = is_ccw(tuple);
    case PrimitiveType::Tetrahedron: {
        assert(!is_boundary_face(tuple));
        // need test
        const int64_t gvid = id(tuple, PrimitiveType::Vertex);
        const int64_t geid = id(tuple, PrimitiveType::Edge);
        const int64_t gfid = id(tuple, PrimitiveType::Triangle);

        auto tt = m_tt_accessor->const_vector_attribute(tuple);

        int64_t gcid_new = tt(tuple.m_local_fid);

        /*handle exception here*/
        assert(gcid_new != -1);
        // check if is_boundary allows removing this exception in 3d cases
        // if (gcid_new == -1) {
        //     return Tuple(-1, -1, -1, -1, -1);
        // }
        /*handle exception end*/

        int64_t lvid_new = -1, leid_new = -1, lfid_new = -1;

        auto tv = m_tv_accessor->index_access().const_vector_attribute(gcid_new);

        auto te = m_te_accessor->index_access().const_vector_attribute(gcid_new);

        auto tf = m_tf_accessor->index_access().const_vector_attribute(gcid_new);

        for (int64_t i = 0; i < 4; ++i) {
            if (tv(i) == gvid) {
                lvid_new = i;
            }
            if (tf(i) == gfid) {
                lfid_new = i;
            }
        }

        for (int64_t i = 0; i < 6; ++i) {
            if (te(i) == geid) {
                leid_new = i;
                break; // check if the break is correct
            }
        }


        assert(lvid_new != -1);
        assert(leid_new != -1);
        assert(lfid_new != -1);

        const Tuple res(lvid_new, leid_new, lfid_new, gcid_new, get_cell_hash_slow(gcid_new));
        assert(is_valid_slow(res));
        return res;
    }
    case PrimitiveType::Vertex:
    case PrimitiveType::Edge:
    case PrimitiveType::Triangle:
    default: return autogen::tet_mesh::local_switch_tuple(tuple, type);
    }
}

bool TetMesh::is_ccw(const Tuple& tuple) const
{
    assert(is_valid_slow(tuple));
    return autogen::tet_mesh::is_ccw(tuple);
}

bool TetMesh::is_valid(const Tuple& tuple, ConstAccessor<int64_t>& hash_accessor) const
{
    if (tuple.is_null()) return false;
    const bool is_connectivity_valid = tuple.m_local_vid >= 0 && tuple.m_local_eid >= 0 &&
                                       tuple.m_local_fid >= 0 && tuple.m_global_cid >= 0 &&
                                       autogen::tet_mesh::tuple_is_valid_for_ccw(tuple);

    if (!is_connectivity_valid) {
        return false;
    }

    return Mesh::is_hash_valid(tuple, hash_accessor);
}

bool TetMesh::is_boundary(PrimitiveType pt, const Tuple& tuple) const
{
    switch (pt) {
    case PrimitiveType::Vertex: return is_boundary_vertex(tuple);
    case PrimitiveType::Edge: return is_boundary_edge(tuple);
    case PrimitiveType::Triangle: return is_boundary_face(tuple);
    case PrimitiveType::Tetrahedron:
    default: break;
    }
    assert(
        false); // "tried to compute the boundary of an tet mesh for an invalid simplex dimension"
    return false;
}


bool TetMesh::is_boundary_face(const Tuple& tuple) const
{
    ConstAccessor<int64_t> tt_accessor = create_accessor<int64_t>(m_tt_handle);
    return tt_accessor.const_vector_attribute(tuple)(tuple.m_local_fid) < 0;
}

bool TetMesh::is_boundary_edge(const Tuple& edge) const
{
    for (const Tuple& f : simplex::cofaces_single_dimension_tuples(
             *this,
             simplex::Simplex::edge(edge),
             PrimitiveType::Triangle)) {
        if (is_boundary_face(f)) {
            return true;
        }
    }
    return false;
}
bool TetMesh::is_boundary_vertex(const Tuple& vertex) const
{
    // go through all faces and check if they are boundary
    const simplex::SimplexCollection neigh =
        wmtk::simplex::open_star(*this, simplex::Simplex::vertex(vertex));
    for (const simplex::Simplex& s : neigh.simplex_vector(PrimitiveType::Triangle)) {
        if (is_boundary(s)) {
            return true;
        }
    }

    return false;
}

bool TetMesh::is_connectivity_valid() const
{
    // get Accessors for topology
    ConstAccessor<int64_t> tv_accessor = create_const_accessor<int64_t>(m_tv_handle);
    ConstAccessor<int64_t> te_accessor = create_const_accessor<int64_t>(m_te_handle);
    ConstAccessor<int64_t> tf_accessor = create_const_accessor<int64_t>(m_tf_handle);
    ConstAccessor<int64_t> tt_accessor = create_const_accessor<int64_t>(m_tt_handle);
    ConstAccessor<int64_t> vt_accessor = create_const_accessor<int64_t>(m_vt_handle);
    ConstAccessor<int64_t> et_accessor = create_const_accessor<int64_t>(m_et_handle);
    ConstAccessor<int64_t> ft_accessor = create_const_accessor<int64_t>(m_ft_handle);
    ConstAccessor<char> v_flag_accessor = get_flag_accessor(PrimitiveType::Vertex);
    ConstAccessor<char> e_flag_accessor = get_flag_accessor(PrimitiveType::Edge);
    ConstAccessor<char> f_flag_accessor = get_flag_accessor(PrimitiveType::Triangle);
    ConstAccessor<char> t_flag_accessor = get_flag_accessor(PrimitiveType::Tetrahedron);

    // VT and TV
    for (int64_t i = 0; i < capacity(PrimitiveType::Vertex); ++i) {
        if (v_flag_accessor.index_access().const_scalar_attribute(i) == 0) {
            wmtk::logger().debug("Vertex {} is deleted", i);
            continue;
        }
        int cnt = 0;
        for (int j = 0; j < 4; ++j) {
            if (tv_accessor.index_access().const_vector_attribute(
                    vt_accessor.index_access().const_scalar_attribute(i))[j] == i) {
                cnt++;
            }
        }
        if (cnt != 1) {
            wmtk::logger().info("fail VT and TV");
            return false;
        }
    }

    // ET and TE
    for (int64_t i = 0; i < capacity(PrimitiveType::Edge); ++i) {
        if (e_flag_accessor.index_access().const_scalar_attribute(i) == 0) {
            wmtk::logger().debug("Edge {} is deleted", i);
            continue;
        }
        int cnt = 0;
        for (int j = 0; j < 6; ++j) {
            if (te_accessor.index_access().const_vector_attribute(
                    et_accessor.index_access().const_scalar_attribute(i))[j] == i) {
                cnt++;
            }
        }
        if (cnt != 1) {
            wmtk::logger().info("fail ET and TE");
            return false;
        }
    }

    // FT and TF
    for (int64_t i = 0; i < capacity(PrimitiveType::Triangle); ++i) {
        if (f_flag_accessor.index_access().const_scalar_attribute(i) == 0) {
            wmtk::logger().debug("Face {} is deleted", i);
            continue;
        }
        int cnt = 0;
        for (int j = 0; j < 4; ++j) {
            if (tf_accessor.index_access().const_vector_attribute(
                    ft_accessor.index_access().const_scalar_attribute(i))[j] == i) {
                cnt++;
            }
        }
        if (cnt != 1) {
            wmtk::logger().info("fail FT and TF");
            return false;
        }
    }

    // TF and TT
    for (int64_t i = 0; i < capacity(PrimitiveType::Tetrahedron); ++i) {
        if (t_flag_accessor.index_access().const_scalar_attribute(i) == 0) {
            wmtk::logger().debug("Tet {} is deleted", i);
            continue;
        }

        for (int j = 0; j < 4; ++j) {
            int64_t nb = tt_accessor.index_access().const_vector_attribute(i)(j);
            if (nb == -1) {
                if (ft_accessor.index_access().const_scalar_attribute(
                        tf_accessor.index_access().const_vector_attribute(i)(j)) != i) {
                    wmtk::logger().info("fail TF and TT 1");
                    return false;
                }
                continue;
            }

            int cnt = 0;
            int id_in_nb;
            for (int k = 0; k < 4; ++k) {
                if (tt_accessor.index_access().const_vector_attribute(nb)(k) == i) {
                    cnt++;
                    id_in_nb = k;
                }
            }
            if (cnt != 1) {
                wmtk::logger().info("fail TF and TT 2");
                return false;
            }

            if (tf_accessor.index_access().const_vector_attribute(i)(j) !=
                tf_accessor.index_access().const_vector_attribute(nb)(id_in_nb)) {
                wmtk::logger().info("fail TF and TT 3");
                return false;
            }
        }
    }

    return true;
}

std::vector<std::vector<TypedAttributeHandle<int64_t>>> TetMesh::connectivity_attributes() const
{
    std::vector<std::vector<TypedAttributeHandle<int64_t>>> handles(4);

    handles[0].push_back(m_tv_handle);
    handles[1].push_back(m_te_handle);
    handles[2].push_back(m_tf_handle);

    handles[3].push_back(m_tt_handle);
    handles[3].push_back(m_vt_handle);
    handles[3].push_back(m_et_handle);
    handles[3].push_back(m_ft_handle);

    return handles;
}

Tuple TetMesh::tuple_from_global_ids(int64_t tid, int64_t fid, int64_t eid, int64_t vid) const
{
    auto tv = m_tv_accessor->index_access().const_vector_attribute(tid);
    auto te = m_te_accessor->index_access().const_vector_attribute(tid);
    auto tf = m_tf_accessor->index_access().const_vector_attribute(tid);

    int64_t lvid = -1, leid = -1, lfid = -1;

    for (int j = 0; j < 4; ++j) {
        if (tv(j) == vid) {
            lvid = j;
        }
        if (tf(j) == fid) {
            lfid = j;
        }
    }

    for (int j = 0; j < 6; ++j) {
        if (te(j) == eid) {
            leid = j;
            break;
        }
    }

    assert(lvid != -1);
    assert(leid != -1);
    assert(lfid != -1);

    return Tuple(lvid, leid, lfid, tid, get_cell_hash_slow(tid));
}


} // namespace wmtk
