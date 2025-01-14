#include "RubiksCube.h"
#include <glm/gtc/matrix_transform.hpp> // For glm::rotate, glm::translate
#include <iostream>

// Constructor
RubiksCube::RubiksCube() {
    initializeCubes();
}

// Initialize all cubes in a 3x3x3 grid
void RubiksCube::initializeCubes() {
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                Cube cubie;
                cubie.id = cubes.size();
                cubie.position = glm::vec3(x, y, z);
                cubie.initialPosition = cubie.position;
                cubes.push_back(cubie);
                if (x == 0 && y == 0 && z == 0)
                    centerCube = cubie;
            }
        }
    }
}

std::string toStringVec3(const glm::vec3& vec) {
    std::ostringstream oss;
    oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return oss.str();
}

std::vector<int> RubiksCube::findFaceIds(int face){
    std::vector<int> faceIds; // first id belongs to the center cubie
    float epsilon = 0.5f;
    switch (face){ // face: right = 0, left = 1, up = 2, down = 3, back = 4, front = 5
        case 0: // right, axis x
            for(const Cube& cubie : cubes){
                if (cubie.position.x > centerCube.position.x + epsilon){
                    faceIds.push_back(cubie.id);
                }
            }
            return faceIds;
        case 1: // left, axis x
            for(const Cube& cubie : cubes){
                if (cubie.position.x < centerCube.position.x - epsilon){
                    faceIds.push_back(cubie.id);
                }
            }
            return faceIds;
        case 2: // up, axis y
            for(const Cube& cubie : cubes){
                if (cubie.position.y > centerCube.position.y + epsilon){
                    faceIds.push_back(cubie.id);
                }
            }
            return faceIds;
        case 3: // down, axis y
            for(const Cube& cubie : cubes){
                if (cubie.position.y < centerCube.position.y - epsilon){
                    faceIds.push_back(cubie.id);
                }
            }
            return faceIds;
        case 4: // back, axis z
            for(const Cube& cubie : cubes){
                if (cubie.position.z < centerCube.position.z - epsilon){
                    faceIds.push_back(cubie.id);
                }
            }
            return faceIds;
        case 5: // front, axis z
            for(const Cube& cubie : cubes){
                if (cubie.position.z > centerCube.position.z + epsilon){
                    faceIds.push_back(cubie.id);
                }
            }
            return faceIds;
        default:
            std::cerr << "Error: RubikCube - invalid face choice" << std::endl;
            return {};
    }
}

bool isFraction(glm::vec3 vector){
    const float epsilon = 1e-6f; // Small threshold for floating-point precision
    float fracPartX = vector.x - std::floor(vector.x);
    float fracPartY = vector.y - std::floor(vector.y);
    float fracPartZ = vector.z - std::floor(vector.z);
    
    return (std::abs(fracPartX) > epsilon && std::abs(fracPartX - 1.0f) > epsilon) ||
           (std::abs(fracPartY) > epsilon && std::abs(fracPartY - 1.0f) > epsilon) ||
           (std::abs(fracPartZ) > epsilon && std::abs(fracPartZ - 1.0f) > epsilon);
}

void RubiksCube::updateLocks(int rotatedFace, glm::vec3 axis, std::vector<int> &faceIds){
    glm::vec3 l1 = cubes[faceIds[0]].position;
    glm::vec3 l2 = cubes[faceIds[1]].position;
    if (isFraction(l1) || isFraction(l2)){ // locks axis
            locks= glm::vec3(1.0f)- axis;
    }
    else{ // check if can release lock - {0,1} {2,3} {4,5}
        int twinFace = -1;
        if(rotatedFace % 2 == 0)
            twinFace = rotatedFace + 1;  
        else
            twinFace = rotatedFace - 1;
        std::vector<int> twinFaceIds = findFaceIds(twinFace); 
        glm::vec3 twinl1 = cubes[twinFaceIds[0]].position;
        glm::vec3 twinl2 = cubes[twinFaceIds[1]].position;
        if (!isFraction(twinl1) && !isFraction(twinl2)){ // release locks axis
            locks = glm::vec3(0.0f);
        }
    }
}

// Rotate a face by applying a transformation to the cubes in that face
void RubiksCube::rotateFace(int face, glm::vec3 axis, float angle) { // face: right = 0, left =1, up =2, down = 3, back = 4, front = 5
    float isLocked = glm::dot(axis,locks);
    std::cout << "isLocked: " << isLocked << std::endl;
    if(isLocked == 0){
        std::vector<int> faceIds = findFaceIds(face);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
        for (int id : faceIds) {
            glm::vec4 newPosition = rotationMatrix * glm::vec4(cubes[id].position, 1.0f);
            cubes[id].position = glm::vec3(newPosition);
            cubes[id].rotationMatrix = rotationMatrix * cubes[id].rotationMatrix;
            cubes[id].transformations.push_back({axis, angle});
            std::cout << cubes[id].toString() << std::endl;
        }
        // lock/unlock axises if needed
        updateLocks(face, axis, faceIds);
    }
}

// Reset the Rubik's Cube to its initial state
void RubiksCube::resetCube() {
    for (Cube& cubie : cubes) {
        cubie.position = cubie.initialPosition;
        cubie.rotationMatrix = glm::mat4(1.0f);
        cubie.transformations.clear();
    }
}

// Getter for the cubes
std::vector<Cube>& RubiksCube::getCubes() {
    return cubes;
}


