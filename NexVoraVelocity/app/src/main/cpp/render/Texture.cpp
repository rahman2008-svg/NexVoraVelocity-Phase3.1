#include "Texture.h"
#include "Logger.h"
#include <vector>

namespace nexvora {

Texture::~Texture() {
    if (m_id) {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
}

bool Texture::create(int width, int height, const uint8_t* rgbaData) {
    if (m_id) {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_width = width;
    m_height = height;
    NV_LOGD("Texture created %dx%d id=%u", width, height, m_id);
    return true;
}

bool Texture::createSolidColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    uint8_t pixel[4] = {r, g, b, a};
    return create(1, 1, pixel);
}

void Texture::bind(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace nexvora
