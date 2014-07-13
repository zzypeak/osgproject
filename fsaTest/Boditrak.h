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
	
	//��ʼ��
	bool Initialize();
	
	//
	bool Uninitialize();

	//����ӿ�
	void setStart();					//���Խ�������
	void setEnd();					 //ֹͣ����
	bool updateBoditrak();	//��������
	
	//����ѹ��ͼ������ת�ƹ켣
	osg::ref_ptr<osg::Geometry> createTonogram();
	osg::ref_ptr<osg::Geometry> createMasspoint();
	//
	osg::ref_ptr<osg::Node> getBoditrakNode();

	//��ѯ����״̬����������״̬
	void getStatus( bool& _connected, bool& recvFlag );
	
	//���÷���
	bool  getValues( std::vector<float>& values );	//��ȡʵʱѹ�����󣬲�����
    int     getColumns() const { return _columns;}	//����ѹ������
    int     getRows() const { return _rows;}				//����ѹ������
	float  getHeight() const { return _height;}			//���ظ�Ӧ����ĸ߶ȣ���λ����
    float  getWidth() const { return _width;}				//���ظ�Ӧ����Ŀ�ȣ���λ����


	bool  getValuesUncalibrate( std::vector<float>& values );	//	
    int     getTime() const { return _time;}					//����У׼���ݴ洢������
	char* getUnits()  { return _units;}							//����ѹ����λ���̶�(%)���ߵ絼ϵ��(uSiemens)
    float  getMax() const { return _maximum;}				//�������ѹ��
    float  getMin() const { return _minimum;}				//������Сѹ��
    int     getArea() const { return _height*_width;}		//���ظ�Ӧ������������λƽ������

private:
    FSAT7 _t7;				//boditrak�豸
	bool _connected;		//�豸�Ƿ�����
	bool _recvFlag;		//�����Ƿ��������
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