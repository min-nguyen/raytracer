
#include <typeinfo>
#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <string>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using namespace std;
#ifndef SHAPE2D_H
#define SHAPE2D_H

class Ray;
class Material;
class Intersection;
class LightSource;

class Shape2D{
public:
    const float L = 1000;// Length of Cornell Box side.
    std::string name;
    std::string id;
    vec3    color;
    vector<Material*> materials;
    
    Shape2D(){};
    Shape2D(vec3 color,
            vector<Material*> materials, 
            std::string id): 
            color(color), id(id),materials(materials) {
    };

    inline
    vec4 scalevec4(vec4 v) {
        v *= 2/L;
        v -= vec4(1,1,1,1);
        v.x *= -1;
        v.y *= -1;
        v.w = 1.0;
        return v;
    }
    inline
    vec4 unscalevec4(vec4 v) {
        v.x *= -1;
        v.y *= -1;
        v += vec4(1,1,1,1);
        v *= L/2;
        v.w = 1.0;
        return v;
    }
    vec3 scalevec3(vec3 v) {
        v *= 2/L;
        v -= vec3(1,1,1);
        v.x *= -1;
        v.y *= -1;
        return v;
    }
    float scalefloat(float v) {
        v *= 2/L;
        return v;
    }
    float unscalefloat(float v) {
        v *= L/2;
        return v;
    }
    virtual glm::vec3 getcolor(Intersection& intersection, const Ray& primary_ray, const std::vector<Shape2D*>& shapes, LightSource* lightSource) = 0;
    virtual bool intersect(Ray& ray, glm::vec3 dir, glm::vec4& intersectionpoint, glm::vec2* uv = nullptr) = 0;
    virtual glm::vec4 toworldcoordinates(glm::vec4 cam_intersect) = 0;
    virtual glm::vec3 getnormal(Intersection& intersection) = 0;
    virtual vec2 getUV(Intersection& intersectpoint) = 0;
    virtual vec3 minPosition() {
        return vec3();
    };
    virtual vec3 maxPosition() {
        return vec3();
    };
    virtual vec3 midpoint() {
        return vec3();
    };

    bool operator==(const Shape2D& other) const
    {
        // If the derived types are the same then compare them
        return typeid(*this) == typeid(other) && isEqual(other);
    }
private:
    virtual bool isEqual(const Shape2D& other) const = 0;
};

#endif
