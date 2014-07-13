#include "Boditrak.h"
#include <osg/Point>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>

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
			for( int i=  vertices->size(); i > 0; i--)
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
    BoditrakCallback(osg::Geometry* geom, float* value, Boditrak* bk)
	{
		_geom = geom;
        _value = value;
		_bk = bk;
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
                for(int i = 0; i<_bk->getRows(); i++)
                {
                    for(int j = 0; j< _bk->getColumns(); j++,pv++)
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
	float* _value;
	Boditrak* _bk;
};

Boditrak::Boditrak():
_rows(0),
	_columns(0),
	_height(0.0),
	_width(0.0),
	_values(NULL),
	_minimum(0),
	_maximum(0),	
	_recvFlag(false),
	_connected(false)
{
	memset(_units,0x00,sizeof(char)*32);
}

Boditrak::~Boditrak()
{
	if(!_values){
		delete []_values;
	}
	if(_t7){
		fsat7Destroy(_t7);
	}

	fsat7Uninitialize();
}

bool	Boditrak::Initialize()
{
	_connected = false;
	fsat7Initialize();
	_t7 = fsat7Create(0, 0);

	if( !_t7 ) //连接失败
	{
		_connected = false;
		return _connected;
	}

	_connected = true;

	_rows = fsat7GetRowCount(_t7);
	_columns = fsat7GetColumnCount(_t7);
	_height = fsat7GetHeight(_t7);
	_width = fsat7GetWidth(_t7);
	_time = fsat7GetCalibrationTime(_t7);

	//分配数据空间
	_values = new float[_columns * _rows];
	if( _values){
		return false;
	}

	if (fsat7LoadCalibration(_t7)) {
		_minimum = fsat7GetMinimum(_t7);
		_maximum = fsat7GetMaximum(_t7);
		fsat7GetUnits(_t7, _units);
	}
	else {
		_minimum = 0;
		_maximum = fsat7GetConductance(_t7);//获取最大导电系数
		strcpy(_units, "uSiemens");
	}

	return _connected;	
}

bool Boditrak::Uninitialize()
{
	if(!_values){
		delete []_values;
	}

	if(_t7){
		fsat7Destroy(_t7);
	}

	fsat7Uninitialize();

	return true;
}

void Boditrak::setStart()
{
	_recvFlag = true;
}

void Boditrak::setEnd()
{
	_recvFlag = false;
}

bool Boditrak::updateBoditrak()
{
	if(_recvFlag)
	{
		if(!fsat7LoadCalibration(_t7)) {
			return false;
		}

		if (!fsat7Scan(_t7, _values, NULL))	{
			return false;
		}
	}
}

bool Boditrak::getValues( std::vector<float>& values )
{
	float *p = _values;
	for(int i=0; i < _rows; i++)
		for( int j=0; j< _columns;j++,p++)
			values.push_back( *p );

	return true;
}

bool Boditrak::getValuesUncalibrate( std::vector<float>& unvalues)
{
	if(!fsat7UnloadCalibration(_t7)){return false;}

	if (!fsat7Scan(_t7, _values, NULL))	
	{ return false;}
	
	float *p = _values;
	for(int i=0; i < _rows; i++)
		for( int j=0; j< _columns;j++,p++)
			unvalues.push_back( *p );
	
	return true;
}

osg::ref_ptr<osg::Geometry> Boditrak::createTonogram()
{
	 //Construct the vertices
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array( _rows*_columns );


	//建立顶点和纹理坐标
    for(int i=0; i < _rows; i++ )
    {
        for(int j=0; j < _columns; j++ )
        {
            vertices->push_back(osg::Vec3( (float)j , 0.0f , (float)i));
            (*texcoords)[i*_columns + j].set( (float)i/(float)_rows,(float)j/(float)_columns );
        }
    }
       
    osg::ref_ptr<osg::DrawElementsUInt> mesh = new osg::DrawElementsUInt(osg::DrawElementsUInt::QUADS);
	//建立顶点索引
    for(int i=0; i< (_rows-1); i++ )
    {
        for(int j=0; j< ( _columns-1); j++ )
        {
            mesh->push_back(i*_columns+j);
            mesh->push_back(i*_columns+j+1);
            mesh->push_back(i*_columns+j+_columns+1);
            mesh->push_back(i*_columns+j+_columns);
        }
    }

    // The normal array
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(1);
    (*normals)[0].set( 0.0f,-1.0f, 0.0f );

    // Construct the borderlines geometry
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    for(int i = 0; i<25 ; i++){
        for(int j = 0; j<16;j++){
            colors->push_back(osg::Vec4(1.0,1.0,1.0,0.0));
		}
	}

    // Construct the polygon geometry
    _presuregeom->setVertexArray( vertices.get() );
    _presuregeom->setNormalArray( normals.get() );
    _presuregeom->setColorArray( colors.get() );
    _presuregeom->setTexCoordArray( 0, texcoords.get() );
    _presuregeom->setNormalBinding( osg::Geometry::BIND_OVERALL );
    _presuregeom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
   // polygon->addPrimitiveSet( new osg::DrawArrays(osg::DrawArrays::QUADS, 0, 4) );
    _presuregeom->addPrimitiveSet( mesh.get() );

	_presuregeom->setUseDisplayList(false);
    _presuregeom->setUseVertexBufferObjects(true);

    return _presuregeom;
}

osg::ref_ptr<osg::Geometry> Boditrak::createMasspoint()
{
	//Construct the vertices
	const int numPoints = 10;	//	轨迹长度
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(numPoints);
	for(int i=0 ; i<numPoints ; i++)
	{
		(*vertices)[i].set( 0.0,-0.1,0.0 );
	}

    // Construct the borderlines geometry
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.0,1.0,1.0,1.0);

    // Construct the polygon geometry
    _pointgeom->setVertexArray( vertices.get() );
    _pointgeom->setColorArray( colors.get() );
	_pointgeom->setColorBinding( osg::Geometry::BIND_OVERALL );
	_pointgeom->addPrimitiveSet( new osg::DrawArrays(osg::DrawArrays::POINTS, 0, 1) );
	_pointgeom->addPrimitiveSet( new osg::DrawArrays(osg::DrawArrays::LINE_STRIP,0, numPoints) );

	// Set point size
	osg::ref_ptr<osg::StateSet> ss = _pointgeom->getOrCreateStateSet();
	ss->setAttribute( new osg::Point(10.0),osg::StateAttribute::ON );
	
	// Set point smooth
	ss->setMode( GL_POINT_SMOOTH ,osg::StateAttribute::ON);

	_pointgeom->setUseDisplayList(false);
	_pointgeom->setUseVertexBufferObjects(true);

	return _pointgeom;
}

osg::ref_ptr<osg::Node> Boditrak::getBoditrakNode()
{
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	//mt->setMatrix( osg::Matrix::rotate( osg::) )

	//设置压力图纹理贴图
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage( osgDB::readImageFile("boditrak.png") );
	texture->setResizeNonPowerOfTwoHint(false);
	_presuregeom->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get() );

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable( _presuregeom.get() );
	geode->addDrawable( _pointgeom.get() );
	geode->setUpdateCallback( new BoditrakCallback( _presuregeom , _values, this) );
	geode->setUpdateCallback( new MasscenterCallback( _pointgeom ) );

	mt->addChild(geode);
	return mt;
}



