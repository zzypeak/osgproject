#ifndef BODITRAK_H
#define BODITRAK_H

#include "fsa.h"
#define _AFXDLL
#include <afx.h>
#pragma comment(lib,"setupapi")

extern "C" 
{
#include "setupapi.h"
}

#include <iostream>
#include <vector>
#include <osg/Referenced>
#include <osg/Node>
#include <osg/Geometry>

class Boditrak : public osg::Referenced
{
public:
	Boditrak();
	~Boditrak();
	
	//初始化
	bool Initialize();
	
	//
	bool Uninitialize();

	//对外接口
	void setStart();					//可以接收数据
	void setEnd();					 //停止接收
	bool updateBoditrak();	//接收数据
	
	//创建压感图和重心转移轨迹
	osg::ref_ptr<osg::Geometry> createTonogram();
	osg::ref_ptr<osg::Geometry> createMasspoint();
	//
	osg::ref_ptr<osg::Node> getBoditrakNode();

	//查询连接状态，接收数据状态
	void getStatus( bool& _connected, bool& recvFlag );
	
	//常用返回
	bool  getValues( std::vector<float>& values );	//获取实时压力矩阵，并返回
    int     getColumns() const { return _columns;}	//返回压感列数
    int     getRows() const { return _rows;}				//返回压感行数
	float  getHeight() const { return _height;}			//返回感应区域的高度，单位毫米
    float  getWidth() const { return _width;}				//返回感应区域的宽度，单位毫米


	bool  getValuesUncalibrate( std::vector<float>& values );	//	
    int     getTime() const { return _time;}					//返回校准数据存储的日期
	char* getUnits()  { return _units;}							//返回压力单位，刻度(%)或者电导系数(uSiemens)
    float  getMax() const { return _maximum;}				//返回最大压力
    float  getMin() const { return _minimum;}				//返回最小压力
    int     getArea() const { return _height*_width;}		//返回感应区域的面积，单位平方毫米

private:
    FSAT7 _t7;				//boditrak设备
	bool _connected;		//设备是否连接
	bool _recvFlag;		//设置是否接收数据
	int _columns;
	int _rows;
	float _height;
	float _width;	
	float* _values;
	int _time;
	float _minimum, _maximum;
    char _units[32];
    std::vector<float> _bodivalues;

    osg::ref_ptr<osg::Geometry> _pointgeom;
	osg::ref_ptr<osg::Geometry> _presuregeom;
};

#endif