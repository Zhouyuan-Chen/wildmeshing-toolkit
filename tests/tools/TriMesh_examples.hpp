#pragma once

#include <wmtk/TriMesh.hpp>

namespace wmtk::tests {


//         0
//        / \   .
//       2   1  \ .
//      /  0  \  \|
//     /       \ .
//  1  ----0---- 2
//
TriMesh single_triangle();

// a single triangle with position
// (0,0,0), (1,0,0), (0.5, sqrt(3)/2, 0)ß
TriMesh single_equilateral_triangle();

//  3--1--- 0
//   |     / \ .
//   2 f1 /2   1
//   |  0/ f0  \ .
//   |  /       \ 
//  1  ----0---- 2
//
TriMesh one_ear(); // an alias for quad
TriMesh quad();

//  3--1--- 0 --1- 4
//   |     / \     |
//   2 f1 /2 1\ f2 |
//   |  0/ f0  \1  0
//   |  /       \  |
//   1  ----0----  2
//
TriMesh two_neighbors();

TriMesh two_neighbors_cut_on_edge01();

//
//  4------ 0 ---- 3
//   |     / \     |
//   | f2 /   \ f0 |
//   |   / f1  \   |
//   |  /       \  |
//   2  ---------  1
//      \       /  .
//       \ f3  /   .
//        \   /    .
//         \ /     .
//          5
TriMesh three_neighbors();

//  3------ 0 ---- 3
//   |     / \     |
//   | f2 /   \ f0 |
//   |   / f1  \   |
//   |  /       \  |
//   2  ---------  1
//      \       /  .
//       \ f3  /   .
//        \   /    .
//         \ /     .
//          3
TriMesh tetrahedron();

TriMesh tetrahedron_with_position();

//  3--1--- 0
//   |     / \ .
//   2 f1 /2   1
//   |  0/ f0  \ .
//   |  /       \ .
//  1  ----0---- 2
//     \        /
//      \  f2  /
//       \    /
//        \  /
//         4
TriMesh interior_edge();


//    0---1---2
//   / \ / \ / \ .
//  3---4---5---6
//   \ / \ /  .
//    7---8
TriMesh hex_plus_two();

TriMesh hex_plus_two_with_position();

//    0---1---2
//   / \ / \ / \ .
//  3---4---5---6
//   \ / \ / \ /
//    7---8---9
TriMesh edge_region();

//  0---2  3---4
//  | /    | \ |
//  1      5---6
TriMesh three_triangles_with_two_components();


// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  0⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠞⠁⠀⡇⠉⠓⠦⢤⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠞⠁⠀⠀⠀⠸⡄⠀⠀⠀⠀⠉⠓⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⢀⡤⠋⠀⠀⠀⠀⠀⠀⠀⢧⠀⠀⠀⠀⠀⠀⠀⠈⠓⠦⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⣠⠴⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⠦⢄⡀⠀⠀⠀⠀⠀
// ⠀⠀⣀⠔⠋⠁⠀⠀⠀⠀⠀ ⠀⠀⠀⠀⠀⠀⠀⢧⠀⠀⠀⠀ ⠀⠀⠀⠀⠀⠀⠀⠀⠈⠓⠦⣄⠀⠀
// ⢀⡞⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠓
//  1⠹⡌⠉⠉⠉⠓⠒⠒⠒⠒⠒⠒⠦⠤⠤⠤⠤2⣤⠤⠤⠤⠤⠤⠤⠤⠤⠤⠤⠤⠖⠒⠒3⠀
// ⠀⡇⠀⢧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠋⡏⡁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸
// ⠀⡇⠀⠀⠱⡄⠀⠀⠀⠀ ⠀⠀⠀⠀⠀⠀⢀⡞⣱⠇⣶⣷⡤⡀⠀⠀⠀⠀ ⠀⠀⠀⠀⠀⠀⠀⡜⠁⢸
// ⢠⠇⠀⠀⠀⠘⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⡴⣋⣴⣟⣽⠥⠹⣯⣧⡆⠀⠀⠀⠀⠀⠀⠀⠀⢀⡖⠁⠀⢸
// ⢸⠀⠀⠀⠀⠀⠈⢦⠀⠀⠀⠀⠀⠀⣠⡟⣩⡟⢿⢸⡇⢀⣔⣯⣿⡆⠀⠀⠀⠀⠀⠀⠀⡴⠃⠀⠀⠀⢸
// ⠀⡇⠀⠀⠀⠀⠀⠀⠱⡄⠀⠀⣠⢿⣷⣿⣃⣀⠈⠙⠛⡿⠛⠋⢠⡿⡇⠀⠀⠀⠀⡴⠋⠀⠀⠀⠀⠀⡇
// ⠀⡇⠀⠀ ⠀⠀⠀⠀⠈⢇⡞⠁⠟⠓⠒⠛⠘⠙⠑⠂⠓⣒⣶⣿⡟⣆⣀⠀⢀⡏⠀⠀⠀⠀⠀⠀⣸⠀
// ⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀4⢯⠉⠉⠑⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠤⠤⠤⠤5⠀⠀⠀⠀⠀⠀⠀⡏⠀
// ⠀⡇⠀⠀⠀⠀⠀⠀⣠⠎⠀⠀⢧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠖⠁⠑⢆⠀⠀⠀⠀ ⢸
// ⠀⡇⠀⠀⠀⢀⠔⠃⠀⠀⠀⠀⠘⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡤⠋⠀⠀⠀⠀⠀⠳⡄⠀⠀⢸
// ⠀⡇⢀⠖⠋⠀⠀⠀⠀⠀⠀⠀⠀⠘⡄⠀⠀⠀⠀⠀⠀⠀⠀⣠⠔⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠙⡆⠀⡼
// ⠀6⣁⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠸⡄⠀⠀⠀⣀⣠⠔⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣆⡇
// ⠀⠀⠀⠀⠀⠀⠀⠉⠉⠉⠉⠉⠙⠿⠋⠛7⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠢⠤⠼⢦⡇⠀8⠀
TriMesh nine_triangles_with_a_hole();

TriMesh edge_region_with_position();

//      1---2
//     / \ / \ .
//    3---4---5
//   / \ / \ / \ .
//  6---7---8---9
//   \ / \ / \ /
//   10--11--12
//     \ / \ /
//     13---14
TriMesh embedded_diamond();

//
//  4------ 0 ---- 3
//   |     / \     |
//   | f1 /   \ f0 |
//   |   /     \   |
//   |  /       \  |
//   2  ---------  1
//      \       /  .
//       \ f2  /   .
//        \   /    .
//         \ /     .
//          5
TriMesh three_individuals();

} // namespace wmtk::tests
