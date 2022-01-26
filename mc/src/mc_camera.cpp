#if 0
internal m4x4
Camera_CalculateViewMatrix(Camera_View* camera) {
    v3 direction;
    direction.x = cosf(Radians(camera->Orientation.y)) * cosf(Radians(camera->Orientation.x));
    direction.y = sinf(Radians(camera->Orientation.x));
    direction.z = sinf(Radians(camera->Orientation.y)) * cosf(Radians(camera->Orientation.x));
    v3 cameraForward = Normalize(direction);
    v3 cameraPosition = v3{camera->Position.x, camera->Position.y, camera->Position.z};
    
    v3 localRight = Cross(cameraForward, v3{0.0f, 1.0f, 0.0f});
    v3 localUp = Cross(localRight, cameraForward);
    
    
    camera->Forward = glm::vec3(cameraForward.x, cameraForward.y, cameraForward.z);
    return LookAt(cameraPosition,
                  cameraPosition + cameraForward, 
                  localUp);
}
#else
internal glm::mat4
Camera_CalculateViewMatrix(Camera_View* camera) {
    glm::vec3 direction;
    direction.x = cosf(Radians(camera->Orientation.y)) * cosf(Radians(camera->Orientation.x));
    direction.y = sinf(Radians(camera->Orientation.x));
    direction.z = sinf(Radians(camera->Orientation.y)) * cosf(Radians(camera->Orientation.x));
    camera->Forward = glm::normalize(direction);
    
    glm::vec3 localRight = glm::cross(camera->Forward, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 localUp = glm::cross(localRight, camera->Forward);
    
    return glm::lookAt(camera->Position,
                       camera->Position + camera->Forward, 
                       localUp);
}
#endif

#if 0
internal m4x4
Camera_CalculateProjectionMatrix(Camera_View* camera, f32 windowWidth, f32 windowHeight) {
    m4x4 projection;
    
#if 0
    GLM_IF_CONSTEXPR(GLM_CONFIG_CLIP_CONTROL == GLM_CLIP_CONTROL_LH_ZO) {
        Assert(!"LH_ZO\n");
    } else GLM_IF_CONSTEXPR(GLM_CONFIG_CLIP_CONTROL == GLM_CLIP_CONTROL_LH_NO) {
        Assert(!"LH_NO\n");
    } else GLM_IF_CONSTEXPR(GLM_CONFIG_CLIP_CONTROL == GLM_CLIP_CONTROL_RH_ZO) {
        Assert(!"RH_ZO\n");
    } else GLM_IF_CONSTEXPR(GLM_CONFIG_CLIP_CONTROL == GLM_CLIP_CONTROL_RH_NO) {
        printf("RH_NO\n");
    }
#endif
    
    projection = Perspective(Radians(camera->FOV), // FOV y
                             windowWidth/windowHeight,  // Aspect
                             0.1f,  // ZNear
                             10000.0f); // ZFar
    return projection;
}
#else
internal glm::mat4
Camera_CalculateProjectionMatrix(Camera_View* camera, f32 windowWidth, f32 windowHeight) {
    glm::mat4 projection;
    
    projection = glm::perspective(Radians(camera->FOV), // FOV y
                                  windowWidth/windowHeight,  // Aspect
                                  0.1f,  // ZNear
                                  10000.0f); // ZFar
    return projection;
}
#endif