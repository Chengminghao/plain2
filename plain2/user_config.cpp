#include "user_config.h"
#include<iostream>
using namespace std;
#include"modbus.h"
#include"open62541.h"


//定义user_config类的默认构造函数
user_config::user_config()
{

}

//定义user_config类的构造函数
user_config::user_config(string a,int b,int c,int d,int e,string f):connection(a),ID(b),F(c),address(d),num(e),mode(f)
{

}

//定义连接模式的实现
string user_config::set_connection()
{
    string judge_connection;
    cout<<"now,please choose a connection!"<<endl;
    cout<<"you have 2 choices:TCP or RTU"<<endl;
    cin>>judge_connection;
    if(judge_connection=="TCP")
    {
        cout<<"you choose the connection:"<<judge_connection<<endl<<endl;
    }
    else if(judge_connection=="RTU")
    {
        cout<<"you choose the connetion:"<<judge_connection<<endl<<endl;;
    }
    else
    {
        cout<<"fail to choose connection!"<<endl;
        return 0;
    }
    connection=judge_connection;
    return connection;
}
//定义ID的实现
int user_config::set_ID()
{
    int judge_ID;
    cout<<"please choose an ID(0-255) you want to read/write:"<<endl;
    cin>>judge_ID;
    if(judge_ID>=0&&judge_ID<=255)
    {
        cout<<"you choose the ID is:"<<judge_ID<<endl<<endl;;
    }
    else
    {
        cout<<"ID is out of rank"<<endl;
        return 0;
    }
   ID=judge_ID;
   return  ID;
}
//定义F的实现
int user_config::set_F()
{
    int judge_F;
    cout<<"please choose a F you want to read/write:"<<endl;
    cout<<"Coil Status(0x):01"<<endl;
    cout<<"Input Status(1x):02"<<endl;
    cout<<"Holding Register(2x):03"<<endl;
    cout<<"Input Register(3x):04"<<endl;
    cin>>judge_F;
    if(judge_F==01)
    {
        cout<<"you choose Coil Status:"<<judge_F<<endl<<endl;
    }
    else if(judge_F==02)
    {
        cout<<"you choose Input Status:"<<judge_F<<endl<<endl;
    }
    else if(judge_F==03)
    {
        cout<<"you choose Holding Register:"<<judge_F<<endl<<endl;
    }
    else if(judge_F==04)
    {
        cout<<"you choose Input Register:"<<judge_F<<endl<<endl;
    }
    else
    {
        cout<<"failed to choose F!"<<endl;
        return 0;
    }
    F=judge_F;
    return F;

}
//定义adress的实现
int user_config::set_address()
{
    int judge_address;
    cout<<"please set an start_address you want to read/write:"<<endl;
    cin>>judge_address;
    cout<<"the start_address you choose is:"<<judge_address<<endl<<endl;;
    address=judge_address;
    return address;
}
//定义num的实现
int user_config::set_num()
{
    int judge_num;
    cout<<"please set the number you want to read/write:"<<endl;
    cin>>judge_num;
    cout<<"the num you set is:"<<judge_num<<endl<<endl;;
    num=judge_num;
    return num;
}
//定义mode的实现
string user_config::set_mode()
{
    string judge_mode;
    cout<<"pleass set the mode,you have 2 choices:opc_read/opc_write"<<endl;
    cin>>judge_mode;
    if(judge_mode=="opc_read")
    {
        cout<<"the mode you choose is:"<<judge_mode<<endl<<endl;
    }
    else if(judge_mode=="opc_write")
    {
        cout<<"the mode you choose is:"<<judge_mode<<endl<<endl;
    }
    else
    {
        cout<<"fail to set mode"<<endl;
    }
    mode=judge_mode;
    return mode;
}
//外界访问connection的APT
string user_config::get_connection()
{
    return connection;
}
//外界访问ID的API
int user_config::get_ID()
{
    return ID;
}
//外界访问F的API
int user_config::get_F()
{
    return F;
}
//外界访问adress的API
int user_config::get_address()
{
    return address;
}
//外界访问num的API
int user_config::get_num()
{
    return num;
}
//外界访问mode的API
string user_config::get_mode()
{
    return mode;
}
//一次性将用户输入的信息设置好，其实只是为了简化main函数
void user_config::set_user_config()
{
    set_connection();
    set_mode();
    set_ID();
    set_F();
    set_address();
    set_num();
}
//该函数可以将用户的配置信息全部显示出来
void user_config::show_user_config()
{
    cout<<"user_config information are as follow:"<<endl;
    cout<<"connection:"<<connection<<endl;
    cout<<"mode:"<<mode<<endl;
    cout<<"ID:"<<ID<<endl;
    cout<<"F:"<<F<<endl;
    cout<<"address:"<<address<<endl;
    cout<<"num:"<<num<<endl<<endl;
}








