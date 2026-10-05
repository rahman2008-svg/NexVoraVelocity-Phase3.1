#pragma once

#include <GLES3/gl3.h>
#include <cstdint>
#include <string>

namespace nexvora {

class Texture {
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    bool create(int width, int height, const uint8_t* rgbaData);
    bool createSolidColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    void bind(GLuint unit = 0) const;
    void unbind() const;

    GLuint id() const { return m_id; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    bool isValid() const { return m_id != 0; }

private:
    GLuint m_id = 0;
    int m_width = 0;
    int m_height = 0;
};

} // namespace nexvora
