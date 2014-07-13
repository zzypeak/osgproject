#include "CFsa.h"

CFsa*	CFsa::_fsaInstance = NULL;

CFsa::CFsa()
{
	fsat7Initialize();
	_t7 = fsat7Create(0, 0);

	memset(_units,0x00,sizeof(char)*32);
	_values = NULL;
	_minimum = 0;
	_maximum = 0;

	setAll();
	Ondo();
	 /* switch(Ondo())
	  {
	  case -1: std::cout<<"Failed to connect to a sensor array."<<std::endl;
		  break;
	  case -2: std::cout<<"Failed to allocate value buffer."<<std::endl;
		  break;
  	  case -3: std::cout<<"Failed to scan sensor array."<<std::endl;
		  break;
	  default:
		  break;
	  }*/
}

CFsa::~CFsa()
{
	//free(_values);
	if(!_values)	delete []_values;

	if(_t7) fsat7Destroy(_t7);

	fsat7Uninitialize();
}

CFsa* CFsa::getInstance()
{
	if(_fsaInstance == NULL)
	{
		_fsaInstance = new CFsa();
	}
	return _fsaInstance;
}

int CFsa::Ondo()
{
	if(!_t7)
		{return -1;}

	//_values = (float*)malloc(_columns * _rows * sizeof(float));
	_values = new float[_columns * _rows];

	if(!_values)
		{return -2;}

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

	return 0;
}

void CFsa::setAll()
{
	if(_t7)
	{
		_rows = fsat7GetRowCount(_t7);
		_columns = fsat7GetColumnCount(_t7);
		_time = fsat7GetCalibrationTime(_t7);
		_height = fsat7GetHeight(_t7);
		_width = fsat7GetWidth(_t7);
	}
	else
	{
		_rows = 0;
		_columns = 0;
		_time = 0;
		_height = 0;
		_width = 0;
	}
}

float* CFsa::getValues()
{
	//从左至右，从上至下扫描；返回扫描到的传感器数目
	if(!_t7) return NULL;
	
	if(!fsat7LoadCalibration(_t7)) return NULL;

	if (!fsat7Scan(_t7, _values, NULL))	
		{ return NULL;}
    
	return _values;
}

float* CFsa::getValuesUncalibrate()
{
	if(!_t7) return NULL;

	if(!fsat7UnloadCalibration(_t7)){return NULL;}

	if (!fsat7Scan(_t7, _values, NULL))	
		{ return NULL;}

	return _values;

}

void CFsa::setColumns()
{
	_columns = fsat7GetColumnCount(_t7);
}

void CFsa::setRows()
{
	if(_t7)
		_rows = fsat7GetRowCount(_t7);
	else
		_rows = 0;
}