#pragma once

#include <GLES3/gl3.h>
#include <string>
#include "Math.h"

namespace nexvora {

class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    bool loadFromSource(const char* vertexSrc, const char* fragmentSrc);
    void use() const;
    void destroy();

    GLuint program() const { return m_program; }
    bool isValid() const { return m_program != 0; }

    // Uniform helpers
    void setMat4(const char* name, const math::Mat4& mat) const;
    void setVec3(const char* name, const math::Vec3& v) const;
    void setVec4(const char* name, const math::Vec4& v) const;
    void setFloat(const char* name, float v) const;
    void setInt(const char* name, int v) const;

private:
    GLuint compileShader(GLenum type, const char* source);
    bool linkProgram(GLuint vs, GLuint fs);

    GLuint m_program = 0;
};

} // namespace nexvora
