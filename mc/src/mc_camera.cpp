internal glm::mat4
Camera_CalculateViewMatrix(Camera_View* camera) {
    glm::vec3 direction;
    direction.x = cos(glm::radians(camera->Orientation.y)) * cos(glm::radians(camera->Orientation.x));
    direction.y = sin(glm::radians(camera->Orientation.x));
    direction.z = sin(glm::radians(camera->Orientation.y)) * cos(glm::radians(camera->Orientation.x));
    camera->Forward = glm::normalize(direction);
    
    glm::vec3 localRight = glm::cross(camera->Forward, glm::vec3(0, 1, 0));
    glm::vec3 localUp = glm::cross(localRight, camera->Forward);
    
    return glm::lookAt(camera->Position,
                       camera->Position + camera->Forward, 
                       localUp);
}

internal glm::mat4 
Camera_CalculateProjectionMatrix(Camera_View* camera, f32 windowWidth, f32 windowHeight) {
    glm::mat4 projection;
    
    projection = glm::perspective(glm::radians(camera->FOV), // FOV y
                                  windowWidth/windowHeight,  // Aspect
                                  0.1f,  // ZNear
                                  10000.0f); // ZFar
    return projection;
}