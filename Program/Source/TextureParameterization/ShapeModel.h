#include "MeshObject.h"


class ShapeModel {
public:    
    ~ShapeModel() {
        m_vertices.clear();
        m_faces.clear();
    };

    bool LoadFromFile(const std::string& filename);
    bool SaveToFile(const std::string& filename);

    void AddVertex(const ShapeVertex& vertex);
    void AddFace(const ShapeFace& face);

    int GetVertexCount() const;
    int GetFaceCount() const;

    //ShapeVertex GetVertex(int index) const;
    //ShapeFace GetFace(int index) const;

private: 
    //std::vector<ShapeVertex> m_vertices;
    //std::vector<ShapeFace> m_faces;
};