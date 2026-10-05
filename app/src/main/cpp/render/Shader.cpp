#include "Shader.h"
#include "Logger.h"
#include <vector>

namespace nexvora {

Shader::~Shader() {
    destroy();
}

void Shader::destroy() {
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

GLuint Shader::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(static_cast<size_t>(logLen) + 1);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        NV_LOGE("Shader compile error (%s): %s",
                type == GL_VERTEX_SHADER ? "VS" : "FS", log.data());
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::linkProgram(GLuint vs, GLuint fs) {
    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);

    GLint status = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint logLen = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(static_cast<size_t>(logLen) + 1);
        glGetProgramInfoLog(m_program, logLen, nullptr, log.data());
        NV_LOGE("Shader link error: %s", log.data());
        glDeleteProgram(m_program);
        m_program = 0;
        return false;
    }
    return true;
}

bool Shader::loadFromSource(const char* vertexSrc, const char* fragmentSrc) {
    destroy();

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
    if (!vs) return false;

    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!fs) {
        glDeleteShader(vs);
        return false;
    }

    bool ok = linkProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    if (ok) {
        NV_LOGI("Shader program linked successfully (id=%u)", m_program);
    }
    return ok;
}

void Shader::use() const {
    glUseProgram(m_program);
}

void Shader::setMat4(const char* name, const math::Mat4& mat) const {
    GLint loc = glGetUniformLocation(m_program, name);
    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, mat.data());
    }
}

void Shader::setVec3(const char* name, const math::Vec3& v) const {
    GLint loc = glGetUniformLocation(m_program, name);
    if (loc >= 0) {
        glUniform3f(loc, v.x, v.y, v.z);
    }
}

void Shader::setVec4(const char* name, const math::Vec4& v) const {
    GLint loc = glGetUniformLocation(m_program, name);
    if (loc >= 0) {
        glUniform4f(loc, v.x, v.y, v.z, v.w);
    }
}

void Shader::setFloat(const char* name, float v) const {
    GLint loc = glGetUniformLocation(m_program, name);
    if (loc >= 0) {
        glUniform1f(loc, v);
    }
}

void Shader::setInt(const char* name, int v) const {
    GLint loc = glGetUniformLocation(m_program, name);
    if (loc >= 0) {
        glUniform1i(loc, v);
    }
}

} // namespace nexvora
