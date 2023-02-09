# Ceci est un fichier de configuration pour une application Qt
# Il faut peut-etre legerement l adapter pour votre ordinateur.

# nom de votre executable
TARGET	= ray-tracer
# config de l executable
CONFIG *= qt opengl release
CONFIG += c++11
# config de Qt
QT		 *= opengl xml
QMAKE_CXXFLAGS += -std=c++11
# Qt6
# QT		 += openglwidgets

# Noms de vos fichiers entete
HEADERS = Viewer.h PointVector.h Color.h Sphere.h GraphicalObject.h Light.h \
					Material.h PointLight.h Image2D.h Image2DWriter.h Renderer.h Ray.h

# Noms de vos fichiers source
SOURCES = Viewer.cpp ray-tracer.cpp Sphere.cpp 

###########################################################
# Commentez/decommentez selon votre config/systeme
# (Une config windows est possible)
###########################################################

# Exemple de configuration Linux Ubuntu 20.04, Qt5 et libQGLViewer
LIBS *= -L/usr/lib/x86_64-linux-gnu -lQGLViewer-qt5

# Exemple de configuration MacOSX avec macports
## INCLUDEPATH *= /opt/local/include
## LIBS *= -L/opt/local/lib -lQGLViewer 

# Exemple de configuration MacOSX avec frameworks
# INCLUDEPATH *= /Users/lachaud/local/src/libQGLViewer-2.9.1
# LIBS *= -F/Users/lachaud/local/src/libQGLViewer-2.9.1/QGLViewer -framework QGLViewer

# Exemple de configuration Windows :
# LIBS *= -lopengl32 -lglu32
# INCLUDEPATH *= D:\Cours\Info805\TP1\libQGLViewer-2.7.1
# LIBS *= -LD:\Cours\Info805\TP1\libQGLViewer-2.7.1\QGLViewer -lQGLViewer2

