#ifndef MATERIAL_H
#define MATERIAL_H

#include <cmath>
#include "AGLM.h"
#include "ray.h"
#include "hittable.h"

using namespace glm;
using namespace std;

class material {
public:
  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const = 0;
  virtual ~material() {}
};

class lambertian : public material {
public:
  lambertian(const glm::color& a) : albedo(a) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const override 
  {
      glm::vec3 scatter_direction = rec.normal + random_unit_vector();
      if (near_zero(scatter_direction)) scatter_direction = rec.normal;
      scattered = ray(rec.p, scatter_direction);
      attenuation = albedo;
      return true;
  }

public:
  glm::color albedo;
};

class phong : public material {
public:
  phong(const glm::vec3& view) :
     diffuseColor(0,0,1), 
     specColor(1,1,1),
     ambientColor(.01f, .01f, .01f),
     lightPos(5,5,0),
     viewPos(view), 
     kd(0.45), ks(0.45), ka(0.1), shininess(10.0) 
  {}

  phong(const glm::color& idiffuseColor, 
        const glm::color& ispecColor,
        const glm::color& iambientColor,
        const glm::point3& ilightPos, 
        const glm::point3& iviewPos, 
        float ikd, float iks, float ika, float ishininess) : 
     diffuseColor(idiffuseColor), 
     specColor(ispecColor),
     ambientColor(iambientColor),
     lightPos(ilightPos),
     viewPos(iviewPos), kd(ikd), ks(iks), ka(ika), shininess(ishininess) 
  {}

  virtual bool scatter(const ray& r_in, const hit_record& hit, 
     glm::color& attenuation, ray& scattered) const override 
  {
     /*glm::vec3 L = lightPos - hit.p;
     glm::color Ia = ka*ambientColor; 
     glm::color Id = kd*diffuseColor*max(vec3(0,0,0),glm::dot(normalize(L), normalize(hit.normal)));
     glm::vec3 r = 2*glm::dot(normalize(L), normalize(hit.normal))*hit.normal-L; 
     glm::point3 v = viewPos - hit.p;
     glm::color Is;
     if(glm::dot(normalize(v), r)>0){
         Is = ks*specColor*pow(glm::dot(normalize(v),r),shininess); 
     }
     else Is = ks*specColor*glm::dot(normalize(v),r);
     glm::color If = Ia + Id + Is; 

     // copied from lambertian scattering
     glm::vec3 scatter_direction = hit.normal + random_unit_vector();
     if (near_zero(scatter_direction)) scatter_direction = hit.normal;
     scattered = ray(hit.p, scatter_direction);
     attenuation = If;
     return true;*/
     return false;
     
     // I followed my intuition and the algorithm you provided to a T, but no luck 
     // in getting it to render correctly (feel free to check how it works by
     // uncommenting the above code). Sad face
  }

public:
  glm::color diffuseColor;
  glm::color specColor;
  glm::color ambientColor;
  glm::point3 lightPos;
  glm::point3 viewPos; 
  float kd; 
  float ks;
  float ka; 
  float shininess;
};

class metal : public material {
public:
   metal(const glm::color& a, float f) : albedo(a), fuzz(glm::clamp(f,0.0f,1.0f)) {}

   virtual bool scatter(const ray& r_in, const hit_record& rec, 
      glm::color& attenuation, ray& scattered) const override 
   {
      //from section 9 
      vec3 reflected = reflect(normalize(r_in.direction()), rec.normal);
      scattered = ray(rec.p, reflected);
      attenuation = albedo;
      return (dot(scattered.direction(), rec.normal) > 0);
   }

public:
   glm::color albedo;
   float fuzz;
};

glm::vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
      auto cos_theta = fmin(dot(-uv, n), 1.0);
      vec3 vec = vec3(uv + vec3(cos_theta*n.x, cos_theta*n.y, cos_theta*n.z));
      vec3 r_out_perp = vec3(etai_over_etat * vec.x, etai_over_etat * vec.y, etai_over_etat * vec.z);
      float cnst = -sqrt(fabs(1.0 - dot(r_out_perp, r_out_perp)));
      vec3 r_out_parallel = vec3(cnst * n.x, cnst * n.y, cnst * n.z);
      return r_out_perp + r_out_parallel;
   }

class dielectric : public material {
public:
  dielectric(float index_of_refraction) : ir(index_of_refraction) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const override 
   {
      attenuation = color(1.0, 1.0, 1.0);
      double refraction_ratio = rec.front_face ? (1.0/ir) : ir;
      vec3 unit_direction = normalize(r_in.direction());
      vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);
      scattered = ray(rec.p, refracted);
      return true;
   }
   
public:
  float ir; // Index of Refraction
};

#endif

