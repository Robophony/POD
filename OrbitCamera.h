#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

class OrbitCamera
{
public:
    OrbitCamera(const glm::vec3& center, const glm::vec3& upVector, float radius, float minRadius, float azimuthAngle, float polarAngle, glm::mat4& view, glm::vec3 viewPoint);

    void rotateAzimuth(const float radians);
    void rotatePolar(const float radians);
    void zoom(const float by);

    void moveHorizontal(const float distance);
    void moveVertical(const float distance);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getEye() const;
    glm::vec3 getViewPoint() const;
    glm::vec3 getUpVector() const;
    glm::vec3 getNormalizedViewVector() const;
    float getAzimuthAngle() const;
    float getPolarAngle() const;
    float getRadius() const;

private:
    glm::mat4 view_;
    glm::vec3 center_; // Center of the orbit camera sphere (the point upon which the camera looks)
    glm::vec3 upVector_; // Up vector of the camera
    glm::vec3 viewPoint_;
    float radius_; // Radius of the orbit camera sphere
    float _minRadius; // Minimal radius of the orbit camera sphere (cannot fall below this value)
    float azimuthAngle_; // Azimuth angle on the orbit camera sphere
    float polarAngle_; // Polar angle on the orbit camera sphere
};