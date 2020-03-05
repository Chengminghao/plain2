#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include<iostream>
using namespace std;

class user_config
{
public:
    user_config();
    user_config(string a,int b,int c,int d,int e,string f);

    string set_connection();//对连接模式的设置，并赋值
    int set_ID();           //对ID的设置，并赋值
    int set_F();            //对F的设置，并赋值
    int set_address();      //对起始地址设置，并赋值
    int set_num();          //对寄存器数量设置，并赋值
    string set_mode();      //对模式设置，并赋值

    string get_connection();//外界获取连接模式的API
    int get_ID();           //外界获取ID的API
    int get_F();              //外界获取F的API
    int get_address();      //外界获取地址的API
    int get_num();          //外界过去寄存器数量的API
    string get_mode();      //外界获取OPC client读写模式的API

    void set_user_config(); //一次性将6个参数设置好
    void show_user_config();//通过实例，将所设置的参数全部展现出来





private:
    string connection;//modbus的连接模式：RTU/TCP
    int ID;           //设备的ID
    int F;            //寄存器的功能吗（4种，判断寄存器的种类）
    int address;      //某个设备ID下所希望读写的寄存器其实地址
    int num;          //所希望读写的寄存器数量
    string mode;      //半双工通信，设置为opc client读模式或者是写模式



};

#endif // USER_CONFIG_H
