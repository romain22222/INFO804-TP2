/**
@file Renderer.h
@author JOL
*/
#pragma once
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Color.h"
#include "Image2D.h"
#include "Ray.h"
#include "Material.h"

/// Namespace RayTracer
namespace rt {

	Real randomBetween(Real i, Real j) {
		return Real(i + static_cast <float> (rand()) / ( static_cast <float> (RAND_MAX/j-i)));
	}

	inline void progressBar( std::ostream& output,
													 const double currentValue, const double maximumValue)
	{
		static const int PROGRESSBARWIDTH = 60;
		static int myProgressBarRotation = 0;
		static int myProgressBarCurrent = 0;
		// how wide you want the progress meter to be
		double fraction = currentValue /maximumValue;

		// part of the progressmeter that's already "full"
		int dotz = static_cast<int>(floor(fraction * PROGRESSBARWIDTH));
		if (dotz > PROGRESSBARWIDTH) dotz = PROGRESSBARWIDTH;

		// if the fullness hasn't changed skip display
		if (dotz == myProgressBarCurrent) return;
		myProgressBarCurrent = dotz;
		myProgressBarRotation++;

		// create the "meter"
		int ii=0;
		output << "[";
		// part	that's full already
		for ( ; ii < dotz;ii++) output<< "#";
		// remaining part (spaces)
		for ( ; ii < PROGRESSBARWIDTH;ii++) output<< " ";
		static const char* rotation_string = "|\\-/";
		myProgressBarRotation %= 4;
		output << "] " << rotation_string[myProgressBarRotation]
					 << " " << (int)(fraction*100)<<"/100\r";
		output.flush();
	}

	struct Background {
		virtual Color backgroundColor( const Ray& ray ) = 0;
	};
	Color blue(0.,0.,1.);
	Color white(1.,1.,1.);
	Color black(0.,0.,0.);
	Color vertDEau(0.729, 0.886, 0.784);
	Color corail(0.992, 0.616, 0.529);

	struct MyBackground : public Background {
		Color backgroundColor( const Ray& ray )
		{
			auto z = ray.direction[2];
			if (z < 0) {
				Color result;
				Real x = -0.5f * ray.direction[ 0 ] / ray.direction[ 2 ];
				Real y = -0.5f * ray.direction[ 1 ] / ray.direction[ 2 ];
				Real d = sqrt( x*x + y*y );
				Real t = std::min( d, 30.0f ) / 30.0f;
				x -= floor( x );
				y -= floor( y );
				if ( ( ( x >= 0.5f ) && ( y >= 0.5f ) ) || ( ( x < 0.5f ) && ( y < 0.5f ) ) )
					result += (1.0f - t)*vertDEau + t * Color( 1.0f, 1.0f, 1.0f );
				else
					result += (1.0f - t)*corail + t * Color( 1.0f, 1.0f, 1.0f );
				return result;
			}
			return (1-z) * white + z * blue;
		}
	};

	/// This structure takes care of rendering a scene.
	struct Renderer {

		/// The scene to render
		Scene* ptrScene;
		/// The origin of the camera in space.
		Point3 myOrigin;
		/// (myOrigin, myOrigin+myDirUL) forms a ray going through the upper-left
		/// corner pixel of the viewport, i.e. pixel (0,0)
		Vector3 myDirUL;
		/// (myOrigin, myOrigin+myDirUR) forms a ray going through the upper-right
		/// corner pixel of the viewport, i.e. pixel (width,0)
		Vector3 myDirUR;
		/// (myOrigin, myOrigin+myDirLL) forms a ray going through the lower-left
		/// corner pixel of the viewport, i.e. pixel (0,height)
		Vector3 myDirLL;
		/// (myOrigin, myOrigin+myDirLR) forms a ray going through the lower-right
		/// corner pixel of the viewport, i.e. pixel (width,height)
		Vector3 myDirLR;
		// On rajoute un pointeur vers un objet Background
		Background* ptrBackground;

		int myWidth;
		int myHeight;

		Renderer() : ptrScene( 0 ) {}
		Renderer( Scene& scene ) : ptrScene( &scene ) {}
		void setScene( rt::Scene& aScene ) { ptrScene = &aScene; }

		void setViewBox( Point3 origin,
										 Vector3 dirUL, Vector3 dirUR, Vector3 dirLL, Vector3 dirLR )
		{
			myOrigin = origin;
			myDirUL = dirUL;
			myDirUR = dirUR;
			myDirLL = dirLL;
			myDirLR = dirLR;
		}

		void setResolution( int width, int height )
		{
			myWidth	= width;
			myHeight = height;
		}

		// Affiche les sources de lumi??res avant d'appeler la fonction qui
		// donne la couleur de fond.
		Color background( const Ray& ray )
		{
			Color result = Color( 0.0, 0.0, 0.0 );
			for ( Light* light : ptrScene->myLights )
			{
				Real cos_a = light->direction( ray.origin ).dot( ray.direction );
				if ( cos_a > 0.99f )
				{
					Real a = acos( cos_a ) * 360.0 / M_PI / 8.0;
					a = std::max( 1.0f - a, 0.0f );
					result += light->color( ray.origin ) * a * a;
				}
			}
			if ( ptrBackground != 0 ) result += ptrBackground->backgroundColor( ray );
			return result;
		}


		/// The main rendering routine
		void render( Image2D<Color>& image, int max_depth )
		{
			std::cout << "Rendering into image ... might take a while." << std::endl;
			image = Image2D<Color>( myWidth, myHeight );
			for ( int y = 0; y < myHeight; ++y )
				{
					Real		ty	 = (Real) y / (Real)(myHeight-1);
					progressBar( std::cout, ty, 1.0 );
					Vector3 dirL = (1.0f - ty) * myDirUL + ty * myDirLL;
					Vector3 dirR = (1.0f - ty) * myDirUR + ty * myDirLR;
					dirL				/= dirL.norm();
					dirR				/= dirR.norm();
					for ( int x = 0; x < myWidth; ++x )
						{
							Real		tx	 = (Real) x / (Real)(myWidth-1);
							Vector3 dir	= (1.0f - tx) * dirL + tx * dirR;
							Ray eye_ray	= Ray( myOrigin, dir, max_depth );
							Color result = trace( eye_ray );
							image.at( x, y ) = result.clamp();
						}
				}
			std::cout << "Done." << std::endl;
		}

		void randomRender( Image2D<Color>& image, int max_depth, int nb_ray_by_pixel, int start_considering_check_mean, Real delta_min_mean )
		{
			std::cout << "Rendering into image ... might take a while." << std::endl;
			image = Image2D<Color>( myWidth, myHeight );
			Real deltaMaxX = 1.f / (Real)(myWidth-1);
			for ( int y = 0; y < myHeight; ++y )
			{
				Real		ty	 = (Real) y / (Real)(myHeight-1);
				progressBar( std::cout, ty, 1.0 );
				Vector3 dirL = (1.0f - ty) * myDirUL + ty * myDirLL;
				Vector3 dirR = (1.0f - ty) * myDirUR + ty * myDirLR;
				dirL				/= dirL.norm();
				dirR				/= dirR.norm();
				for ( int x = 0; x < myWidth; ++x )
				{
					Color result;
					Real		tx	 = (Real) x / (Real)(myWidth-1);
					int i = 0;
					while (i < nb_ray_by_pixel) {
						Real var = randomBetween(0.f, deltaMaxX);
						Vector3 dir	= (1.0f - tx - var) * dirL + (tx + var) * dirR;
						Ray eye_ray	= Ray( myOrigin, dir, max_depth );
						Color tmp = trace( eye_ray );
						if (i > start_considering_check_mean) {
							if (distance(tmp,result * (1.f/(float)i)) < delta_min_mean) break;
						}
						result += tmp;
						i++;
					}
					result = result * (1.f/(float)i);
					image.at( x, y ) = result.clamp();
				}
			}
			std::cout << "Done." << std::endl;
		}

		/// Calcule la couleur de la lumi??re (donn??e par light_color) dans la
		/// direction donn??e par le rayon. Si aucun objet n'est travers??,
		/// retourne light_color, sinon si un des objets travers??s est opaque,
		/// retourne du noir, et enfin si les objets travers??s sont
		/// transparents, attenue la couleur.
		Color shadow( const Ray& ray, Color light_color ) {
			// Soit C <- couleur de lumi??re en entr??e.
			auto C = light_color;

			// le rayon par de p et va vers L (direction lumi??re)
			auto L = ray.direction;
			auto p = ray.origin;

			// tant que c'est pas noir
			while (C.max() > .003f) {
				//on d??place l??g??rement p vers L pour ??viter d'intersecter l'objet initial.
				p = p + .01f * L;

				Point3 p_i;
				GraphicalObject* obj_i = 0;
				Ray rp = Ray(p, L);
				Real ri = ptrScene->rayIntersection(rp, obj_i, p_i);
				if (ri > 0) break;
				// on r??cup??re le mat??riau m de l'objet au point p' d'intersection
				Material m = obj_i->getMaterial(p_i);
				// C est multipli?? par la couleur diffuse et le coefficient de refraction de m
				C = C * m.diffuse * m.coef_refraction;
				//p <- p'
				p = p_i;
			}
			return C;
		}

		Ray refractionRay( const Ray& aRay, const Point3& p, Vector3 N, const Material& m ) {
			auto V = aRay.direction;
			// C > 0 -> ext??rieur
			// C < 0 -> int??rieur
			auto c = - V.dot(N);
			Real r = c > 0 ? m.out_refractive_index / m.in_refractive_index : m.in_refractive_index / m.out_refractive_index;
			auto dir = (1. - r*r*(1.-c*c)) > 0 ? r * V + (r*c + Real((c > 0 ? -1. : 1.) * sqrt(1. - r*r*(1.-c*c)))) * N : reflect(V, N);
			return Ray(p + dir * .01f, dir, aRay.depth - 1);
		}


		/// The rendering routine for one ray.
		/// @return the color for the given ray.
		Color trace( const Ray& ray )
		{
			assert( ptrScene != 0 );
			Color C = Color( 0.0, 0.0, 0.0 );
			GraphicalObject* obj_i = 0; // pointer to intersected object
			Point3					 p_i;			 // point of intersection

			// Look for intersection in this direction.
			Real ri = ptrScene->rayIntersection( ray, obj_i, p_i );
			// Nothing was intersected
			if ( ri >= 0.0f ) return background(ray); // some background color
			//return Color( 1.0, 1.0, 1.0 );
			// calculer la somme des couleurs diffuses et ambientes

			// return obj_i->getMaterial(p_i).diffuse + obj_i->getMaterial(p_i).ambient;
			auto m = obj_i->getMaterial(p_i);
			if (ray.depth > 0 && m.coef_reflexion != 0) {
				auto rd = reflect(ray.direction, obj_i->getNormal(p_i));
				Ray ray_refl(ray.origin + rd * .01f, rd, ray.depth - 1);
				auto C_refl = trace(ray_refl);
				C = C + C_refl * m.specular * m.coef_reflexion;
			}

			if (ray.depth > 0 && m.coef_refraction != 0) {
				Ray ray_refr = refractionRay(ray, p_i, obj_i->getNormal(p_i), m);
				auto C_refr = trace(ray_refr);
				C = C + C_refr * m.diffuse * m.coef_refraction;
			}

			C = C + illumination(ray, obj_i, p_i) * (ray.depth > 0 ? m.coef_diffusion : 1);
			return C;
		}

		/// Calcule le vecteur r??fl??chi ?? W selon la normale N.
		Vector3 reflect( const Vector3& V, Vector3 N ) const {
			return V - 2 * N.dot(V) * N;
		}

		/// Calcule l'illumination de l'objet \a obj au point \a p, sachant que l'observateur est le rayon \a ray.
		Color illumination( const Ray& ray, GraphicalObject* obj, Point3 p ) {
			auto m = obj->getMaterial(p);
			auto c = Color();
			Vector3 N = obj->getNormal(p);

			// V la direction du rayon ray venant de l'oeil
			auto v = ray.direction;

			// la direction miroir W de V par rapport ?? la normale N au point p
			Vector3 w = reflect(v, N);

			for (auto light : ptrScene->myLights) {
				// On r??cup??re sa direction L
				Vector3 L = light->direction(p);
				auto B = light->color(p);
				// On calcule le coefficient de diffusion kd comme le cosinus de l'angle entre L et la normale N au point p
				Real kd = L.dot(N);
				// On ajoute ?? C la couleur produit entre la couleur de la lumi??re B, la couleur diffuse du mat??riau D et son coefficient de diffusion
				if (kd > 0) c = c + kd * m.diffuse * light->color(p);

				// on calcule le cosinus ?? de l'angle entre W et la direction L de la lumi??re l
				auto beta = L.dot(w) / (w.norm() * L.norm());

				//si il est n??gatif il n'y a pas de sp??cularit??.
				if (beta > 0) {
					//on utilise la brillance s du mat??riau (shinyness) pour r??gler la taille de la tache
					//le coefficient de sp??cularit?? ks?????s (utilisez pow)
					Real ks = pow(beta, m.shinyness);

					//ajoute ?? C la couleur produit entre couleur de la lumi??re, la couleur sp??culaire et son coefficient de sp??cularit?? ks.
					c = c + ks * m.specular;
				}

				Color sc = shadow(Ray(p, L), B);
				c = c * sc;
			}
			// On ajoute ?? C la couleur ambiente et on retourne le r??sultat.
			return c + m.ambient;
		}

	};

} // namespace rt

#endif // #define _RENDERER_H_
