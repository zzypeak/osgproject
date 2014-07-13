#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <osg/TextureRectangle>
#include <osg/Texture2D>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osg/Geode> 
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgAnimation\EaseMotion>
#include <osg/StateAttribute>
#include <osg/LineWidth>
#include <osg/Geometry>
#include <iostream>
#include <osgText/Text>
#include <osg/CameraNode>
#include <osg/Camera>
#include <osg/BlendFunc>
#include <osg/Point>
#include <fstream>
#include <osg/PointSprite>

#include "Boditrak.h"

int main() 
{
	Boditrak bk;
	bk.Initialize();
	bk.setStart();

	osg::ref_ptr<osg::Node> root = bk.getBoditrakNode();

	osgViewer::Viewer viewer;
	viewer.setSceneData( root );
    viewer.setCameraManipulator( new osgGA::TrackballManipulator );
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) ); 
    viewer.setUpViewInWindow( 100, 100, 640, 480 );

    while(!viewer.done())
    {

        viewer.frame();
    }

    return 0;
}

osg::ref_ptr<osg::Geometry> createMasspoint()
{
    //Construct the vertices
	const int numPoints = 10;
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(numPoints);
	for(int i=0 ; i<numPoints ; i++)
	{
		(*vertices)[i].set( 0.0,-0.1,0.0 );
	}

    // Construct the borderlines geometry
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.0,1.0,1.0,1.0);

    // Construct the polygon geometry
    osg::ref_ptr<osg::Geometry> pointgeo = new osg::Geometry;
    pointgeo->setVertexArray( vertices.get() );
    pointgeo->setColorArray( colors.get() );
	pointgeo->setColorBinding( osg::Geometry::BIND_OVERALL );
	pointgeo->addPrimitiveSet( new osg::DrawArrays(osg::DrawArrays::POINTS, 0, 1) );
	pointgeo->addPrimitiveSet( new osg::DrawArrays(osg::DrawArrays::LINE_STRIP,0, numPoints) );

	// Set point size
	osg::ref_ptr<osg::StateSet> ss = pointgeo->getOrCreateStateSet();
	ss->setAttribute( new osg::Point(10.0),osg::StateAttribute::ON );
	
	// Set point smooth
	ss->setMode( GL_POINT_SMOOTH ,osg::StateAttribute::ON);

	pointgeo->setUseDisplayList(false);
	pointgeo->setUseVertexBufferObjects(true);
    return pointgeo;
}

osg::ref_ptr<osg::Geometry> createGeometry()
{
    //Construct the vertices
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    //
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array(25*16);

    for(int i=0; i<25; i++ )
    {
        for(int j=0; j<16; j++ )
        {
            vertices->push_back(osg::Vec3( (float)j , 0.0f , (float)i));
            (*texcoords)[i*16 + j].set( (float)i/(float)25,(float)j/(float)16 );
        }
    }
       
    osg::ref_ptr<osg::DrawElementsUInt> mesh = new osg::DrawElementsUInt(osg::DrawElementsUInt::QUADS);

    for(int i=0; i<24; i++ )
    {
        for(int j=0; j<15; j++ )
        {
            mesh->push_back(i*16+j);
            mesh->push_back(i*16+j+1);
            mesh->push_back(i*16+j+17);
            mesh->push_back(i*16+j+16);
        }
    }

    // The normal array
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(1);
    (*normals)[0].set( 0.0f,-1.0f, 0.0f );

    // Construct the borderlines geometry
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    for(int i = 0; i<25 ; i++)
        for(int j = 0; j<16;j++)
            colors->push_back(osg::Vec4(1.0,1.0,1.0,0.0));

    // Construct the polygon geometry
    osg::ref_ptr<osg::Geometry> polygon = new osg::Geometry;
    polygon->setVertexArray( vertices.get() );
    polygon->setNormalArray( normals.get() );
    polygon->setColorArray( colors.get() );
    polygon->setTexCoordArray( 0, texcoords.get() );
    polygon->setNormalBinding( osg::Geometry::BIND_OVERALL );
    polygon->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
   // polygon->addPrimitiveSet( new osg::DrawArrays(osg::DrawArrays::QUADS, 0, 4) );
    polygon->addPrimitiveSet( mesh.get() );

	polygon->setUseDisplayList(false);
    polygon->setUseVertexBufferObjects(true);

    return polygon;
}

void createTexture(osg::StateSet& ss)
{
    osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
    //tex->setImage();
    tex->setImage(osgDB::readImageFile("boditrak.png"));
    tex->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    tex->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    tex->setWrap( osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_BORDER);
    tex->setWrap( osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_BORDER);
    tex->setBorderColor(osg::Vec4(1.0,1.0,0.0,1.0));
    ss.setTextureAttributeAndModes(0,tex.get());
}