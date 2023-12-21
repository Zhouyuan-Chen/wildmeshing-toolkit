#pragma once

#include "MeshOperation.hpp"

namespace wmtk::operations::tri_mesh {

class EdgeSplit : public MeshOperation
{
public:
    EdgeSplit(Mesh& m);


protected:
    PrimitiveType primitive_type() const override { return PrimitiveType::Edge; }

    std::vector<Simplex> execute(EdgeMesh& mesh, const Simplex& simplex) override;
    std::vector<Simplex> unmodified_primitives(const EdgeMesh& mesh, const Simplex& simplex)
        const override;

    std::vector<Simplex> execute(TriMesh& mesh, const Simplex& simplex) override;
    std::vector<Simplex> unmodified_primitives(const TriMesh& mesh, const Simplex& simplex)
        const override;

    std::vector<Simplex> execute(TetMesh& mesh, const Simplex& simplex) override;
    std::vector<Simplex> unmodified_primitives(const TetMesh& mesh, const Simplex& simplex)
        const override;
    std::vector<Simplex> execute(const Simplex& simplex) override;
    std::vector<Simplex> unmodified_primitives(const Simplex& simplex) const override;
};

} // namespace wmtk::operations::tri_mesh
