#include <iostream>
#include <thread>
#include <mutex>
#define THREAD_NUM 100
using namespace std;
class Singleton
{
public:
    static Singleton *getInstance()
    {
        static mutex smMutex ;
        if(smInstance==nullptr)
        {
            smMutex .lock();
            if(smInstance==nullptr)
            {
                smInstance= new Singleton;
            }
            smMutex.unlock();
        }
        return smInstance;
    }
    ~Singleton()
    {
        cout<<"singleton destuct"<<endl;
    }

    class Garbo
    {
    public:
        ~Garbo()
        {
            if(smInstance!=nullptr)
                delete smInstance;
        }
    };

private:
    //默认构造
    Singleton()
    {
        cout<<"singleton constuct"<<endl;
    }
    Singleton(const Singleton&)=delete;//废掉默认的拷贝构造
    Singleton(Singleton&&)=delete;//废掉默认的移动构造
    static Singleton *smInstance;
    static Garbo g;
};
Singleton *Singleton::smInstance=nullptr;
Singleton::Garbo Singleton::g;

void func(int i)
{
    //cout<<"aaa"<<i<<endl;
    Singleton::getInstance();
}

int main()
{
    thread *t100[THREAD_NUM];
    for(int i = 0 ;i<THREAD_NUM;++i)
    {
        t100[i] = new thread(func,i);
    }
    
    for(int i = 0 ;i<THREAD_NUM;++i)
    {
        t100[i]->join();
        delete t100[i];
    }

    return 0;
}
