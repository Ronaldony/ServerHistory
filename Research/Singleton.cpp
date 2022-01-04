#include <stdio.h>
#include <iostream>

template <typename T>
class TemplateSingleton
{
protected:
    TemplateSingleton() {}
    virtual ~TemplateSingleton() {}
public:
    static T* GetInstance()             // 싱글톤 객체 반환
    {
        if (_singleton == nullptr)
            _singleton = new T;         // 싱글톤 객체 동적 생성(이렇게 해야 싱글톤 생성 시점을 정할 수 있다.)
        return _singleton;
    }
    static void DestroyInstance()       // 싱글톤 객체 파괴
    {
        if (_singleton != nullptr)
            delete _singleton;
    }
private:
    static T* _singleton;               // 싱글톤 객체
};

template <typename T>
T* TemplateSingleton<T>::_singleton = nullptr;

// 싱글톤 클래스를 <System> 타입으로 상속한 System 클래스 
class System : public TemplateSingleton<System>
{
public:
    System() :_data(5)
    {}
    ~System() 
    {}
    int getData()
    {
        return _data;
    }
private:
    int _data;
};

int main()
{
    System* p = System::GetInstance();      // 싱글톤 객체 생성
    cout << p->getData() << endl;	
    System::DestroyInstance();              // 싱글톤 객체 파괴
}
