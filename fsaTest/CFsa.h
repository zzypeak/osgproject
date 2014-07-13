#include "fsa.h"
#include <iostream>
#include <vector>

#define _AFXDLL
#include <afx.h>
#pragma comment(lib,"setupapi")

extern "C" 
{
#include "setupapi.h"
}


class CFsa
{
public:
	CFsa();
	~CFsa();
		
    static CFsa* getInstance();

    int getColumns() const { return _columns;}	//返回列数
    int getRows() const { return _rows;}		//返回行数
    float getMax() const { return _maximum;}		//返回最大压力
    float getMin() const { return _minimum;}		//返回最小压力
    char* getUnits()  { return _units;}	//返回压力单位，刻度(%)或者电导系数(uSiemens)
	
    float* getValues();	//获取实时压力矩阵，并返回
	
    float* getValuesUncalibrate();	//
	
    int getTime() const { return _time;}		//返回校准数据存储的日期
	
    float getHeight() const { return _height;}	//返回感应区域的高度，单位毫米
    float getWidth() const { return _width;}		//返回感应区域的宽度，单位毫米
    int getArea() const { return _height*_width;}		//返回感应区域的面积，单位平方毫米

	void setAll();		//
	void setColumns();	//计算列数
	void setRows();		//计算行数
	
    int Ondo();

private:

    FSAT7 _t7; //
	int _columns;
	int _rows;
	int _time;
	float _height;
	float _width;
	float _minimum, _maximum;
    char _units[32];
	float* _values;
    std::vector<float> _bodivalues;
    static CFsa* _fsaInstance;
};

