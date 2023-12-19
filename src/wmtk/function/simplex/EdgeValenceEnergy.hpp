#pragma once
#include <wmtk/function/PerSimplexFunction.hpp>

namespace wmtk::function {

class EdgeValenceEnergy : public PerSimplexFunction
{
public:
    EdgeValenceEnergy(
        const Mesh& mesh,
        const attribute::MeshAttributeHandle<double>& variable_attribute_handle);
    double get_value(const Simplex& simplex) const override;
    using PerSimplexFunction::get_value;

protected:
    const TriMesh& tri_mesh() const;
};
} // namespace wmtk::function
