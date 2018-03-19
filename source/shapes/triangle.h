#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <cmath>
#include <glm/glm.hpp>
#include "shape2D.h"
#include "../light/intersection.h"
#include "../light/ray.h"
#include <initializer_list>
#include "../materials/material_properties/gloss.h"
#include "../materials/material.h"
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3; 

class Triangle : public Shape2D
{
public:
    Shape3D* shape3D = nullptr;
    float area;
    glm::vec4 v0;
    glm::vec4 v1;
    glm::vec4 v2;
    
    glm::vec4 normal;
    std::vector<std::vector<vec3>> v;
    Triangle(glm::vec4 v0, 
             glm::vec4 v1, 
             glm::vec4 v2, 
             glm::vec3 color, 
             const std::initializer_list<Material*>& materials = std::initializer_list<Material*>())
        :   Shape2D(color, materials), 
            v0((scalevec4(v0))), v1(scalevec4(v1)), v2(scalevec4(v2)), normal(ComputeNormal())
    {
    }
    Triangle(glm::vec4 v0, 
             glm::vec4 v1, 
             glm::vec4 v2, 
             glm::vec3 color, 
             bool scale,
             const std::initializer_list<Material*>& materials = std::initializer_list<Material*>()
             )
        :   Shape2D(color, materials), 
            v0(v0), v1(v1), v2(v2), normal(ComputeNormal())
    {
    }
    virtual vec2 getUV(Intersection& intersectpoint) override {
        // if (!hasTextures) return false;


        //Use the same code as for getNormalAt to get the barycentric points
        vec3 e1 = vec3(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
        vec3 e2 = vec3(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
        vec3 b = vec3(intersectpoint.position.x - v0.x, intersectpoint.position.y - v0.y, intersectpoint.position.z - v0.z);
        mat3 A( -(vec3)intersectpoint.direction, e1, e2 );
        vec3 x = glm::inverse( A ) * b;
        
    
        return vec2(x.y,x.z);
    }
    virtual bool intersect(Ray& ray, vec3 dir, vec4& intersectionpoint) override {
        vec3 e1 = vec3(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
        vec3 e2 = vec3(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
        vec3 b = vec3(ray.position.x - v0.x, ray.position.y - v0.y, ray.position.z - v0.z);
        mat3 A( -dir, e1, e2 );
        vec3 x = glm::inverse( A ) * b;

        float t = x.x;
        float u = x.y;
        float v = x.z; 
        if (0 <= t && 0 <= u && 0 <= v && u + v <= 1) { 
            intersectionpoint = toworldcoordinates(vec4(t,u,v,1));
            return true;
        } 
        return false;
    }

    virtual glm::vec3 getcolor(Intersection& intersection, const Ray& primary_ray, const std::vector<Shape2D*>& shapes, LightSource* lightSource) override{
        
        vector<vec3> colors;
        for(int a = 0; a < materials.size(); a++){
            colors.push_back(materials[a]->material_color(intersection, primary_ray, shapes, lightSource));
        }
        vec3 t_color = vec3(0,0,0);
        for(int a = 0; a < colors.size(); a++){
            if(a == 0){
               t_color = colors[0];
            }
            else
               t_color = glm::mix(t_color, colors[a], 0.5f);
        }

        return t_color;
    }
    virtual vec4 toworldcoordinates(glm::vec4 cam_intersect) override {
        float u = cam_intersect[1], v = cam_intersect[2];
        vec4 e1 = v1 - v0;
        vec4 e2 = v2 - v0;
        return v0 + u * e1 + v * e2;
    }

    virtual vec3 getnormal(Intersection& intersection){

        vec3 a =  (vec3) glm::normalize(glm::triangleNormal((vec3) v0, (vec3) v1, (vec3) v2));
        vec3 b = -a;
        if(glm::dot(a, (vec3) glm::normalize(intersection.direction)) <= 0){
            return a;
        }
        else{
            return b;
        }
    }

    virtual vec3 minPosition() override {
        vec3 minPos = (vec3)v0;
        for (int i = 0; i < 3; i++) {
            if ( v1[i] < minPos[i] ) minPos[i] = v1[i];
        }
        for (int i = 0; i < 3; i++) {
            if ( v2[i] < minPos[i] ) minPos[i] = v2[i];
        }
        return minPos;
    }

    virtual vec3 maxPosition() override {
        vec3 maxPos = (vec3)v0;
        for (int i = 0; i < 3; i++) {
            if ( v1[i] > maxPos[i] ) maxPos[i] = v1[i];
        }
        for (int i = 0; i < 3; i++) {
            if ( v2[i] > maxPos[i] ) maxPos[i] = v2[i];
        }
        return maxPos;
    }

    virtual vec3 midpoint() override {
        vec3 point = vec3(0, 0, 0);
        point.x = (v0.x + v1.x + v2.x) / 3.0;
        point.y = (v0.y + v1.y + v2.y) / 3.0;
        point.z = (v0.z + v1.z + v2.z) / 3.0;
        return point;
    }

private:
    virtual bool isEqual(const Shape2D& other) const override {
        Triangle tri = static_cast<const Triangle&>(other);
        if( (v0 == tri.v0 && v1 == tri.v1 && v2 == tri.v2) ||
            (v0 == tri.v0 && v1 == tri.v2 && v2 == tri.v1) ||
            (v0 == tri.v1 && v1 == tri.v2 && v2 == tri.v0) ||
            (v0 == tri.v1 && v1 == tri.v0 && v2 == tri.v2) ||
            (v0 == tri.v2 && v1 == tri.v0 && v2 == tri.v1) ||
            (v0 == tri.v2 && v1 == tri.v1 && v2 == tri.v0)){
            return true;
        }
        return false;
    }

    vec4 ComputeNormal()
    {
        vec4 norm;
        glm::vec3 e1 = glm::vec3(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
        glm::vec3 e2 = glm::vec3(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);
        glm::vec3 normal3 = glm::normalize( glm::cross( e2, e1 ) );
        norm.x = normal3.x;
        norm.y = normal3.y;
        norm.z = normal3.z;
        norm.w = 1.0;

        return norm;
    }
};
#endif