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

enum EdgeType {
    UnknownConnection = 0,
    ExcitatoryConnection = 1,
    InhibitoryConnection = 2,
    NUMEdgeType
};

struct Edge {
    EdgeType type_ = UnknownConnection;
    float weight_ = 0.0;

    Edge(EdgeType _type, float _weight): type_(_type), weight_(_weight) {}
};

enum VertexType: int {
    UnknownVertex = 0,
    PrincipalNeuron = 1,
    Interneuron = 2,
    NUMVertexType
};

/**
 * This version is very simple, as it merges all edges in one direction between a pair of
 * vertices into a single edge with combined weight.
 * 
 * This creates a sparse representation of the Connectome. To simplify finding both
 * incoming and outgoing connections of a vertex, it maintains two edge lists
 * for each vertex.
 * Editing the connectome has to edit both lists.
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
    VertexType type_ = UnknownVertex;
    // < target vertex ID, edge data >
    std::map<unsigned int, std::unique_ptr<Edge>> OutEdges;
    std::map<unsigned int, std::unique_ptr<Edge>> InEdges;

    Vertex(VertexType _type): type_(_type) {}
};

struct Connectome {
    std::vector<std::unique_ptr<Vertex>> Vertices;
};

} // BG
