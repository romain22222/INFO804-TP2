/**
@file GraphicalObject.h
@author JOL
*/
#pragma once
#ifndef _GRAPHICAL_OBJECT_H_
#define _GRAPHICAL_OBJECT_H_

// In order to call opengl commands in all graphical objects
#include "Viewer.h"
#include "PointVector.h"
#include "Material.h"
#include "Ray.h"

/// Namespace RayTracer
namespace rt {

	/// This is an interface specifying methods that any graphical
	/// object should have. It is also drawable to be seen in QGLViewer
	/// window.
	/// Concrete exemples of a GraphicalObject include spheres.
	struct GraphicalObject {

		/// Default constructor. Nothing to do.
		GraphicalObject() {}

		/// Virtual destructor since object contains virtual methods.
		virtual ~GraphicalObject() {}

		/// This method is called by Scene::init() at the beginning of the
		/// display in the OpenGL window. May be useful for some
		/// precomputations.
		virtual void init( Viewer& /* viewer */ ) = 0;

		/// This method is called by Scene::draw() at each frame to
		/// redisplay objects in the OpenGL window.
		virtual void draw( Viewer& /* viewer */ ) = 0;

		/// @return the normal vector at point \a p on the object (\a p
		/// should be on or close to the sphere).
		virtual Vector3 getNormal( Point3 p ) = 0;

		/// @return the material associated to this part of the object
		virtual Material getMaterial( Point3 p ) = 0;

		/// @param[in] ray the incoming ray
		/// @param[out] returns the point of intersection with the object
		/// (if any), or the closest point to it.
		///
		/// @return either a real < 0.0 if there is an intersection, or a
		/// kind of distance to the closest point of intersection.
		virtual Real rayIntersection( const Ray& ray, Point3& p ) = 0;


	};

	struct PeriodicPlane : public GraphicalObject // Vous devez dériver de GraphicalObject
	{
		/// Creates a periodic infinite plane passing through \a c and
		/// tangent to \a u and \a v. Then \a w defines the width of the
		/// band around (0,0) and its period to put material \a band_m,
		/// otherwise \a main_m is used.
		Point3 _c;
		Vector3 _u;
		Vector3 _v;
		Material _main_m;
		Material _band_m;
		Real _w;
		PeriodicPlane( Point3 c, Vector3 u, Vector3 v,
					   Material main_m, Material band_m, Real w)
					   : _c(c), _u(u), _v(v), _main_m(main_m), _band_m(band_m), _w(w) {};

		void coordinates( Point3 p, Real& x, Real& y ) {
			x = p[0] + _u[0] + _v[0];
			y = p[1] + _u[1] + _v[1];
		}

		void init(Viewer & /* viewer */) override {}

		void draw(Viewer & /* viewer */ ) override
		{
			glBegin(GL_TRIANGLES);
			glColor4fv(_main_m.ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, _main_m.diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, _main_m.specular);
			glMaterialf(GL_FRONT, GL_SHININESS, _main_m.shinyness);
			glVertex3f(_c[0], _c[1], _c[2]);
			glVertex3f(_c[0] + _u[0], _c[1] + _u[1], _c[2] + _u[2]);
			glVertex3f(_c[0] + _v[0], _c[1] + _v[1], _c[2] + _v[2]);
			glEnd();

			glBegin(GL_TRIANGLES);
			glColor4fv(_main_m.ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, _main_m.diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, _main_m.specular);
			glMaterialf(GL_FRONT, GL_SHININESS, _main_m.shinyness);
			glVertex3f(_c[0] + _u[0] + _v[0], _c[1] + _u[1] + _v[1], _c[2] + _u[2] + _v[2]);
			glVertex3f(_c[0] + _u[0], _c[1] + _u[1], _c[2] + _u[2]);
			glVertex3f(_c[0] + _v[0], _c[1] + _v[1], _c[2] + _v[2]);

			glEnd();
		}

		Vector3 getNormal(Point3 /*p*/) override
		{
			return _u.cross(_v)/_u.cross(_v).norm();
		}

		Material getMaterial(Point3 p) override
		{
			Real x, y;
			coordinates(p, x, y);
			return ((int)(x + _w) - (int)x ) + ((int)(y + _w) - (int)y ) == 0 ? _main_m : _band_m;
		}

		Real rayIntersection(const Ray &ray, Point3 &p) override
		{
			Vector3 N = getNormal(_c);
			Vector3 V = ray.direction / ray.direction.norm();
			if (V.dot(N) == 0.0f || (_c - ray.origin).norm() == 0.0f) return 1;
			Real t = N.dot((_c - ray.origin) / (_c - ray.origin).norm()) / V.dot(N);
			if (t > 0) p = ray.origin + ray.direction * t;
			return -t;
		}
	};

} // namespace rt

#endif // #define _GRAPHICAL_OBJECT_H_
