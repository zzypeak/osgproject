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
#include <fstream>

osg::ref_ptr<osg::Geometry> createGeometry();

class BodiCallback: public osg::NodeCallback
{
public:
	BodiCallback(osg::Image* image)
	{
		_image = image;
	}
	
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{ 
		osg::Geode* geode = dynamic_cast<osg::Geode*>(node);

		if(geode)
		{
			_image->dirty();
			//traverse(node,nv);   
		}
	}

private:
	osg::Image* _image;
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

    //geode->addDrawable( quad );    
    //geode->setUpdateCallback( new BodiCallback(image));
    osg::ref_ptr<osg::Geometry> geom = createGeometry();

    //设置纹理
    //geom->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get() );
    ss->setTextureAttributeAndModes( 0, texture.get() );

    //纹理函数
    osg::ref_ptr<osg::TexEnv> texe = new osg::TexEnv;
    texe->setMode(osg::TexEnv::BLEND);
    //geom->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texe );
      
    //设置混合
    osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
    blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //geom->getOrCreateStateSet()->setAttributeAndModes(blendFunc);
    //ss->setAttributeAndModes(blendFunc);
    //quad->getOrCreateStateSet()->setAttributeAndModes(blendFunc);

    geode->addDrawable( geom.get() );
    geode->setUpdateCallback( new BoditrakCallback(geom , a ,value));

	osgViewer::Viewer viewer;
	viewer.setSceneData( geode );
    viewer.setCameraManipulator( new osgGA::TrackballManipulator );
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) ); 
    viewer.setUpViewInWindow( 100, 100, 640, 480 );
    


#if 0
    int count = 0;
    for(int r=0;r<row;r++)
    {
        for(int c=0;c<col;c++,value++)
        {
            //float step = 15*c/455.0+15*24*r/710.0;
            //value += (int)step;           
            //*p = *(value+(int)step);
            //*(p+1) = *(value+(int)step);
            //*(p+2) = *(value+(int)step);
            //p += 2;
            printf("%d ",int(*value));
        }
        puts("");
    }
#endif

    //image->allocateImage(a->getWidth(),a.getHeight(),1,GL_RGB,GL_UNSIGNED_BYTE);
    //unsigned char* data = (unsigned char*)image->getDataPointer();
    //image->setImage(a.getWidth(),a.getHeight(),1,GL_RGB,
    //    GL_RGB,GL_UNSIGNED_BYTE,/*(unsigned char*)*/data,osg::Image::AllocationMode::NO_DELETE );
 

    while(!viewer.done())
    {
        value = a->getValues();
#if 0        
        
        printf("--[%o]\n",value);
        osg::Vec4Array* colors = static_cast<osg::Vec4Array*>( geom->getColorArray() );
        // printf("[%O]-[%O]\n",pv,_value);
        unsigned char* p = image->data();
        for(int r=0;r<row;r++)
        {
            for(int c=0;c<col;c++,value++)
            {
                *p = 255 - int(*value);
                *(p+1) = 255 - int(*value);
                *(p+2) = 255 - int(*value);
                p += int(455/15)*3;
                //printf("[%d]%d ",count++,(int)step);
            }
            //p += int(710/24)*455*3 ;
            p = image->data();
            p += (710/24)*455*3*(r+1);
            //puts("");
        }
#endif
        viewer.frame();
    }

    return 0;
}

osg::ref_ptr<osg::Geometry> createGeometry()
{
    //Construct the vertices
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    //
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array(25*16);

#if 1 
    for(int i=0; i<25; i++ )
    {
        for(int j=0; j<16; j++ )
        {
            vertices->push_back(osg::Vec3( (float)j , 0.0f , (float)i));
            (*texcoords)[i*16 + j].set( (float)i/(float)25,(float)j/(float)16 );
        }
    }
#endif    

#if 0
    (*vertices)[0].set( 0.0f, 0.0f, 0.0f );
    (*vertices)[1].set( 3.0f, 0.0f, 0.0f );
    (*vertices)[2].set( 3.0f, 0.0f, 3.0f );
    (*vertices)[3].set( 0.0f, 0.0f, 3.0f );
    (*vertices)[4].set( 1.0f, 0.0f, 1.0f );
    (*vertices)[5].set( 2.0f, 0.0f, 1.0f );
    (*vertices)[6].set( 2.0f, 0.0f, 2.0f );
    (*vertices)[7].set( 1.0f, 0.0f, 2.0f );
    vertices->push_back(osg::Vec3(-0.2,-0.5,3.5));
    vertices->push_back(osg::Vec3(6.2,-0.5,3.5));
    vertices->push_back(osg::Vec3(0.8,2.0,6.0));
    vertices->push_back(osg::Vec3(5.2,2.0,6.0));
    vertices->push_back(osg::Vec3(-0.2,4.5,3.5));
    vertices->push_back(osg::Vec3(6.2,4.5,3.5));
#endif

       
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
            colors->push_back(osg::Vec4(.0,.0,.0,0.0));

    // Construct the polygon geometry
    osg::ref_ptr<osg::Geometry> polygon = new osg::Geometry;
    polygon->setVertexArray( vertices.get() );
    polygon->setNormalArray( normals.get() );
    polygon->setColorArray( colors.get() );
    polygon->setTexCoordArray( 0, texcoords.get() );
 //   polygon->setTexCoordIndices(0, );
    polygon->setNormalBinding( osg::Geometry::BIND_OVERALL );
    polygon->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
   // polygon->addPrimitiveSet( new osg::DrawArrays(osg::DrawArrays::QUADS, 0, 4) );
    polygon->addPrimitiveSet( mesh.get() );

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