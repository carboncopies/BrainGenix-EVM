//===========================================================//
// This file is part of the BrainGenix-EVM Validation System //
//===========================================================//

/*
    Description: This file provides a Connectome definition.
    Additional Notes: None
    Date Created: 2024-04-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
//#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")

namespace BG {

struct Edge {
    EdgeType type_;
    float weight_;
};

/**
 * This version is very simple, as it merges all edges in one direction between a pair of
 * vertices into a single edge with combined weight.
 * 
 * TODO: *** More sophisticated versions of this will be needed to analyze and express
 * functional differences when the underlying neuronal circuits are more complex (e.g.
 * have multiple types of synapses, have multiple connections between a pair.)
 * 
 * TODO: *** For a high resolution analysis it can be necessary to treat every branch
 * point as a vertex instead of merely treating somas as vertices. In that case, branch
 * points can receive their own 'vertex type'.
 */
struct Vertex {
    VertexType type_;
    // < target vertex ID, edge data >
    std::map<unsigned int, std::unique_ptr<Edge>>

    Vertex(VertexType _type): type_(_type) {}
};

struct Connectome {
    std::vector<std::unique_ptr<Vertex>> Vertices;
};

} // BG
