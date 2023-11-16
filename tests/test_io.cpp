#include <wmtk/Mesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/io/Cache.hpp>
#include <wmtk/io/HDF5Writer.hpp>
#include <wmtk/io/MeshReader.hpp>
#include <wmtk/io/ParaviewWriter.hpp>
#include <wmtk/utils/Rational.hpp>
#include <wmtk/utils/mesh_utils.hpp>

#include <wmtk/operations/OperationFactory.hpp>
#include <wmtk/operations/tri_mesh/EdgeSplit.hpp>

#include "tools/DEBUG_TriMesh.hpp"
#include "tools/TriMesh_examples.hpp"

#include <catch2/catch_test_macros.hpp>


using namespace wmtk;
using namespace wmtk::tests;

namespace fs = std::filesystem;


constexpr PrimitiveType PV = PrimitiveType::Vertex;
constexpr PrimitiveType PE = PrimitiveType::Edge;

TEST_CASE("hdf5_2d", "[io]")
{
    RowVectors3l tris;
    tris.resize(1, 3);
    tris.row(0) = Eigen::Matrix<long, 3, 1>{0, 1, 2};

    TriMesh mesh;
    mesh.initialize(tris);

    HDF5Writer writer("hdf5_2d.hdf5");
    mesh.serialize(writer);
}

TEST_CASE("hdf5_2d_read", "[io]")
{
    RowVectors3l tris;
    tris.resize(1, 3);
    tris.row(0) = Eigen::Matrix<long, 3, 1>{0, 1, 2};

    TriMesh mesh;
    mesh.initialize(tris);

    HDF5Writer writer("hdf5_2d_read.hdf5");
    mesh.serialize(writer);

    auto mesh1 = read_mesh("hdf5_2d_read.hdf5");

    CHECK(*mesh1 == mesh);
}

TEST_CASE("hdf5_rational", "[io]")
{
    Eigen::Matrix<long, 2, 4> T;
    T << 0, 1, 2, 3, 4, 5, 6, 7;
    TetMesh mesh;
    mesh.initialize(T);
    Eigen::Matrix<Rational, 8, 3> V;
    for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            V(i, j) = Rational(std::rand()) / Rational(std::rand());
        }
    }

    mesh_utils::set_matrix_attribute(V, "vertices", PrimitiveType::Vertex, mesh);

    HDF5Writer writer("hdf5_rational.hdf5");
    mesh.serialize(writer);

    auto mesh1 = read_mesh("hdf5_rational.hdf5");

    CHECK(*mesh1 == mesh);
}

TEST_CASE("paraview_2d", "[io]")
{
    auto mesh = read_mesh(WMTK_DATA_DIR "/fan.msh");

    ParaviewWriter writer("paraview_2d", "vertices", *mesh, true, true, true, false);
    mesh->serialize(writer);
}

TEST_CASE("hdf5_3d", "[io]")
{
    Eigen::Matrix<long, 2, 4> T;
    T << 0, 1, 2, 3, 4, 5, 6, 7;
    TetMesh mesh;
    mesh.initialize(T);

    HDF5Writer writer("hdf5_3d.hdf5");
    mesh.serialize(writer);
}

TEST_CASE("paraview_3d", "[io]")
{
    Eigen::Matrix<long, 2, 4> T;
    T << 0, 1, 2, 3, 4, 5, 6, 7;
    TetMesh mesh;
    mesh.initialize(T);
    Eigen::MatrixXd V(8, 3);
    V.setRandom();
    mesh_utils::set_matrix_attribute(V, "vertices", PrimitiveType::Vertex, mesh);

    ParaviewWriter writer("paraview_3d", "vertices", mesh, true, true, true, true);
    mesh.serialize(writer);
}

TEST_CASE("msh_3d", "[io]")
{
    auto mesh = read_mesh(WMTK_DATA_DIR "/sphere_delaunay.msh");
}

TEST_CASE("attribute_after_split", "[io]")
{
    DEBUG_TriMesh m = single_equilateral_triangle();
    wmtk::MeshAttributeHandle<long> attribute_handle =
        m.register_attribute<long>(std::string("test_attribute"), PE, 1);
    wmtk::MeshAttributeHandle<double> pos_handle =
        m.get_attribute_handle<double>(std::string("position"), PV);

    {
        Accessor<long> acc_attribute = m.create_accessor<long>(attribute_handle);
        Accessor<double> acc_pos = m.create_accessor<double>(pos_handle);

        const Tuple edge = m.edge_tuple_between_v1_v2(0, 1, 0);

        Eigen::Vector3d p_mid;
        {
            const Eigen::Vector3d p0 = acc_pos.vector_attribute(edge);
            const Eigen::Vector3d p1 = acc_pos.vector_attribute(m.switch_vertex(edge));
            p_mid = 0.5 * (p0 + p1);
        }

        {
            // set edge(0,1)'s tag as 1
            acc_attribute.scalar_attribute(edge) = 1;

            // all edges hold 0 besides "edge"
            for (const Tuple& t : m.get_all(PE)) {
                if (m.simplices_are_equal(Simplex::edge(edge), Simplex::edge(t))) {
                    CHECK(acc_attribute.scalar_attribute(t) == 1);
                } else {
                    CHECK(acc_attribute.scalar_attribute(t) == 0);
                }
            }

            wmtk::operations::OperationSettings<operations::tri_mesh::EdgeSplit> op_settings;
            op_settings.split_boundary_edges = true;
            op_settings.initialize_invariants(m);

            operations::tri_mesh::EdgeSplit op(m, edge, op_settings);
            REQUIRE(op());

            // set new vertex position
            acc_pos.vector_attribute(op.return_tuple()) = p_mid;
        }

        // since the default value is 0, there should be no other value in this triangle
        for (const Tuple& t : m.get_all(PE)) {
            CHECK(acc_attribute.scalar_attribute(t) == 0);
        }
    } // end of scope for the accessors

    {
        Accessor<long> acc_attribute = m.create_accessor<long>(attribute_handle);
        for (const Tuple& t : m.get_all(PE)) {
            CHECK(acc_attribute.scalar_attribute(t) == 0);
        }
    }

    // attribute_after_split_edges.hdf contains a 1 in the "test_attribute"
    ParaviewWriter writer("attribute_after_split", "position", m, true, true, true, false);
    m.serialize(writer);
}

TEST_CASE("cache_init", "[cache][io]")
{
    const fs::path dir = std::filesystem::current_path();
    const std::string prefix = "wmtk_cache";

    fs::path cache_dir;
    {
        io::Cache cache(prefix, dir);
        cache_dir = cache.get_cache_path();

        CHECK(fs::exists(cache_dir));

        CHECK(dir == cache_dir.parent_path());
        CHECK(cache_dir.stem().string().rfind(prefix, 0) == 0); // cache dir starts with prefix
    }
    CHECK_FALSE(fs::exists(cache_dir));
}

TEST_CASE("cache_files", "[cache][io]")
{
    fs::path filepath;
    std::string file_name = "my_new_file";
    {
        io::Cache cache("wmtk_cache", fs::current_path());

        filepath = cache.create_unique_file(file_name, ".txt");

        CHECK(fs::exists(filepath));
        CHECK(filepath.stem().string().rfind(file_name, 0) == 0);
        CHECK(filepath.extension().string() == ".txt");

        const fs::path filepath_from_cache = cache.get_file_path(file_name);

        CHECK(filepath_from_cache == filepath);
    }
    CHECK_FALSE(fs::exists(filepath));
}

TEST_CASE("cache_read_write_mesh", "[cache][io]")
{
    io::Cache cache("wmtk_cache", fs::current_path());
    TriMesh mesh = tests::single_triangle();

    const std::string name = "cached_mesh";
    cache.write_mesh(mesh, name);

    auto mesh_from_cache = cache.read_mesh(name);

    CHECK(*mesh_from_cache == mesh);
    CHECK_THROWS(cache.read_mesh("some_file_that_does_not_exist"));
}

TEST_CASE("cache_export_import", "[cache][io]")
{
    const fs::path export_location =
        io::Cache::create_unique_directory("wmtk_cache_export", fs::current_path());

    const std::vector<std::string> file_names = {"a", "b", "c"};

    // create cache
    fs::path first_cache_path;
    {
        io::Cache cache("wmtk_cache", fs::current_path());
        // generate some files
        for (const std::string& name : file_names) {
            const fs::path p = cache.create_unique_file(name, ".txt");
            CHECK(fs::exists(p));
            CHECK(p.stem().string().rfind(name, 0) == 0);
            CHECK(p.extension().string() == ".txt");
        }

        first_cache_path = cache.get_cache_path();

        // delete dummy directory
        fs::remove_all(export_location);
        REQUIRE_FALSE(fs::exists(export_location));
        // export cache to dummy directory
        REQUIRE(cache.export_cache(export_location));
    }
    CHECK_FALSE(fs::exists(first_cache_path));

    // create new cache
    {
        io::Cache cache("wmtk_cache", fs::current_path());
        // import the previously exported
        CHECK(cache.import_cache(export_location));

        // check if files are there
        for (const std::string& name : file_names) {
            const fs::path p = cache.get_file_path(name);
            CHECK(fs::exists(p));
            CHECK(p.stem().string().rfind(name, 0) == 0);
            CHECK(p.extension().string() == ".txt");
        }
    }

    // try to import even though the cache contains a file
    {
        io::Cache cache("wmtk_cache", fs::current_path());
        cache.create_unique_file("some_file", "");
        // import should not work if the cache already contains files
        CHECK_FALSE(cache.import_cache(export_location));
    }

    // clean up export
    fs::remove_all(export_location);
}