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

    int getColumns() const { return _columns;}	//��������
    int getRows() const { return _rows;}		//��������
    float getMax() const { return _maximum;}		//�������ѹ��
    float getMin() const { return _minimum;}		//������Сѹ��
    char* getUnits()  { return _units;}	//����ѹ����λ���̶�(%)���ߵ絼ϵ��(uSiemens)
	
    float* getValues();	//��ȡʵʱѹ�����󣬲�����
	
    float* getValuesUncalibrate();	//
	
    int getTime() const { return _time;}		//����У׼���ݴ洢������
	
    float getHeight() const { return _height;}	//���ظ�Ӧ����ĸ߶ȣ���λ����
    float getWidth() const { return _width;}		//���ظ�Ӧ����Ŀ�ȣ���λ����
    int getArea() const { return _height*_width;}		//���ظ�Ӧ������������λƽ������

	void setAll();		//
	void setColumns();	//��������
	void setRows();		//��������
	
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

