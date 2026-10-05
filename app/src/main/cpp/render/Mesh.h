#pragma once

#include <GLES3/gl3.h>
#include <vector>
#include <cstdint>
#include <memory>
#include <string>

namespace nexvora {

struct Vertex {
    float px, py, pz;
    float nx, ny, nz;
    float u, v;
};

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void setVertices(const std::vector<Vertex>& vertices);
    void setIndices(const std::vector<uint32_t>& indices);
    void setName(const std::string& name) { m_name = name; }
    const std::string& name() const { return m_name; }

    void upload();
    void bind() const;
    void draw() const;
    void unbind() const;
    void release();

    bool isUploaded() const { return m_uploaded; }
    size_t indexCount() const { return m_indexCount; }
    size_t vertexCount() const { return m_vertices.size(); }

    static std::shared_ptr<Mesh> createPlane(float size = 10.0f, int subdivisions = 1);
    static std::shared_ptr<Mesh> createCube(float size = 1.0f);
    static std::shared_ptr<Mesh> createBox(float width, float height, float depth);
    static std::shared_ptr<Mesh> createSphere(float radius = 1.0f, int segments = 16);
    static std::shared_ptr<Mesh> createCylinder(float radius = 0.5f, float height = 2.0f, int segments = 16);
    static std::shared_ptr<Mesh> createRoadSegment(float length = 20.0f, float width = 8.0f);

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    size_t m_indexCount = 0;
    bool m_uploaded = false;
    std::string m_name;

    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
};

} // namespace nexvora
