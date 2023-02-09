/**
@file Sphere.cpp
*/
#include <cmath>
#include "Sphere.h"

void
rt::Sphere::draw( Viewer& /* viewer */ )
{
	Material m = material;
	// Taking care of south pole
	glBegin( GL_TRIANGLE_FAN );
	glColor4fv( m.ambient );
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m.specular);
	glMaterialf(GL_FRONT, GL_SHININESS, m.shinyness );
	Point3 south_pole = localize( -90, 0 );
	glNormal3fv( getNormal( south_pole ) );
	glVertex3fv( south_pole );
	for ( int x = 0; x <= NLON; ++x )
		{
			Point3 p = localize( -90 + 180/NLAT, x * 360 / NLON );
			glNormal3fv( getNormal( p ) );
			glVertex3fv( p );
		}
	glEnd();
	// Taking care of in-between poles
	for ( int y = 1; y < NLAT - 1; ++y )
		{
			glBegin( GL_QUAD_STRIP);
			glColor4fv( m.ambient );
			glMaterialfv(GL_FRONT, GL_DIFFUSE, m.diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, m.specular);
			glMaterialf(GL_FRONT, GL_SHININESS, m.shinyness );
			for ( int x = 0; x <= NLON; ++x )
				{
					Point3 p = localize( -90 + y*180/NLAT,		 x * 360 / NLON );
					Point3 q = localize( -90 + (y+1)*180/NLAT, x * 360 / NLON );
					glNormal3fv( getNormal( p ) );
					glVertex3fv( p );
					glNormal3fv( getNormal( q ) );
					glVertex3fv( q );
				}
			glEnd();
		}
	// Taking care of north pole
	glBegin( GL_TRIANGLE_FAN );
	glColor4fv( m.ambient );
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m.specular);
	glMaterialf(GL_FRONT, GL_SHININESS, m.shinyness );
	Point3 north_pole = localize( 90, 0 );
	glNormal3fv( getNormal( north_pole ) );
	glVertex3fv( north_pole );
	for ( int x = NLON; x >= 0; --x )
		{
			Point3 p = localize( -90 + (NLAT-1)*180/NLAT, x * 360 / NLON );
			glNormal3fv( getNormal( p ) );
			glVertex3fv( p );
		}
	glEnd();
}

rt::Point3
rt::Sphere::localize( Real latitude, Real longitude ) const
{
	static const Real conv_deg_rad = 2.0 * M_PI / 360.0;
	latitude	*= conv_deg_rad;
	longitude *= conv_deg_rad;
	return center
		+ radius * Point3( cos( longitude ) * cos( latitude ),
											 sin( longitude ) * cos( latitude ),
											 sin( latitude ) );
}

rt::Vector3
rt::Sphere::getNormal( Point3 p )
{
	Vector3 u = p - center;
	Real	 l2 = u.dot( u );
	if ( l2 != 0.0 ) u /= sqrt( l2 );
	return u;
}

rt::Material
rt::Sphere::getMaterial( Point3 /* p */ )
{
	return material; // the material is constant along the sphere.
}

rt::Real
rt::Sphere::rayIntersection( const Ray& ray, Point3& p )
{
	Vector3 distOC = ray.origin - center;
	rt::Real r2 = radius * radius;
	rt::Real b = 2 * distOC.dot(ray.direction);

	//delta=b^2-4AC
	rt::Real delta = b*b - 4*(distOC.dot(distOC) - r2);
	if (delta < 0) return -delta;

	//calcul de t1 et t2 avec b et delta
	rt::Real t1 = (-b - sqrt(delta)) / 2;
	rt::Real t2 = (-b + sqrt(delta)) / 2;

	//check si pas d'intersection
	if (t2 < 0) return distOC.norm() - radius;

	//calcul p en selectionnant le point le plus proche
	p = ray.origin + (t1 < 0 ? t2 : t1) * ray.direction;
	return -1;
}
