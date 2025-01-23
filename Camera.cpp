#include <../src/Camera.h>
#include <thread> // Required for std::this_thread
#include <chrono> // Required for std::chrono::milliseconds

void Camera::SetOrthographic(float near, float far){
    m_Near = near;
    m_Far = far;

    // Rest Projection and View matrices
    m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, near, far);
    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
}

void Camera::SetPerspective(float near, float far, float FOV){
    if (near < 0.0f || far <= near || FOV <= 0.0f || FOV >= 180.0f || m_Width <= 0 || m_Height <= 0) {
        throw std::invalid_argument("Invalid camera parameters");
    }
    m_Near = near;
    m_Far = far;
    m_FOV = FOV;
    // Rest Projection and View matrices
    m_Projection = glm::perspective(glm::radians(m_FOV), float(m_Width) / m_Height, m_Near, m_Far);
    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
}

///////////////////
// Input Callbacks //
/////////////////////

void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods){
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
        return;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        float degreeMovement = camera->m_DegreeAmout;
        if(!camera->m_ClockwiseMovment)
                    degreeMovement *= -1;
        switch (key)
        {
            case GLFW_KEY_R:
                std::cout << "RIGHT Pressed" << std::endl;
                camera->remoteCubeFaceRotation(window, 0, glm::vec3(1.0f,0.0f,0.0f),degreeMovement);
                break;
            case GLFW_KEY_L:
                std::cout << "LEFT Pressed" << std::endl;
                camera->remoteCubeFaceRotation(window, 1, glm::vec3(1.0f,0.0f,0.0f),degreeMovement);
                break;
            case GLFW_KEY_U:
                std::cout << "UP Pressed" << std::endl;
                camera->remoteCubeFaceRotation(window, 2, glm::vec3(0.0f,1.0f,0.0f),degreeMovement);
                break;
            case GLFW_KEY_D:
                std::cout << "DOWN Pressed" << std::endl;
                camera->remoteCubeFaceRotation(window, 3, glm::vec3(0.0f,1.0f,0.0f),degreeMovement);
                break;
            case GLFW_KEY_B:
                std::cout << "BACK Pressed" << std::endl;
                camera->remoteCubeFaceRotation(window, 4, glm::vec3(0.0f,0.0f,1.0f),degreeMovement);
                break;
            case GLFW_KEY_F:
                std::cout << "FRONT Pressed" << std::endl;
                camera->remoteCubeFaceRotation(window, 5, glm::vec3(0.0f,0.0f,1.0f),degreeMovement);
                break;
            case GLFW_KEY_SPACE:
                std::cout << "SPACE - flipping rotation direction. Pressed" << std::endl;
                camera->m_ClockwiseMovment = !camera->m_ClockwiseMovment;
                break;
            case GLFW_KEY_A:
                std::cout << "A - multiply angle by 2 Pressed" << std::endl;
                if (camera->m_DegreeAmout != 180)
                    camera->m_DegreeAmout *=2;
                break;
            case GLFW_KEY_Z:
                std::cout << "Z - divide angle by 2 Pressed" << std::endl;
                if (camera->m_DegreeAmout != 45)
                    camera->m_DegreeAmout /=2;
                break;
            case GLFW_KEY_P:
                std::cout << "P - color picking" << std::endl;
                camera->m_PickingMode = !camera->m_PickingMode;
                break;
            case GLFW_KEY_M:
                std::cout << "M - mix" << std::endl;
                camera->m_RubiksCube->mixCube();
                break;
            case GLFW_KEY_UP:
                std::cout << "UP - rotate the cube upwards" << std::endl;
                camera->ArrowKeyCallback(GLFW_KEY_UP);
                break;
            case GLFW_KEY_DOWN:
                std::cout << "DOWN - rotate the cube downwards" << std::endl;
                camera->ArrowKeyCallback(GLFW_KEY_UP);
                break;
            case GLFW_KEY_RIGHT:
                std::cout << "RIGHT - rotate the cube right" << std::endl;
                camera->ArrowKeyCallback(GLFW_KEY_RIGHT);
                break;
            case GLFW_KEY_LEFT:
                std::cout << "LEFT - rotate the cube left" << std::endl;
                camera->ArrowKeyCallback(GLFW_KEY_LEFT);
                break;
            case GLFW_KEY_ESCAPE:
                std::cout << "ESCAPE - reset RubiksCube" << std::endl;
                camera->m_RubiksCube->resetCube();
                break;
            default:
                break;
        }
    }
}

void MouseButtonCallback(GLFWwindow* window, double currMouseX, double currMouseY) {
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    glfwGetCursorPos(window, &currMouseX, &currMouseY);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    if (camera->m_PickingMode && (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)) {
            // Clear the screen (Color and Depth Buffers)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Render cubes with unique colors for picking
            const auto& cubes = camera->m_RubiksCube->getCubes();
            for (size_t i = 0; i < cubes.size(); ++i) {
                const Cube& cube = cubes[i];

                // Encode cube ID into color
                int cubeID = cube.id + 1; // Avoid (0, 0, 0) black for no cube
                int r = glm::clamp((cubeID & 0xFF), 0, 255);
                int g = glm::clamp((cubeID >> 8) & 0xFF, 0, 255);
                int b = glm::clamp((cubeID >> 16) & 0xFF, 0, 255);
                glm::vec4 pickingColor = glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

                // Render the cube with the picking color
                camera->m_Shader->Bind();
                camera->m_Shader->SetUniform4f("u_Color", pickingColor);
                camera->m_Shader->SetUniform1i("u_PickingMode", true);

                // Update MVP matrix for the cube
                glm::mat4 model = glm::translate(glm::mat4(1.0f), cube.position+glm::vec3(0.0f, 0.0f, -10.0f)) * cube.rotationMatrix;
                glm::mat4 mvp = camera->GetProjectionMatrix() * camera->GetViewMatrix() * model;
                camera->m_Shader->SetUniformMat4f("u_MVP", mvp);

                // Bind VAO and draw
                camera->m_VA->Bind();
                camera->m_IB->Bind();
                GLCall(glDrawElements(GL_TRIANGLES, camera->m_IB->GetCount(), GL_UNSIGNED_INT, nullptr));
            }

            // Read the color at the mouse position
            unsigned char color[4];
            int x = static_cast<int>(currMouseX);
            int y = static_cast<int>(currMouseY);
            glReadPixels(x, height - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);


            // Decode color to get cube ID
            int cubeID = color[0] | (color[1] << 8) | (color[2] << 16);
            camera->m_pickedCubeID = cubeID - 1; // Convert back to index (subtract 1)

            if ( camera->m_pickedCubeID >= 0 &&  camera->m_pickedCubeID < static_cast<int>(cubes.size())) {
                std::cout << "Picked Cube ID: " <<  camera->m_pickedCubeID << std::endl;
            } else {
                std::cout << "No cube picked." << std::endl;
                 camera->m_pickedCubeID = -1; // Reset if no cube was picked
            }
            glFlush(); // Ensure rendering commands are executed
            camera->m_Shader->SetUniform1i("u_PickingMode", false);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        std::cout << "MOUSE LEFT Click" << std::endl;
        
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        std::cout << "MOUSE RIGHT Click" << std::endl;
    }   
}

void CursorPosCallback(GLFWwindow* window, double currMouseX, double currMouseY) {
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
        return;
    }

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Calculate delta movement
    camera->m_NewMouseX = camera->m_OldMouseX - currMouseX;
    camera->m_NewMouseY = camera->m_OldMouseY - currMouseY;
    float sensitivity = 0.1f;
    float deltaX = float(camera->m_NewMouseX) * sensitivity;
    float deltaY = float(camera->m_NewMouseY) * sensitivity;
    glm::vec3 cubeCenter(0.0f, 0.0f, -10.0f);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        
        if (camera->m_PickingMode &&  camera->m_pickedCubeID >= 0) {
            std::cout << "MOUSE LEFT Click - Rotate Cube ID: " << camera->m_pickedCubeID << std::endl;

            // Rotate the chosen cube based on mouse movement
            Cube& pickedCube = camera->m_RubiksCube->getCubes()[camera->m_pickedCubeID];

            // Rotate around Y axis for horizontal mouse movement
            glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(-deltaX), glm::vec3(0.0f, 1.0f, 0.0f));
            // Rotate around X axis for vertical mouse movement
            glm::vec3 right = glm::normalize(glm::cross(camera->getOrientation(), camera->getUp()));
            glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(-deltaY), right);

            // Combine the rotations and apply to the cube
            pickedCube.rotationMatrix = rotationY * rotationX * pickedCube.rotationMatrix;
        }
        else { // No color picking
            // Store initial vectors before rotation
            glm::vec3 initialLookDir = camera->getOrientation();
            glm::vec3 initialCubeDir = glm::normalize(cubeCenter - camera->getPosition());
            
            // Calculate the offset vector between look direction and cube direction
            glm::vec3 offset = initialLookDir - initialCubeDir;
            
            // Get current camera position relative to cube
            glm::vec3 relativePos = camera->getPosition() - cubeCenter;
            
            if (abs(deltaX) > abs(deltaY)) {
                // Rotate around global Y axis
                glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(deltaX), 
                    glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec4 newPosition = rotationY * glm::vec4(relativePos, 1.0f);
                // Also rotate the offset vector around Y
                glm::vec4 newOffset = rotationY * glm::vec4(offset, 0.0f);
                camera->setPosition(cubeCenter + glm::vec3(newPosition));
                offset = glm::vec3(newOffset);
            } else {
                // Rotate around camera's right vector
                glm::vec3 right = glm::normalize(glm::cross(initialLookDir, 
                    glm::vec3(0.0f, 1.0f, 0.0f)));
                glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(deltaY), right);
                glm::vec4 newPosition = rotationX * glm::vec4(relativePos, 1.0f);
                // Also rotate the offset vector around the right axis
                glm::vec4 newOffset = rotationX * glm::vec4(offset, 0.0f);
                camera->setPosition(cubeCenter + glm::vec3(newPosition));
                offset = glm::vec3(newOffset);
            }

            // Calculate new cube direction
            glm::vec3 newCubeDir = glm::normalize(cubeCenter - camera->getPosition());
            // Apply the rotated offset to maintain relative view
            glm::vec3 newLookDir = glm::normalize(newCubeDir + offset);
            
            camera->setOrientation(newLookDir);
            camera->UpdateCameraVectors(camera->getPosition() + newLookDir);
        }
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (camera->m_PickingMode &&  camera->m_pickedCubeID >= 0) {
            std::cout << "MOUSE RIGHT Click - Translating Cube ID: " <<  camera->m_pickedCubeID << std::endl;

            // Translate the chosen cube based on mouse movement
            Cube& pickedCube = camera->m_RubiksCube->getCubes()[camera->m_pickedCubeID];

            float moveSpeed = 0.05f;
            glm::vec3 right = glm::normalize(glm::cross(camera->getOrientation(), camera->getUp()));
            glm::vec3 translation = right * (-deltaX) * moveSpeed - camera->getUp() * (-deltaY) * moveSpeed;

            // Update cube's position
            pickedCube.position += translation;
        }
        else {
            std::cout << "MOUSE RIGHT Motion" << std::endl;
            float moveSpeed = 0.05f;
            glm::vec3 right = glm::normalize(glm::cross(camera->getOrientation(), camera->getUp()));
            
            // Calculate new position based on mouse movement
            glm::vec3 newPos = camera->getPosition();
            // Move opposite to mouse direction to create correct visual effect
            newPos -= right * (deltaX * moveSpeed);
            // When camera moves up (positive Y), cube appears to move down
            newPos -= camera->getUp() * (-deltaY * moveSpeed);
            camera->setPosition(newPos);
            
            // Update view matrix with new position but maintain original orientation
            camera->UpdateCameraVectors(camera->getPosition() + camera->getOrientation());
        }
    }

    camera->m_OldMouseX = currMouseX;
    camera->m_OldMouseY = currMouseY;
}

void ScrollCallback(GLFWwindow* window, double scrollOffsetX, double scrollOffsetY){
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) {
        std::cout << "Warning: Camera wasn't set as the Window User Pointer! ScrollCallback is skipped" << std::endl;
        return;
    }

    std::cout << "SCROLL Motion" << std::endl;
    
    glm::vec3 cubeCenter(0.0f, 0.0f, -10.0f);
    
    // Get current distance from cube
    glm::vec3 toCube = camera->getPosition() - cubeCenter;
    
    // Calculate zoom
    float zoomSpeed = 0.01f;
    float zoomFactor = 1.0f - float(scrollOffsetY) * zoomSpeed;

    camera->setPosition(cubeCenter + toCube * zoomFactor);
    
    camera->UpdateCameraVectors(camera->getPosition() + camera->getOrientation());
}

void Camera::UpdateCameraVectors(glm::vec3 position) {
    m_View = glm::lookAt(m_Position, position, m_Up); 
}

void Camera::EnableInputs(GLFWwindow* window){
    // Set camera as the user pointer for the window
    glfwSetWindowUserPointer(window, this);

    // Handle key inputs
    glfwSetKeyCallback(window, (void(*)(GLFWwindow *, int, int, int, int)) KeyCallback);

    // Handle cursor buttons
    glfwSetMouseButtonCallback(window, (void(*)(GLFWwindow *, int, int, int)) MouseButtonCallback);

    // Handle cursor position and inputs on motion
    glfwSetCursorPosCallback(window , (void(*)(GLFWwindow *, double, double)) CursorPosCallback);

    // Handle scroll inputs
    glfwSetScrollCallback(window, (void(*)(GLFWwindow *, double, double)) ScrollCallback);
}

void Camera::ArrowKeyCallback(int key) {
    float sensitivity = 0.1f; // Rotation sensitivity
    float deltaAngle = glm::radians(10.0f) * sensitivity; // Fixed rotation step size
    glm::vec3 cubeCenter(0.0f, 0.0f, -10.0f);

    // Store initial vectors before rotation
    glm::vec3 initialLookDir = getOrientation();
    glm::vec3 initialCubeDir = glm::normalize(cubeCenter - getPosition());

    // Calculate the offset vector between look direction and cube direction
    glm::vec3 offset = initialLookDir - initialCubeDir;

    // Get current camera position relative to cube
    glm::vec3 relativePos = getPosition() - cubeCenter;

    if (key == GLFW_KEY_RIGHT) {
        // Rotate around global Y axis to the right
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), deltaAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec4 newPosition = rotationY * glm::vec4(relativePos, 1.0f);
        glm::vec4 newOffset = rotationY * glm::vec4(offset, 0.0f);
        setPosition(cubeCenter + glm::vec3(newPosition));
        offset = glm::vec3(newOffset);
    } else if (key == GLFW_KEY_LEFT) {
        // Rotate around global Y axis to the left
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), -deltaAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec4 newPosition = rotationY * glm::vec4(relativePos, 1.0f);
        glm::vec4 newOffset = rotationY * glm::vec4(offset, 0.0f);
        setPosition(cubeCenter + glm::vec3(newPosition));
        offset = glm::vec3(newOffset);
    } else if (key == GLFW_KEY_UP) {
        // Rotate around camera's right vector upward
        glm::vec3 right = glm::normalize(glm::cross(initialLookDir, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), deltaAngle, right);
        glm::vec4 newPosition = rotationX * glm::vec4(relativePos, 1.0f);
        glm::vec4 newOffset = rotationX * glm::vec4(offset, 0.0f);
        setPosition(cubeCenter + glm::vec3(newPosition));
        offset = glm::vec3(newOffset);
    } else if (key == GLFW_KEY_DOWN) {
        // Rotate around camera's right vector downward
        glm::vec3 right = glm::normalize(glm::cross(initialLookDir, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), -deltaAngle, right);
        glm::vec4 newPosition = rotationX * glm::vec4(relativePos, 1.0f);
        glm::vec4 newOffset = rotationX * glm::vec4(offset, 0.0f);
        setPosition(cubeCenter + glm::vec3(newPosition));
        offset = glm::vec3(newOffset);
    }

    // Calculate new cube direction
    glm::vec3 newCubeDir = glm::normalize(cubeCenter - getPosition());
    // Apply the rotated offset to maintain relative view
    glm::vec3 newLookDir = glm::normalize(newCubeDir + offset);

    setOrientation(newLookDir);
    UpdateCameraVectors(getPosition() + newLookDir);
}


void Camera::render(GLFWwindow* window)
{
    /* Set white background color */
    GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
    /* Render here */
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    for (const auto &cube : m_RubiksCube->getCubes())
    {
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), cube.position + glm::vec3(0.0f, 0.0f, -10.0f));
        glm::mat4 rot = cube.rotationMatrix;
        glm::mat4 scl = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)); // No scaling

        // Compute Model-View-Projection (MVP) matrix
        glm::mat4 model = trans * rot * scl;
        glm::mat4 mvp = GetProjectionMatrix() * GetViewMatrix() * model;

        // Bind shader and set uniforms
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        m_Shader->SetUniform1i("u_Texture", 0);

        // Bind VAO and Index Buffer, then draw
        m_VA->Bind();
        m_IB->Bind();
        GLCall(glDrawElements(GL_TRIANGLES, m_IB->GetCount(), GL_UNSIGNED_INT, nullptr));
    }
    /* Update shaders paramters and draw to the screen */
    glm::vec4 color = glm::vec4(1.0, 1.0f, 1.0f, 1.0f);
    m_Shader->SetUniform4f("u_Color", color);
    /* Swap front and back buffers */
    glfwSwapBuffers(window);
    /* Poll for and process events */
    glfwPollEvents();
}

void Camera::remoteCubeFaceRotation(GLFWwindow* window, int face, glm::vec3 rotationAxis, float degree) {
    const int steps = 30; // Number of animation steps
    const float totalAnimationTime = 1.0f; // Total animation time in seconds
    const float stepDuration = totalAnimationTime / steps; // Duration of each step in seconds
    const float angleStep = degree / steps; // Angle to rotate per step

    float currentAngle = 0.0f;
    float isLocked = glm::dot(rotationAxis,m_RubiksCube->locks);
    std::cout << "if isLocked: " << isLocked << std::endl;
    if(isLocked == 0){
        for (int step = 0; step < steps; step++) {
            if(step != steps - 1) //not the final update
                m_RubiksCube->remoteCubeFaceRotation(face, rotationAxis, angleStep, 0.0f);
            else m_RubiksCube->remoteCubeFaceRotation(face, rotationAxis, angleStep, degree); // mark cube to add transition
            render(window);
            currentAngle += angleStep;
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(stepDuration * 300)));
        }
        float correctionAngle = degree - currentAngle;
        if (std::abs(correctionAngle) > 0.001f)
            m_RubiksCube->remoteCubeFaceRotation(face, rotationAxis, correctionAngle, 0.0f);
    }
}
