#pragma once

#include "Primitive.hpp"


namespace wmtk {

class Mesh;
class PointMesh;
class EdgeMesh;
class TriMesh;
class TetMesh;
class MultiMeshManager;

class Tuple
{
private:
    // if Tuple is in 2d mesh m_global_cid is the global triangle id, and local_fid is -1
    // if Tuple is in 3d mesh m_global_cid is the global tetrahedron id
    long m_local_vid = -1;
    long m_local_eid = -1;
    long m_local_fid = -1;
    long m_global_cid = -1;
    long m_hash = -1;

public:
    friend class Mesh;
    friend class PointMesh;
    friend class EdgeMesh;
    friend class TriMesh;
    friend class TetMesh;
    friend class MultiMeshManager;
    // friend long Mesh::id(const Tuple& tuple, const PrimitiveType& type) const;
    // friend Mesh::is_ccw(const Tuple& tuple) const;
    // friend Mesh::switch_tuple(const Tuple& tuple, const PrimitiveType& type) const;

    Tuple(long local_vid, long local_eid, long local_fid, long global_cid, long hash);

    //         v2
    //       /    \.
    //  e1  /      \  e0
    //     v0 - - - v1
    //         e2

    Tuple();
    Tuple(const Tuple& other);
    Tuple(Tuple&& other);
    Tuple& operator=(const Tuple& other);
    Tuple& operator=(Tuple&& other);

    bool operator==(const Tuple& t) const;
    bool operator!=(const Tuple& t) const;
    // equality comparison but skips the hash
    bool same_ids(const Tuple& t) const;

    bool is_null_tuple() const {return m_local_vid == -1 && m_local_eid == -1 && m_local_fid == -1 && m_global_cid == -1 && m_hash == -1;};
};
} // namespace wmtk
