#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Debugger.h>
#include <VertexBuffer.h>
#include <VertexBufferLayout.h>
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture.h>
#include <../src/Camera.h>
#include <RubiksCube.h>
#include <iostream>

/* Window size */
const unsigned int width = 800;
const unsigned int height = 800;
const float FOVdegree = 45.0f;  // Field Of View Angle
const float near = 0.1f;
const float far = 100.0f;

/* Shape cubeVertices coordinates with positions, colors, and corrected texCoords */
float cubeVertices[] = {
    // positions                     // colors            // texCoords
    // Front face (Red)
    -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,  // Top-left

    // Back face (Green)
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,  // Top-left

    // Left face (Blue)
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // Bottom-left
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,  // Bottom-right
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,  // Top-left

    // Right face (Yellow)
     0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f,  // Top-right
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f,  // Top-left

    // Bottom face (Cyan)
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f,  // Bottom-right
     0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f,  // Top-right
    -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f,  // Top-left

    // Top face (Magenta/Pink)
    -0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // Bottom-left
     0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 1.0f,    1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 1.0f,    0.0f, 1.0f,  // Top-left
};

/* Indices for cubeVertices order */
unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,       // Front face
    4, 5, 6, 6, 7, 4,       // Back face
    8, 9, 10, 10, 11, 8,    // Left face
    12, 13, 14, 14, 15, 12, // Right face
    16, 17, 18, 18, 19, 16, // Bottom face
    20, 21, 22, 22, 23, 20  // Top face
};


int main(int argc, char* argv[])
{
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }
    /* Set OpenGL to Version 3.3.0 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Load GLAD so it configures OpenGL */
    gladLoadGL();

    /* Control frame rate */
    glfwSwapInterval(1);

    /* Print OpenGL version after completing initialization */
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /* Set scope so that on widow close the destructors will be called automatically */
    {
        /* Blend to fix images with transperancy */
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        /* Generate VAO, VBO, EBO and bind them */
        VertexArray va;
        VertexBuffer vb(cubeVertices, sizeof(cubeVertices));
        IndexBuffer ib(indices, sizeof(indices));

        VertexBufferLayout layout;
        layout.Push<float>(3);  // positions
        layout.Push<float>(3);  // colors
        layout.Push<float>(2);  // texCoords
        va.AddBuffer(vb, layout);

        /* Create texture */
        Texture texture("res/textures/plane.png");
        texture.Bind();
         
        /* Create shaders */
        Shader shader("res/shaders/basic.shader");
        shader.Bind();

        /* Unbind all to prevent accidentally modifying them */
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        /* Enables the Depth Buffer */
    	GLCall(glEnable(GL_DEPTH_TEST));

        /* Create camera */
        Camera camera(width, height);
        // Change to perspective view
        //camera.SetOrthographic(near, far);
        camera.SetPerspective(near, far, FOVdegree);
        RubiksCube rubiksCube;
        camera.SetRubiksCube(&rubiksCube);
        camera.SetRenderingResources(&va, &ib, &shader);
        camera.EnableInputs(window);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window)){
            /* Set white background color */
            GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            /* Initialize uniform color */
            glm::vec4 color = glm::vec4(1.0, 1.0f, 1.0f, 1.0f);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 proj = camera.GetProjectionMatrix();
            for (const auto& cube : rubiksCube.getCubes()){
                glm::mat4 trans = glm::translate(glm::mat4(1.0f), cube.position+glm::vec3(0.0f, 0.0f, -10.0f));
                glm::mat4 rot = cube.rotationMatrix;
                glm::mat4 scl = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)); // No scaling

                /* Initialize the MVP matrices */ 
                glm::mat4 model = trans * rot * scl;
                glm::mat4 mvp = proj * view * model;
        
                /* Update shaders paramters and draw to the screen */
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                shader.SetUniform1i("u_Texture", 0);

                va.Bind();
                ib.Bind();
                GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
            }
            /* Update shaders paramters and draw to the screen */
            shader.SetUniform4f("u_Color", color);
            /* Swap front and back buffers */
            glfwSwapBuffers(window);
            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;
}