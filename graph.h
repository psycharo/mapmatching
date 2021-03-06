#ifndef GRAPH_H
#define GRAPH_H


#include <vector>
#include <string>

#include "geometry.h"
#include "util.h"
#include "spatial.h"


using namespace mmatch;


namespace mmatch {


//! identifying shared point, does not belong to any edge
static const int32_t EID_COMMON = -1;

/*!
 * \brief The geom_id struct. used as a unique id for all the geometrical nodes
 * (including edge geometry) in the graph
 */
struct geom_id
{
    int32_t eid;
    int32_t gid;

    geom_id(int32_t _eid, int32_t _gid):
        eid(_eid),
        gid(_gid)
    {}

    inline bool operator<(const geom_id &other) const
    {
        return (eid == other.eid) ? gid < other.gid : eid < other.eid;
    }

    inline bool operator==(const geom_id &other) const
    {
        return (eid == other.eid) && (gid == other.gid);
    }

    inline bool operator!=(const geom_id &other) const
    {
        return !(*this == other);
    }

    inline bool is_internal() const
    {
        return eid != EID_COMMON;
    }

};



inline std::ostream &operator<<(std::ostream &os, const geom_id &id)
{
    if (id.is_internal())
        os << "edge(" << id.eid << ", " << id.gid << ")";
    else
        os << "node(" << id.gid << ")";
    return os;
}



/**
 * @brief The Edge class
 */
class Edge
{
public:

    static const size_t MAX_NAME_LENGTH = 64;

    int id;
    int from;
    int to;
    int cost;
    double length;

    // TODO: char name[256]?
    char name[MAX_NAME_LENGTH];
    char type[MAX_NAME_LENGTH];

    std::vector<UTMNode> geometry;

    //! fill geometry data from the given string
    void fillGeometry(const std::string &s);

    //! fill geometry, metric data
    void fillUTMGeometry(const std::string &s);

    //! generate the correct id for the part of the edge
    inline geom_id geometry_id(int32_t gid) const
    {
        if (gid == 0)
            return geom_id(EID_COMMON, from);
        else if (gid == geometry.size()-1)
            return geom_id(EID_COMMON, to);
        return geom_id(id, gid);
    };


    // bounding box???
};



/**
 * @brief The RoadGraph class incapsulates the road structure
 */
class RoadGraph
{
private:
    std::vector<UTMNode> m_nodes;
    std::vector<std::vector<const Edge*> > m_edges;
    std::vector<const Edge*> m_edgeIndex;

public:
    RoadGraph() {}

    virtual ~RoadGraph();

    // TODO: binary save/load!

    //! load all the data from files
    void load(const char *fileNodes, const char *fileEdges, const char *fileGeometry);

    //! load all the data from files, metric coords
    void loadUTM(const char *fileNodes, const char *fileEdges, const char *fileGeometry);

    //! load everything from provided data
    void fromData(const std::vector<UTMNode> &, const std::vector<Edge> &);

    //! load all from binary file
    void loadBinary(const char *outputFile);

    //! save everything as binary file
    void saveBinary(const char *dst);


    // data accessors
    inline const std::vector<UTMNode> &nodes() const { return m_nodes; }
    inline const std::vector<std::vector<const Edge*> > &edges() const { return m_edges; }
    inline const std::vector<const Edge*> &index() const { return m_edgeIndex; }

    //! get GEOMETRICAL node by GEOMETRICAL id
    inline const UTMNode &coord(geom_id id) const { return (id.is_internal()) ? m_edgeIndex[id.eid]->geometry[id.gid] : m_nodes[id.gid]; }

    //! get all all outgoing GEOMETRICAL IDS for a given (eid,gid)
    std::vector<geom_id> adjacent(geom_id id) const;

    //! get all outgoing edges for a given node id
    inline const std::vector<const Edge*> &outgoing(int nodeId) const { return m_edges[nodeId]; }
    //! get an edge by id
    inline const Edge *edge(int id) const { return m_edgeIndex[id]; }
};



} // namespace mmatch


// hash function for geom_id
namespace std
{
template <>
class hash<geom_id>
{
public :
    size_t operator()(const geom_id &x) const
    {
        return size_t((int64_t(x.eid) << 32) & x.gid);
    }
};
}


#endif // GRAPH_H
