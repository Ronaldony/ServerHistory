#include <stdio.h>
#include <iostream>
#include <map>

using namespace std;

class Observer
{
public:
	virtual void update(int, int) = 0;
	string _szName;
};

class Subject
{
public:
	virtual void RegisterObserver(Observer*) = 0;	// 옵저버 등록
	virtual void RemoveObserver(Observer*) = 0;		// 옵저버 지우기
	virtual void Notify(void) = 0;					// 옵저버에게 데이터 갱신 알림
};

// 서브젝트
class WeatherData : public Subject
{
private:
	void Notify(void)						// 옵저버에게 데이터 갱신 알림
	{
		STL_MAP_OBSERVERS_ITR iter = _map_Observers.begin();

		if (iter != _map_Observers.end())
		{
			Observer* ob = iter->second;
			ob->update(_data1, _data2);
			iter++;
		}
	}
public:
	void RegisterObserver(Observer* ob)		// 옵저버 등록
	{
		_map_Observers.insert(STL_MAP_OBSERVERS_VT(ob->_szName, ob));
	}
	void RemoveObserver(Observer* ob)		// 옵저버 지우기
	{
		_map_Observers.erase(ob->_szName);
	}
	void SetData(int data1, int data2)		// 데이터 갱신
	{
		_data1 = data1;
		_data2 = data2;
		Notify();
	}
private:
	int _data1;
	int _data2;

	typedef map<string, Observer*>			STL_MAP_OBSERVERS;		// Observer를 등록할 Map STL
	typedef STL_MAP_OBSERVERS::iterator		STL_MAP_OBSERVERS_ITR;	// Map iterator
	typedef STL_MAP_OBSERVERS::value_type	STL_MAP_OBSERVERS_VT;	// Map value type
	STL_MAP_OBSERVERS						_map_Observers;			// Map 객체
};

// 데이터 사용 클래스(Observer 자식 클래스)
class DisplayCondition : public Observer
{
private:
	int _data1;
	int _data2;
public:
	void update(int data1, int data2)	// Subject로부터 호출되어 데이터 갱신
	{
		_data1 = data1;
		_data2 = data2;
	}
	void Display()						// Observer 출력 동작
	{
		printf("Data1: %d, Data2: %d\n", _data1, _data2);
	}
};

int main()
{
	WeatherData sub;
	DisplayCondition ob;

	sub.RegisterObserver(&ob);	// Display Observer 등록
	sub.SetData(1, 2);			// subject 데이터 갱신	
	ob.Display();				// Observer 출력
}
