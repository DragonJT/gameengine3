#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct OrbitCamera
{
    glm::vec3 target;   // point we orbit around
    float distance;     // distance from target
    float yaw;          // radians
    float pitch;        // radians

    float fov;          // radians
    float aspect;
    float nearClip;
    float farClip;
};

static void orbitcamera_initialize(OrbitCamera *camera){
    camera->target    = glm::vec3(0.0f);
    camera->distance  = 5.0f;
    camera->yaw       = 0.0f;
    camera->pitch     = 0.0f;
    camera->fov       = glm::radians(60.0f);
    camera->aspect    = 16.0f / 9.0f;
    camera->nearClip  = 0.1f;
    camera->farClip   = 100.0f;
}

glm::vec3 orbitcamera_position(OrbitCamera *cam){
    float x = cam->distance * cosf(cam->pitch) * sinf(cam->yaw);
    float y = cam->distance * sinf(cam->pitch);
    float z = cam->distance * cosf(cam->pitch) * cosf(cam->yaw);
    return cam->target + glm::vec3(x, y, z);
}

glm::mat4 orbitcamera_view(OrbitCamera *cam)
{
    glm::vec3 position = orbitcamera_position(cam);

    return glm::lookAt(
        position,
        cam->target,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

glm::mat4 orbitcamera_proj(OrbitCamera *cam)
{
    return glm::perspective(
        cam->fov,
        cam->aspect,
        cam->nearClip,
        cam->farClip
    );
}

void orbitcamera_rotate(
    OrbitCamera *cam,
    float deltaX,
    float deltaY,
    float sensitivity = 0.005f)
{
    cam->yaw   -= deltaX * sensitivity;
    cam->pitch -= deltaY * sensitivity;

    // Clamp pitch to avoid flipping
    float limit = glm::radians(89.0f);
    cam->pitch = glm::clamp(cam->pitch, -limit, limit);
}

void orbitcamera_zoom(
    OrbitCamera *cam,
    float scrollDelta,
    float zoomSpeed = 0.5f)
{
    cam->distance -= scrollDelta * zoomSpeed;
    cam->distance = glm::max(cam->distance, 0.1f);
}
