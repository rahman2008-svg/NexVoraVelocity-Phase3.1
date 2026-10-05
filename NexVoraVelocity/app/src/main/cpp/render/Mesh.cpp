#include "Mesh.h"
#include "Logger.h"
#include <cstring>
#include <cstddef>
#include <cmath>

namespace nexvora {

Mesh::~Mesh() {
    release();
}

void Mesh::release() {
    if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
    if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
    if (m_ebo) { glDeleteBuffers(1, &m_ebo); m_ebo = 0; }
    m_uploaded = false;
}

void Mesh::setVertices(const std::vector<Vertex>& vertices) {
    m_vertices = vertices;
    m_uploaded = false;
}

void Mesh::setIndices(const std::vector<uint32_t>& indices) {
    m_indices = indices;
    m_indexCount = indices.size();
    m_uploaded = false;
}

void Mesh::upload() {
    if (m_vertices.empty() || m_indices.empty()) {
        NV_LOGE("Mesh::upload - empty data (%s)", m_name.c_str());
        return;
    }

    if (m_vao == 0) {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);
    }

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)),
                 m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(m_indices.size() * sizeof(uint32_t)),
                 m_indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, px)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, nx)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, u)));

    glBindVertexArray(0);
    m_uploaded = true;
}

void Mesh::bind() const { glBindVertexArray(m_vao); }
void Mesh::draw() const {
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
}
void Mesh::unbind() const { glBindVertexArray(0); }

std::shared_ptr<Mesh> Mesh::createPlane(float size, int subdivisions) {
    auto mesh = std::make_shared<Mesh>();
    mesh->setName("Plane");
    int segs = std::max(1, subdivisions);
    float step = size / static_cast<float>(segs);
    float h = size * 0.5f;

    std::vector<Vertex> verts;
    std::vector<uint32_t> indices;
    verts.reserve((segs + 1) * (segs + 1));

    for (int z = 0; z <= segs; ++z) {
        for (int x = 0; x <= segs; ++x) {
            float px = -h + x * step;
            float pz = -h + z * step;
            float u = static_cast<float>(x) / segs;
            float v = static_cast<float>(z) / segs;
            verts.push_back({px, 0.0f, pz, 0.0f, 1.0f, 0.0f, u, v});
        }
    }

    for (int z = 0; z < segs; ++z) {
        for (int x = 0; x < segs; ++x) {
            uint32_t i0 = z * (segs + 1) + x;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = i0 + (segs + 1);
            uint32_t i3 = i2 + 1;
            indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
            indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
        }
    }

    mesh->setVertices(verts);
    mesh->setIndices(indices);
    return mesh;
}

std::shared_ptr<Mesh> Mesh::createCube(float size) {
    return createBox(size, size, size);
}

std::shared_ptr<Mesh> Mesh::createBox(float width, float height, float depth) {
    auto mesh = std::make_shared<Mesh>();
    mesh->setName("Box");
    float hx = width * 0.5f, hy = height * 0.5f, hz = depth * 0.5f;

    std::vector<Vertex> verts = {
        {-hx,-hy, hz, 0,0,1, 0,0},{ hx,-hy, hz, 0,0,1, 1,0},{ hx, hy, hz, 0,0,1, 1,1},{-hx, hy, hz, 0,0,1, 0,1},
        { hx,-hy,-hz, 0,0,-1,0,0},{-hx,-hy,-hz, 0,0,-1,1,0},{-hx, hy,-hz, 0,0,-1,1,1},{ hx, hy,-hz, 0,0,-1,0,1},
        {-hx, hy, hz, 0,1,0, 0,0},{ hx, hy, hz, 0,1,0, 1,0},{ hx, hy,-hz, 0,1,0, 1,1},{-hx, hy,-hz, 0,1,0, 0,1},
        {-hx,-hy,-hz, 0,-1,0,0,0},{ hx,-hy,-hz, 0,-1,0,1,0},{ hx,-hy, hz, 0,-1,0,1,1},{-hx,-hy, hz, 0,-1,0,0,1},
        { hx,-hy, hz, 1,0,0, 0,0},{ hx,-hy,-hz, 1,0,0, 1,0},{ hx, hy,-hz, 1,0,0, 1,1},{ hx, hy, hz, 1,0,0, 0,1},
        {-hx,-hy,-hz,-1,0,0, 0,0},{-hx,-hy, hz,-1,0,0, 1,0},{-hx, hy, hz,-1,0,0, 1,1},{-hx, hy,-hz,-1,0,0, 0,1},
    };

    std::vector<uint32_t> indices;
    for (uint32_t f = 0; f < 6; ++f) {
        uint32_t b = f * 4;
        indices.push_back(b+0); indices.push_back(b+1); indices.push_back(b+2);
        indices.push_back(b+0); indices.push_back(b+2); indices.push_back(b+3);
    }
    mesh->setVertices(verts);
    mesh->setIndices(indices);
    return mesh;
}

std::shared_ptr<Mesh> Mesh::createSphere(float radius, int segments) {
    auto mesh = std::make_shared<Mesh>();
    mesh->setName("Sphere");
    int seg = std::max(8, segments);
    int rings = seg / 2;

    std::vector<Vertex> verts;
    std::vector<uint32_t> indices;

    for (int y = 0; y <= rings; ++y) {
        float v = static_cast<float>(y) / rings;
        float phi = v * math::PI;
        float sy = std::sin(phi), cy = std::cos(phi);
        for (int x = 0; x <= seg; ++x) {
            float u = static_cast<float>(x) / seg;
            float theta = u * math::TWO_PI;
            float sx = std::sin(theta), cx = std::cos(theta);
            float nx = sx * sy, ny = cy, nz = cx * sy;
            verts.push_back({nx*radius, ny*radius, nz*radius, nx, ny, nz, u, v});
        }
    }

    for (int y = 0; y < rings; ++y) {
        for (int x = 0; x < seg; ++x) {
            uint32_t i0 = y * (seg + 1) + x;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = i0 + (seg + 1);
            uint32_t i3 = i2 + 1;
            indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
            indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
        }
    }
    mesh->setVertices(verts);
    mesh->setIndices(indices);
    return mesh;
}

std::shared_ptr<Mesh> Mesh::createCylinder(float radius, float height, int segments) {
    auto mesh = std::make_shared<Mesh>();
    mesh->setName("Cylinder");
    int seg = std::max(8, segments);
    float hh = height * 0.5f;

    std::vector<Vertex> verts;
    std::vector<uint32_t> indices;

    // Side
    for (int i = 0; i <= seg; ++i) {
        float u = static_cast<float>(i) / seg;
        float theta = u * math::TWO_PI;
        float cx = std::cos(theta), sx = std::sin(theta);
        verts.push_back({cx*radius, -hh, sx*radius, cx, 0, sx, u, 0});
        verts.push_back({cx*radius,  hh, sx*radius, cx, 0, sx, u, 1});
    }
    for (int i = 0; i < seg; ++i) {
        uint32_t b = i * 2;
        indices.push_back(b); indices.push_back(b+1); indices.push_back(b+2);
        indices.push_back(b+1); indices.push_back(b+3); indices.push_back(b+2);
    }

    // Caps
    uint32_t base = static_cast<uint32_t>(verts.size());
    verts.push_back({0, -hh, 0, 0, -1, 0, 0.5f, 0.5f});
    verts.push_back({0,  hh, 0, 0,  1, 0, 0.5f, 0.5f});
    for (int i = 0; i <= seg; ++i) {
        float theta = static_cast<float>(i) / seg * math::TWO_PI;
        float cx = std::cos(theta), sx = std::sin(theta);
        verts.push_back({cx*radius, -hh, sx*radius, 0, -1, 0, cx*0.5f+0.5f, sx*0.5f+0.5f});
        verts.push_back({cx*radius,  hh, sx*radius, 0,  1, 0, cx*0.5f+0.5f, sx*0.5f+0.5f});
    }
    for (int i = 0; i < seg; ++i) {
        uint32_t bi = base + 2 + i * 2;
        indices.push_back(base); indices.push_back(bi); indices.push_back(bi + 2);
        indices.push_back(base+1); indices.push_back(bi + 3); indices.push_back(bi + 1);
    }

    mesh->setVertices(verts);
    mesh->setIndices(indices);
    return mesh;
}

std::shared_ptr<Mesh> Mesh::createRoadSegment(float length, float width) {
    auto mesh = std::make_shared<Mesh>();
    mesh->setName("Road");
    float hl = length * 0.5f, hw = width * 0.5f;

    std::vector<Vertex> verts = {
        {-hw, 0.01f, -hl, 0,1,0, 0, 0},
        { hw, 0.01f, -hl, 0,1,0, 1, 0},
        { hw, 0.01f,  hl, 0,1,0, 1, 1},
        {-hw, 0.01f,  hl, 0,1,0, 0, 1},
    };
    std::vector<uint32_t> indices = {0,1,2, 0,2,3};
    mesh->setVertices(verts);
    mesh->setIndices(indices);
    return mesh;
}

} // namespace nexvora
