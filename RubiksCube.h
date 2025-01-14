#ifndef RUBIKSCUBE_H
#define RUBIKSCUBE_H

#include <vector>
#include <glm/glm.hpp>
#include <sstream>

// Define the Transformation structure
struct Transformation {
    glm::vec3 rotationAxis; // Axis of rotation
    float rotationAngle;    // Total angle rotated
};

// Define the Cube structure
struct Cube {
    int id;
    glm::vec3 position;
    glm::vec3 initialPosition;
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    std::vector<Transformation> transformations; // Record transformations

    std::string toString() const {
        std::ostringstream oss;
        oss << "Cube ID: " << id << "\n";
        oss << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")\n";
        oss << "Initial Position: (" << initialPosition.x << ", " << initialPosition.y << ", " << initialPosition.z << ")\n";
        // Output rotation matrix as 4x4 matrix
        oss << "Rotation Matrix:\n";
        for (int i = 0; i < 4; ++i) {
            oss << "  ";
            for (int j = 0; j < 4; ++j) {
                oss << rotationMatrix[i][j] << " ";
            }
            oss << "\n";
        }
        oss << "Transformations:\n";
        if (transformations.empty()) {
            oss << "  None\n";
        } else {
            for (const auto& transformation : transformations) {
                oss << "  - Axis: (" << transformation.rotationAxis.x << ", "
                    << transformation.rotationAxis.y << ", " << transformation.rotationAxis.z << ")"
                    << ", Angle: " << transformation.rotationAngle << " degrees\n";
            }
        }
        return oss.str();
    }   

};

// Define the RubiksCube class
class RubiksCube {
private:
    std::vector<Cube> cubes; // All small cubes
    Cube centerCube;
    void initializeCubes(); 
    std::vector<int> findFaceIds(int face);
    void updateLocks(int rotatedFace, glm::vec3 axis,  std::vector<int> &faceIds);
    glm::vec3 locks = glm::vec3(0.0f);
public:
    RubiksCube(); // Constructor

    void rotateFace(int face, glm::vec3 axis, float angle);
    void resetCube();
    std::vector<Cube>& getCubes(); // Getter for cubes
};

#endif // RUBIKSCUBE_H
