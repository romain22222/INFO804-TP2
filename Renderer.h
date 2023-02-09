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

/// Namespace RayTracer
namespace rt {

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

		// Affiche les sources de lumières avant d'appeler la fonction qui
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

		/// Calcule la couleur de la lumière (donnée par light_color) dans la
		/// direction donnée par le rayon. Si aucun objet n'est traversé,
		/// retourne light_color, sinon si un des objets traversés est opaque,
		/// retourne du noir, et enfin si les objets traversés sont
		/// transparents, attenue la couleur.
		Color shadow( const Ray& ray, Color light_color ) {
			// Soit C <- couleur de lumière en entrée.
			auto C = light_color;

			// le rayon par de p et va vers L (direction lumière)
			auto L = ray.direction;
			auto p = ray.origin;

			// tant que c'est pas noir
			while (C.max() > .003f) {
				//on déplace légèrement p vers L pour éviter d'intersecter l'objet initial.
				p = p + .0001f * L;

				Point3 p_i;
				GraphicalObject* obj_i = 0;
				Ray rp = Ray(p, L);
				auto ri = ptrScene->rayIntersection(rp, obj_i, p_i);
				if (ri >= 0) break;
				// on récupère le matériau m de l'objet au point p' d'intersection
				auto m = obj_i->getMaterial(p_i);
				// C est multiplié par la couleur diffuse et le coefficient de refraction de m
				C = C * m.diffuse * m.coef_refraction;
				//p <- p'
				p = p_i;
			}
			return C;
		}


		/// The rendering routine for one ray.
		/// @return the color for the given ray.
		Color trace( const Ray& ray )
		{
			assert( ptrScene != 0 );
			Color result = Color( 0.0, 0.0, 0.0 );
			GraphicalObject* obj_i = 0; // pointer to intersected object
			Point3					 p_i;			 // point of intersection

			// Look for intersection in this direction.
			Real ri = ptrScene->rayIntersection( ray, obj_i, p_i );
			// Nothing was intersected
			if ( ri >= 0.0f ) return background(ray); // some background color
			//return Color( 1.0, 1.0, 1.0 );
			// calculer la somme des couleurs diffuses et ambientes

			// return obj_i->getMaterial(p_i).diffuse + obj_i->getMaterial(p_i).ambient;

			return illumination(ray, obj_i, p_i);
		}

		/// Calcule le vecteur réfléchi à W selon la normale N.
		Vector3 reflect( const Vector3& V, Vector3 N ) const {
			return V - 2 * (N.dot(V)) * N;
		}

		/// Calcule l'illumination de l'objet \a obj au point \a p, sachant que l'observateur est le rayon \a ray.
		Color illumination( const Ray& ray, GraphicalObject* obj, Point3 p ) {
			auto m = obj->getMaterial(p);
			auto c = Color();

			// V la direction du rayon ray venant de l'oeil
			auto v = ray.direction;
			for (auto light : ptrScene->myLights) {
				// On récupère sa direction L
				Vector3 L = light->direction(p);
				// On calcule le coefficient de diffusion kd comme le cosinus de l'angle entre L et la normale N au point p
				Vector3 N = obj->getNormal(p);
				Real kd = L.dot(N);
				// (d = 0.0 si négatif)
				if (kd < 0) kd = 0;
				// On ajoute à C la couleur produit entre la couleur de la lumière B, la couleur diffuse du matériau D et son coefficient de diffusion
				c = c + kd * m.diffuse * light->color(p);

				// la direction miroir W de V par rapport à la normale N au point p
				Vector3 w = reflect(v, N);
				// on calcule le cosinus β de l'angle entre W et la direction L de la lumière l
				auto beta = L.dot(w);

				//si il est négatif il n'y a pas de spécularité.
				if (beta < 0) continue;

				//on utilise la brillance s du matériau (shinyness) pour régler la taille de la tache
				//le coefficient de spécularité ks←βs (utilisez pow)
				Real ks = pow(beta, m.shinyness);

				//ajoute à C la couleur produit entre couleur de la lumière, la couleur spéculaire et son coefficient de spécularité ks.
				c = c + ks * m.specular * light->color(p);


				Color sc = shadow(Ray(p, light->direction(p)), light->color(p));
				c = c * sc;
			}
			// On ajoute à C la couleur ambiente et on retourne le résultat.
			return c + m.ambient;
		}

	};

} // namespace rt

#endif // #define _RENDERER_H_