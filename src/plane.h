#ifndef PLANE_H_
#define PLANE_H_

#include "hittable.h"
#include "AGLM.h"

class plane : public hittable {
public:
   plane() : a(0), n(0), mat_ptr(0) {}
   plane(const glm::point3& p, const glm::vec3& normal, 
      std::shared_ptr<material> m) : a(p), n(normal), mat_ptr(m) {};

   virtual bool hit(const ray& r, hit_record& rec) const override
   {
      float denom = glm::dot(normalize(r.direction()), n);
      if(denom!=0){
         float t = glm::dot(a-r.origin(), n)/denom;
         if(t<0) return false;
         // save relevant data in hit record
         rec.t = t; // save the time when we hit the object
         rec.p = r.at(t); // ray.origin + t * ray.direction
         rec.mat_ptr = mat_ptr; 
         rec.set_face_normal(r, n);
         return true;
      }
      else {
         return false;
      }
   }

public:
   glm::vec3 a;
   glm::vec3 n;
   std::shared_ptr<material> mat_ptr;
};

#endif
