#include "OrbitCamera.h"

#define M_PI 3.1415926535897932384626433832795

OrbitCamera::OrbitCamera(const glm::vec3& center, const glm::vec3& upVector, float radius, float minRadius, float azimuthAngle, float polarAngle, glm::mat4 &view, glm::vec3 viewPoint) {
    center_ = center;
    upVector_ = upVector;
    radius_ = radius;
    _minRadius = minRadius;
    azimuthAngle_ = azimuthAngle;
    polarAngle_ = polarAngle;
    view_ = view;
    viewPoint_ = viewPoint;
}

void OrbitCamera::rotateAzimuth(const float radians)
{
    azimuthAngle_ += radians;

    // Keep azimuth angle within range <0..2PI) - it's not necessary, just to have it nicely output
    const auto fullCircle = 2.0f * M_PI;
    azimuthAngle_ = fmodf(azimuthAngle_, fullCircle);
    if (azimuthAngle_ < 0.0f) {
        azimuthAngle_ = fullCircle + azimuthAngle_;
    }
}

void OrbitCamera::rotatePolar(const float radians)
{
    polarAngle_ += radians;

    // Check if the angle hasn't exceeded quarter of a circle to prevent flip, add a bit of epsilon like 0.001 radians
    const auto polarCap = M_PI / 2.0f - 0.001f;
    if (polarAngle_ > polarCap) {
        polarAngle_ = polarCap;
    }

    if (polarAngle_ < -polarCap) {
        polarAngle_ = -polarCap;
    }
}

void OrbitCamera::zoom(const float by)
{
    radius_ -= by;
    if (radius_ < _minRadius) {
        radius_ = _minRadius;
    }
    if (radius_ > 30.0) {
        radius_ += by;
    }
}

glm::mat4 OrbitCamera::getViewMatrix() const {
    return view_;
}

glm::vec3 OrbitCamera::getEye() const
{
    // Calculate sines / cosines of angles
    const auto sineAzimuth = sin(azimuthAngle_);
    const auto cosineAzimuth = cos(azimuthAngle_);
    const auto sinePolar = sin(polarAngle_);
    const auto cosinePolar = cos(polarAngle_);

    // Calculate eye position out of them
    const auto x = center_.x + radius_ * cosinePolar * cosineAzimuth;
    const auto y = center_.y + radius_ * sinePolar;
    const auto z = center_.z + radius_ * cosinePolar * sineAzimuth;

    return glm::vec3(x, y, z);
}

glm::vec3 OrbitCamera::getViewPoint() const {
    return viewPoint_;
}
glm::vec3 OrbitCamera::getUpVector() const {
    return upVector_;
}
glm::vec3 OrbitCamera::getNormalizedViewVector() const {
    return glm::normalize(getViewPoint());
}
float OrbitCamera::getAzimuthAngle() const {
    return azimuthAngle_;
}
float OrbitCamera::getPolarAngle() const {
    return polarAngle_;
}
float OrbitCamera::getRadius() const {
    return radius_;
}

void OrbitCamera::moveHorizontal(const float distance)
{
    const auto position = getEye();
    const glm::vec3 viewVector = getNormalizedViewVector();
    const glm::vec3 strafeVector = glm::normalize(glm::cross(viewVector, upVector_));
    center_ += strafeVector * distance;
}

void OrbitCamera::moveVertical(const float distance)
{
    center_ += upVector_ * distance;
}