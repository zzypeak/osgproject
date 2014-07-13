#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "CFsa.h" 

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

osg::ref_ptr<osg::Geometry> createGeometry();
osg::ref_ptr<osg::Geometry> createMasspoint();

class MasscenterCallback: public osg::NodeCallback
{
public:
	MasscenterCallback(osg::Geometry* geom):
	  _angle(0.0)
	{
		_geom = geom;
	}
	
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{ 
		osg::Geode* geode = dynamic_cast<osg::Geode*>(node);

		if(geode)
		{
			osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>( _geom->getVertexArray() );
			double time = nv->getFrameStamp()->getReferenceTime();

			//(*vertices)[0].set( 1.0f+cosf(time*15.0f), -0.1f,  0.0f  );
			if( time < 20.0)
				_angle +=1.0/10.0;

			(*vertices)[0].set( 10.0f*cos(_angle), -0.1, 10.0*sin(_angle) );
			for( int i=10; i > 0; i--)
			{
				(*vertices)[i] = (*vertices)[i-1];
			}

			vertices->dirty();
		}			
		traverse(node,nv);   
	}

private:
	osg::Geometry* _geom;
	float _angle;
};

class BoditrakCallback: public osg::NodeCallback
{
public:
    BoditrakCallback(osg::Geometry* geom,CFsa* a , float* value)
	{
		_geom = geom;
        _a = a;
        _value = value;
	}
	
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{ 
		osg::Geode* geode = dynamic_cast<osg::Geode*>(node);

		if(geode)
		{
            osg::Vec4Array* colors = static_cast<osg::Vec4Array*>( _geom->getColorArray() );
            float* pv = _value;//_a->getValues();
            if(pv)
            {
                colors->clear();
                for(int i = 0; i<_a->getRows(); i++)
                {
                    for(int j = 0; j< _a->getColumns(); j++,pv++)
                    {
                        if( int(*pv) < 10 )
                            colors->push_back(osg::Vec4(1.0,1.0,1.0,0.0));
                        else if( int(*pv) < 30)
                            colors->push_back(osg::Vec4(0.0,0.0,0.8,1.0));//blue
                        else if( int(*pv) < 40)
                            colors->push_back(osg::Vec4(0.0,0.5,0.0,1.0));//green
                        else if( int(*pv) < 50)
                            colors->push_back(osg::Vec4(0.0,1.0,0.0,1.0));//green
                        else if( int(*pv) < 90)
                            colors->push_back(osg::Vec4(0.5,0.5,0.0,1.0));//yellow
                        else if( int(*pv) < 100)
                            colors->push_back(osg::Vec4(0.5,0.0,0.0,1.0));//red
                        else 
                            colors->push_back(osg::Vec4(1.0,0.0,0.0,1.0));//red
                    }
                }
                colors->dirty();
              }
		}
	}

private:
	osg::Geometry* _geom;
    CFsa* _a;
    float* _value;
};



int main() 
{
	osg::ref_ptr<osg::Group> root = new osg::Group;
		
    osg::ref_ptr<osg::Image> image = new osg::Image();
    
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage( osgDB::readImageFile("boditrak.png") );
    texture->setResizeNonPowerOfTwoHint(false);

    osg::Drawable* quad = osg::createTexturedQuadGeometry(
        osg::Vec3(), osg::Vec3(15.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 24.0f) );
    quad->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get() );
    quad->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN );//不透明

    CFsa* a = new CFsa();
    float* value = a->getValues();
    int col = a->getColumns();
    int row = a->getRows();
    int heit = a->getHeight();
    int width = a->getWidth();
    //unsigned char* data = new unsigned char[heit*width*3];
    //unsigned char* p = data;
    //memset(data,255,heit*width*3);
    if(!value)
    {
        std::cout<<"can not connect to boditrak.\n"<<std::endl;    
    }

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    osg::StateSet* ss = geode->getOrCreateStateSet();

    osg::ref_ptr<osg::Geometry> geom = createGeometry();
	osg::ref_ptr<osg::Geometry> geompoint = createMasspoint();

    //设置纹理
    geom->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get() );
    //ss->setTextureAttributeAndModes( 0, texture.get() );

    geode->addDrawable( geom.get() );
	geode->addDrawable( geompoint.get() );
    //geode->setUpdateCallback( new BoditrakCallback(geom , a ,value));
    geode->setUpdateCallback( new MasscenterCallback( geompoint ));

	osgViewer::Viewer viewer;
	viewer.setSceneData( geode );
    viewer.setCameraManipulator( new osgGA::TrackballManipulator );
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) ); 
    viewer.setUpViewInWindow( 100, 100, 640, 480 );

    while(!viewer.done())
    {
        value = a->getValues();

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