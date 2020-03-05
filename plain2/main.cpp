#include<iostream>//直接到系统的安装目录下找源文件
#include"modbus.h"//直接在当前目录下找源文件
#include"open62541.h"
#include<cmath>
using namespace std;
#include"user_config.h"
user_config s;          //初始化用户配置类的对象

//一共有10种情况
void *read_write(void* threading)
{
    UA_Server *server=(UA_Server*)threading;
    if(s.get_F()==1)
    {
        if(s.get_connection()=="TCP"){
            if(s.get_mode()=="opc_read"){    //F=1,connection=TCP,mode=read时的节点添加，读取数据的函数实现（情况1）
                for(int i=0;i<s.get_num();i++){
                    UA_VariableAttributes attr = UA_VariableAttributes_default;
                    UA_UInt16 modbus = 0;
                    UA_Variant_setScalar(&attr.value, &modbus, &UA_TYPES[UA_TYPES_UINT16]);
                    attr.description = UA_LOCALIZEDTEXT("en-US","coil status"); //这个统一
                    attr.displayName = UA_LOCALIZEDTEXT("en-US","coil status");//对外显示的变量名
                    attr.dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
                    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

                    /* Add the variable node to the information model */
                    UA_NodeId modbusNodeId = UA_NODEID_NUMERIC(1, s.get_address()+i);
                    UA_QualifiedName modbusName = UA_QUALIFIEDNAME(1, "the answer");
                    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
                    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
                    int check=UA_Server_addVariableNode(server, modbusNodeId, parentNodeId,
                                              parentReferenceNodeId, modbusName,
                                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
                    if(check==0)
                    {
                        cout<<"addvariable done!"<<endl;
                    }
                    else{
                        cout<<"failed"<<endl;
                    }
                }
                while(1){//开始实时读取从Modbus slave上实时读取的数据
                    modbus_t *ctx=NULL;
                    uint8_t *tab_rq_coils=NULL;//存放从modbus中读取的值
                    uint16_t *modbus_opc_value=NULL;//将Modbus的值写入opc信息模型
                    ctx=modbus_new_tcp("127.0.0.1",502);
                    modbus_set_slave(ctx,s.get_ID());
                    modbus_set_debug(ctx,TRUE);
                    //开始连接tcp
                    if(modbus_connect(ctx)==-1){
                        fprintf(stderr,"connection failed:%s\n",modbus_strerror(errno));
                        modbus_free(ctx);
                    }
                    //初始化并分配内存
                    tab_rq_coils=(uint8_t*)malloc(s.get_num()*sizeof (uint8_t));
                    memset(tab_rq_coils,0,s.get_num()*sizeof (uint8_t));
                    modbus_opc_value=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
                    memset(modbus_opc_value,0,s.get_num()*sizeof (uint16_t));
                    //开始循环读取modbus的值并写入Opc信息模型中
                    int rc=modbus_read_bits(ctx,s.get_address(),s.get_num(),tab_rq_coils);
                    if(rc!=s.get_num()){
                        cout<<"read_bits failed!"<<endl;
                        cout<<"address="<<s.get_address();
                    }
                    for(int i=0;i<s.get_num();i++){
                        modbus_opc_value[i]=uint16_t(tab_rq_coils[i]);
                        cout<<modbus_opc_value[i]<<endl;
                        UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                        UA_UInt16 temp_value=modbus_opc_value[i];
                        UA_Variant modbusopc;
                        UA_Variant_init(&modbusopc);
                        UA_Variant_setScalar(&modbusopc,&temp_value,&UA_TYPES[UA_TYPES_UINT16]);
                        int check=UA_Server_writeValue(server,modbusNodeId,modbusopc);
                        if(check==0)
                        {
                            cout<<"write value to opc success!"<<endl;
                        }

                    }
                    Sleep(1000);
                    modbus_free(ctx);
                    modbus_close(ctx);
                }

            }
            else{//F=1，connection=TCP,mode=opc_write时添加节点，读写数据的函数实现（情况2）
                cout<<"now we deal with opc_write mode"<<endl;
                for(int i=0;i<s.get_num();i++){
                    UA_VariableAttributes attr = UA_VariableAttributes_default;
                    UA_UInt16 modbus = 0;
                    UA_Variant_setScalar(&attr.value, &modbus, &UA_TYPES[UA_TYPES_UINT16]);
                    attr.description = UA_LOCALIZEDTEXT("en-US","coil status"); //这个统一
                    attr.displayName = UA_LOCALIZEDTEXT("en-US","coil status");//对外显示的变量名
                    attr.dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
                    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

                    /* Add the variable node to the information model */
                    UA_NodeId modbusNodeId = UA_NODEID_NUMERIC(1, s.get_address()+i);
                    UA_QualifiedName modbusName = UA_QUALIFIEDNAME(1, "the answer");
                    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
                    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
                    int check=UA_Server_addVariableNode(server, modbusNodeId, parentNodeId,
                                              parentReferenceNodeId, modbusName,
                                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
                    if(check==0)
                    {
                        cout<<"addvariable done!"<<endl;
                    }
                    else{
                        cout<<"failed"<<endl;
                    }
                }
                while(1){
                    modbus_t *ctx=NULL;
                    uint8_t *tab_rq_bits;//间接从opc读取的值存放在这里，再写入modbus salve中
                    uint16_t *opc_modbus_value;//直接存放从modbus中读取的值
                    ctx=modbus_new_tcp("127.0.0.1",502);
                    modbus_set_slave(ctx,s.get_ID());
                    modbus_set_debug(ctx,TRUE);
                    if(modbus_connect(ctx)==-1){
                        fprintf(stderr,"connection failed:%s\n",modbus_strerror(errno));
                        modbus_free(ctx);
                    }
                    //初始化并分配内存
                    tab_rq_bits=(uint8_t*)malloc(s.get_num()*sizeof (uint8_t));
                    memset(tab_rq_bits,0,s.get_num()*sizeof (uint8_t));
                    opc_modbus_value=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
                    memset(opc_modbus_value,0,s.get_num()*sizeof (uint16_t));
                    //开始周期性读取opc上节点的值，并且实时写入modbus slave中
                    for(int i=0;i<s.get_num();i++){
                        UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                        UA_Variant temp_value;
                        UA_Variant_init(&temp_value);
                        int check=UA_Server_readValue(server,modbusNodeId,&temp_value);
                        if(check==0){
                            cout<<"read value success"<<endl;
                            opc_modbus_value[i]=*(uint16_t *)temp_value.data;//将读取到的节点值，存放到opc_modbus_value数组中
                            tab_rq_bits[i]=uint8_t(opc_modbus_value[i]);    //将读取到的节点值间接写到tab数组中，准备将起写入Modbus slave中
                        }
                        else{
                            cout<<"read value failed!"<<endl;
                        }
                    }
                    int rc=modbus_write_bits(ctx,s.get_address(),s.get_num(),tab_rq_bits);
                    for(int i=0;i<s.get_num();i++){
                        cout<<"write value to modbus success!"<<endl;
                        cout<<int(tab_rq_bits[i])<<endl;
                    }
                    Sleep(1000);
                    modbus_free(ctx);
                    modbus_close(ctx);
                }
            }
        }
        else{
            if(s.get_mode()=="opc_read"){//情况三：F=1，connection=TRU,mode=opc_read
                cout<<"now we start to work in RTU"<<endl;
                for(int i=0;i<s.get_num();i++){
                    UA_VariableAttributes attr = UA_VariableAttributes_default;
                    UA_UInt16 modbus = 0;
                    UA_Variant_setScalar(&attr.value, &modbus, &UA_TYPES[UA_TYPES_UINT16]);
                    attr.description = UA_LOCALIZEDTEXT("en-US","coil status"); //这个统一
                    attr.displayName = UA_LOCALIZEDTEXT("en-US","coil status");//对外显示的变量名
                    attr.dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
                    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

                    /* Add the variable node to the information model */
                    UA_NodeId modbusNodeId = UA_NODEID_NUMERIC(1, s.get_address()+i);
                    UA_QualifiedName modbusName = UA_QUALIFIEDNAME(1, "the answer");
                    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
                    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
                    int check=UA_Server_addVariableNode(server, modbusNodeId, parentNodeId,
                                              parentReferenceNodeId, modbusName,
                                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
                    if(check==0)
                    {
                        cout<<"addvariable done!"<<endl;
                    }
                    else{
                        cout<<"failed"<<endl;
                    }
                }
                while(1){
                    modbus_t *ctx;
                    uint8_t *tab_rq_bits;
                    uint16_t *modbus_opc_value;
                    ctx=modbus_new_rtu("COM3",9600,'E',8,1);
                    modbus_set_slave(ctx,s.get_ID());
                    modbus_set_debug(ctx,TRUE);
                    //开始连接RTU
                    if(modbus_connect(ctx)==-1){
                        fprintf(stderr,"connection failed:%s\n",modbus_strerror(errno));
                        modbus_free(ctx);
                    }
                    else{
                        cout<<"success"<<endl;
                    }
                    //初始化并分配内存
                    tab_rq_bits=(uint8_t*)malloc(s.get_num()*sizeof (uint8_t));
                    memset(tab_rq_bits,0,s.get_num()*sizeof (uint8_t));
                    modbus_opc_value=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
                    memset(modbus_opc_value,0,s.get_num()*sizeof (uint16_t));
                    //开始读取modbus的值并写入opc信息模型中
                    int rc=modbus_read_bits(ctx,s.get_address(),s.get_num(),tab_rq_bits);
                    if(rc!=s.get_num()){
                        cout<<"read bits failed!"<<endl;
                        cout<<"address="<<s.get_address()<<endl;
                    }
                    for(int i=0;i<s.get_num();i++){
                        modbus_opc_value[i]=uint16_t(tab_rq_bits[i]);
                        cout<<modbus_opc_value[i]<<endl;
                        UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                        UA_UInt16 temp_value=modbus_opc_value[i];
                        UA_Variant modbusopc;
                        UA_Variant_init(&modbusopc);
                        UA_Variant_setScalar(&modbusopc,&temp_value,&UA_TYPES[UA_TYPES_UINT16]);
                        int check=UA_Server_writeValue(server,modbusNodeId,modbusopc);
                        if(check==0)
                        {
                            cout<<"write value to opc success!"<<endl;
                        }
                    }
                    Sleep(100);
                    modbus_free(ctx);
                    modbus_close(ctx);

                }
            }
            else{//情况4，F=1，connectioN=RTU,mode=opc_write模式下Opc写数据的函数实现
                cout<<"now we work opc_write in RTU"<<endl;
                for(int i=0;i<s.get_num();i++){
                    UA_VariableAttributes attr = UA_VariableAttributes_default;
                    UA_UInt16 modbus = 0;
                    UA_Variant_setScalar(&attr.value, &modbus, &UA_TYPES[UA_TYPES_UINT16]);
                    attr.description = UA_LOCALIZEDTEXT("en-US","coil status"); //这个统一
                    attr.displayName = UA_LOCALIZEDTEXT("en-US","coil status");//对外显示的变量名
                    attr.dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
                    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

                    /* Add the variable node to the information model */
                    UA_NodeId modbusNodeId = UA_NODEID_NUMERIC(1, s.get_address()+i);
                    UA_QualifiedName modbusName = UA_QUALIFIEDNAME(1, "the answer");
                    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
                    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
                    int check=UA_Server_addVariableNode(server, modbusNodeId, parentNodeId,
                                              parentReferenceNodeId, modbusName,
                                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
                    if(check==0)
                    {
                        cout<<"addvariable done!"<<endl;
                    }
                    else{
                        cout<<"failed"<<endl;
                    }
                }
                while(1){
                    modbus_t *ctx;
                    uint8_t *tab_rq_bits;
                    uint16_t *opc_modbus_value;
                    ctx=modbus_new_rtu("COM3",9600,'E',8,1);
                    modbus_set_slave(ctx,s.get_ID());
                    modbus_set_debug(ctx,TRUE);
                    //开始连接RTU
                    if(modbus_connect(ctx)==-1){
                        fprintf(stderr,"connection failed:%s\n",modbus_strerror(errno));
                        modbus_free(ctx);
                    }
                    else{
                        cout<<"success"<<endl;
                    }
                    //初始化并分配内存
                    tab_rq_bits=(uint8_t*)malloc(s.get_num()*sizeof (uint8_t));
                    memset(tab_rq_bits,0,s.get_num()*sizeof (uint8_t));
                    opc_modbus_value=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
                    memset(opc_modbus_value,0,s.get_num()*sizeof (uint16_t));
                    //开始周期性读取opc上节点的值，并且实时写入modbus slave中
                    for(int i=0;i<s.get_num();i++){
                        UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                        UA_Variant temp_value;
                        UA_Variant_init(&temp_value);
                        int check=UA_Server_readValue(server,modbusNodeId,&temp_value);
                        if(check==0){
                            cout<<"read value success"<<endl;
                            opc_modbus_value[i]=*(uint16_t *)temp_value.data;//将读取到的节点值，存放到opc_modbus_value数组中
                            tab_rq_bits[i]=uint8_t(opc_modbus_value[i]);    //将读取到的节点值间接写到tab数组中，准备将起写入Modbus slave中
                        }
                        else{
                            cout<<"read value failed!"<<endl;
                        }
                    }
                    for(int i=0;i<s.get_num();i++){
                        UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                        UA_Variant temp_value;
                        UA_Variant_init(&temp_value);
                        int check=UA_Server_readValue(server,modbusNodeId,&temp_value);
                        if(check==0){
                            cout<<"read value success"<<endl;
                            opc_modbus_value[i]=*(uint16_t *)temp_value.data;//将读取到的节点值，存放到opc_modbus_value数组中
                            tab_rq_bits[i]=uint8_t(opc_modbus_value[i]);    //将读取到的节点值间接写到tab数组中，准备将起写入Modbus slave中
                        }
                        else{
                            cout<<"read value failed!"<<endl;
                        }
                    }
                    int rc=modbus_write_bits(ctx,s.get_address(),s.get_num(),tab_rq_bits);
                    for(int i=0;i<s.get_num();i++){
                        cout<<"write value to modbus success!"<<endl;
                        cout<<int(tab_rq_bits[i])<<endl;
                    }
                    Sleep(1000);
                    modbus_free(ctx);
                    modbus_close(ctx);
                }
            }
        }

    }
    else if(s.get_F()==2)
    {
        for(int i=0;i<s.get_num();i++){
            UA_VariableAttributes attr = UA_VariableAttributes_default;
            UA_UInt16 modbus = 0;
            UA_Variant_setScalar(&attr.value, &modbus, &UA_TYPES[UA_TYPES_UINT16]);
            attr.description = UA_LOCALIZEDTEXT("en-US","Input Status"); //这个统一
            attr.displayName = UA_LOCALIZEDTEXT("en-US","Input Status");//对外显示的变量名
            attr.dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
            attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

            /* Add the variable node to the information model */
            UA_NodeId modbusNodeId = UA_NODEID_NUMERIC(1, s.get_address()+i);
            UA_QualifiedName modbusName = UA_QUALIFIEDNAME(1, "the answer");
            UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
            UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
            int check=UA_Server_addVariableNode(server, modbusNodeId, parentNodeId,
                                      parentReferenceNodeId, modbusName,
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
            if(check==0)
            {
                cout<<"addvariable done!"<<endl;
            }
            else{
                cout<<"failed"<<endl;
            }
        }
        while(1){
            if(s.get_connection()=="TCP"){//情况5:F=2,connection=TCP,opc读取数据的函数实现
                modbus_t *ctx=NULL;
                uint8_t *tab_rq_coils=NULL;//存放从modbus中读取的值
                uint16_t *modbus_opc_value=NULL;//将Modbus的值写入opc信息模型
                ctx=modbus_new_tcp("127.0.0.1",502);
                modbus_set_slave(ctx,s.get_ID());
                modbus_set_debug(ctx,TRUE);
                //开始连接tcp
                if(modbus_connect(ctx)==-1){
                    fprintf(stderr,"connection failed:%s\n",modbus_strerror(errno));
                    modbus_free(ctx);
                }
                //初始化并分配内存
                tab_rq_coils=(uint8_t*)malloc(s.get_num()*sizeof (uint8_t));
                memset(tab_rq_coils,0,s.get_num()*sizeof (uint8_t));
                modbus_opc_value=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
                memset(modbus_opc_value,0,s.get_num()*sizeof (uint16_t));
                //开始循环读取modbus的值并写入Opc信息模型中
                int rc=modbus_read_bits(ctx,s.get_address(),s.get_num(),tab_rq_coils);
                if(rc!=s.get_num()){
                    cout<<"read_bits failed!"<<endl;
                    cout<<"address="<<s.get_address();
                }
                for(int i=0;i<s.get_num();i++){
                    modbus_opc_value[i]=uint16_t(tab_rq_coils[i]);
                    cout<<modbus_opc_value[i]<<endl;
                    UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                    UA_UInt16 temp_value=modbus_opc_value[i];
                    UA_Variant modbusopc;
                    UA_Variant_init(&modbusopc);
                    UA_Variant_setScalar(&modbusopc,&temp_value,&UA_TYPES[UA_TYPES_UINT16]);
                    int check=UA_Server_writeValue(server,modbusNodeId,modbusopc);
                    if(check==0)
                    {
                        cout<<"write value to opc success!"<<endl;
                    }

                }
                Sleep(1000);
                modbus_free(ctx);
                modbus_close(ctx);
            }
            else{//情况6：F=2,connection=RTU,
                modbus_t *ctx;
                uint8_t *tab_rq_bits;
                uint16_t *modbus_opc_value;
                ctx=modbus_new_rtu("COM3",9600,'E',8,1);
                modbus_set_slave(ctx,s.get_ID());
                modbus_set_debug(ctx,TRUE);
                //开始连接RTU
                if(modbus_connect(ctx)==-1){
                    fprintf(stderr,"connection failed:%s\n",modbus_strerror(errno));
                    modbus_free(ctx);
                }
                else{
                    cout<<"success"<<endl;
                }
                //初始化并分配内存
                tab_rq_bits=(uint8_t*)malloc(s.get_num()*sizeof (uint8_t));
                memset(tab_rq_bits,0,s.get_num()*sizeof (uint8_t));
                modbus_opc_value=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
                memset(modbus_opc_value,0,s.get_num()*sizeof (uint16_t));
                //开始读取modbus的值并写入opc信息模型中
                int rc=modbus_read_bits(ctx,s.get_address(),s.get_num(),tab_rq_bits);
                if(rc!=s.get_num()){
                    cout<<"read bits failed!"<<endl;
                    cout<<"address="<<s.get_address()<<endl;
                }
                for(int i=0;i<s.get_num();i++){
                    modbus_opc_value[i]=uint16_t(tab_rq_bits[i]);
                    cout<<modbus_opc_value[i]<<endl;
                    UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                    UA_UInt16 temp_value=modbus_opc_value[i];
                    UA_Variant modbusopc;
                    UA_Variant_init(&modbusopc);
                    UA_Variant_setScalar(&modbusopc,&temp_value,&UA_TYPES[UA_TYPES_UINT16]);
                    int check=UA_Server_writeValue(server,modbusNodeId,modbusopc);
                    if(check==0)
                    {
                        cout<<"write value to opc success!"<<endl;
                    }
                }
                Sleep(100);
                modbus_free(ctx);
                modbus_close(ctx);
            }
        }
    }
    else if(s.get_F()==3)       
    {
        for(int i=0;i<s.get_num();i++){
            UA_VariableAttributes attr = UA_VariableAttributes_default;
            UA_UInt16 modbus = 0;
            UA_Variant_setScalar(&attr.value, &modbus, &UA_TYPES[UA_TYPES_UINT16]);
            attr.description = UA_LOCALIZEDTEXT("en-US","Holding Register"); //这个统一
            attr.displayName = UA_LOCALIZEDTEXT("en-US","Holding Register");//对外显示的变量名
            attr.dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
            attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

            /* Add the variable node to the information model */
            UA_NodeId modbusNodeId = UA_NODEID_NUMERIC(1, s.get_address()+i);
            UA_QualifiedName modbusName = UA_QUALIFIEDNAME(1, "the answer");
            UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
            UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
            int check=UA_Server_addVariableNode(server, modbusNodeId, parentNodeId,
                                      parentReferenceNodeId, modbusName,
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
            if(check==0)
            {
                cout<<"addvariable done!"<<endl;
            }
            else{
                cout<<"failed"<<endl;
            }
        }
        while(1){
            if(s.get_connection()=="TCP"){
                modbus_t *ctx=NULL;
                uint16_t *tab_register=NULL;//存放从modbus中读取的值
                uint16_t *modbus_opc_value=NULL;//将Modbus的值写入opc信息模型
                ctx=modbus_new_tcp("127.0.0.1",502);
                modbus_set_slave(ctx,s.get_ID());
                modbus_set_debug(ctx,TRUE);
                //开始连接RTU
                if(modbus_connect(ctx)==-1){
                    fprintf(stderr,"connection failed:%s\n",modbus_strerror(errno));
                    modbus_free(ctx);
                }

                //初始化存放Modbus数组并且分配内存空间
                tab_register=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
                memset(tab_register,0,s.get_num()*sizeof (uint16_t));
                modbus_opc_value=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
                memset(modbus_opc_value,0,s.get_num()*sizeof (uint16_t));
                if(s.get_mode()=="opc_read"){//情况7：F=3,connection=TCP,mode=opc_read情况下，opc读取数据的函数实现
                    cout<<"now we work in situation 6"<<endl;
                    int rc=modbus_read_registers(ctx,s.get_address(),s.get_num(),tab_register);
                    if(rc!=s.get_num()){
                        cout<<"read_bits failed!"<<endl;
                        cout<<"address="<<s.get_address();
                    }
                    for(int i=0;i<s.get_num();i++){
                        modbus_opc_value[i]=uint16_t(tab_register[i]);
                        cout<<modbus_opc_value[i]<<endl;
                        UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                        UA_UInt16 temp_value=modbus_opc_value[i];
                        UA_Variant modbusopc;
                        UA_Variant_init(&modbusopc);
                        UA_Variant_setScalar(&modbusopc,&temp_value,&UA_TYPES[UA_TYPES_UINT16]);
                        int check=UA_Server_writeValue(server,modbusNodeId,modbusopc);
                        if(check==0)
                        {
                            cout<<"write value to opc success!"<<endl;
                        }

                    }
                    Sleep(1000);
                    modbus_free(ctx);
                    modbus_close(ctx);
                }
                else{  //情况8：F=3，connection=RTU,mode=opc_write情况下，opc写入数据到Modbus的函数实现
                    for(int i=0;i<s.get_num();i++){
                        UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                        UA_Variant temp_value;
                        UA_Variant_init(&temp_value);
                        int check=UA_Server_readValue(server,modbusNodeId,&temp_value);
                        if(check==0){
                            cout<<"read value success"<<endl;
                            modbus_opc_value[i]=*(uint16_t *)temp_value.data;//将读取到的节点值，存放到opc_modbus_value数组中
                            tab_register[i]=modbus_opc_value[i];    //将读取到的节点值间接写到tab数组中，准备将起写入Modbus slave中
                        }
                        else{
                            cout<<"read value failed!"<<endl;
                        }
                    }
                    int rc=modbus_write_registers(ctx,s.get_address(),s.get_num(),tab_register);
                    for(int i=0;i<s.get_num();i++){
                        cout<<"write value to modbus success!"<<endl;
                        cout<<int(tab_register[i])<<endl;
                    }
                    Sleep(1000);
                    modbus_free(ctx);
                    modbus_close(ctx);
                }
            }
            else{
                cout<<"now we go to"<<endl;
                modbus_t *ctx=NULL;
                uint16_t *tab_register=NULL;//存放从modbus中读取的值
                uint16_t *modbus_opc_value=NULL;//将Modbus的值写入opc信息模型
                ctx=modbus_new_rtu("COM3",9600,'E',8,1);
                modbus_set_slave(ctx,s.get_ID());
                modbus_set_debug(ctx,TRUE);
                //开始连接RTU
                if(modbus_connect(ctx)==-1){
                    fprintf(stderr,"connection failed:%s\n",modbus_strerror(errno));
                    modbus_free(ctx);
                }
                //初始化存放Modbus数组并且分配内存空间
                tab_register=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
                memset(tab_register,0,s.get_num()*sizeof (uint16_t));
                modbus_opc_value=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
                memset(modbus_opc_value,0,s.get_num()*sizeof (uint16_t));
                if(s.get_mode()=="opc_read"){//情况9：F=3，connection=RTU,mode=opc_read情况下，opc读取数据的函数实现
                    int rc=modbus_read_registers(ctx,s.get_address(),s.get_num(),tab_register);
                    if(rc!=s.get_num()){
                        cout<<"read bits failed!"<<endl;
                        cout<<"address="<<s.get_address()<<endl;
                    }
                    for(int i=0;i<s.get_num();i++){
                        modbus_opc_value[i]=tab_register[i];
                        cout<<modbus_opc_value[i]<<endl;
                        UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                        UA_UInt16 temp_value=modbus_opc_value[i];
                        UA_Variant modbusopc;
                        UA_Variant_init(&modbusopc);
                        UA_Variant_setScalar(&modbusopc,&temp_value,&UA_TYPES[UA_TYPES_UINT16]);
                        int check=UA_Server_writeValue(server,modbusNodeId,modbusopc);
                        if(check==0)
                        {
                            cout<<"write value to opc success!"<<endl;
                        }
                    }
                    Sleep(100);
                    modbus_free(ctx);
                    modbus_close(ctx);
                }
                else{//情况10：F=3，connection=RTU,mode=opc_write情况下，opc写入数据的函数实现
                    //开始周期性读取opc上节点的值，并且实时写入modbus slave中
                    for(int i=0;i<s.get_num();i++){
                        UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                        UA_Variant temp_value;
                        UA_Variant_init(&temp_value);
                        int check=UA_Server_readValue(server,modbusNodeId,&temp_value);
                        if(check==0){
                            cout<<"read value success"<<endl;
                            modbus_opc_value[i]=*(uint16_t *)temp_value.data;//将读取到的节点值，存放到opc_modbus_value数组中
                            tab_register[i]=modbus_opc_value[i];    //将读取到的节点值间接写到tab数组中，准备将起写入Modbus slave中
                        }
                        else{
                            cout<<"read value failed!"<<endl;
                        }
                    }
                    int rc=modbus_write_registers(ctx,s.get_address(),s.get_num(),tab_register);
                    for(int i=0;i<s.get_num();i++){
                        cout<<"write value to modbus success!"<<endl;
                        cout<<int(tab_register[i])<<endl;
                    }
                    Sleep(1000);
                    modbus_free(ctx);
                    modbus_close(ctx);

                }
            }
        }
    }
    else
    {
        for(int i=0;i<s.get_num();i++){
            UA_VariableAttributes attr = UA_VariableAttributes_default;
            UA_UInt16 modbus = 0;
            UA_Variant_setScalar(&attr.value, &modbus, &UA_TYPES[UA_TYPES_UINT16]);
            attr.description = UA_LOCALIZEDTEXT("en-US","Input Register"); //这个统一
            attr.displayName = UA_LOCALIZEDTEXT("en-US","Input Register");//对外显示的变量名
            attr.dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
            attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

            /* Add the variable node to the information model */
            UA_NodeId modbusNodeId = UA_NODEID_NUMERIC(1, s.get_address()+i);
            UA_QualifiedName modbusName = UA_QUALIFIEDNAME(1, "the answer");
            UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
            UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
            int check=UA_Server_addVariableNode(server, modbusNodeId, parentNodeId,
                                      parentReferenceNodeId, modbusName,
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
            if(check==0)
            {
                cout<<"addvariable done!"<<endl;
            }
            else{
                cout<<"failed"<<endl;
            }
        }
        while(1){
            modbus_t *ctx=NULL;
            uint16_t *tab_register=NULL;//存放从modbus中读取的值
            uint16_t *modbus_opc_value=NULL;//将Modbus的值写入opc信息模型
            if(s.get_connection()=="TCP"){
                ctx=modbus_new_tcp("127.0.0.1",502);
            }
            else{
                ctx=modbus_new_rtu("COM3",9600,'E',8,1);
            }
            modbus_set_slave(ctx,s.get_ID());
            modbus_set_debug(ctx,TRUE);
            //开始连接RTU
            if(modbus_connect(ctx)==-1){
                fprintf(stderr,"connection failed:%s\n",modbus_strerror(errno));
                modbus_free(ctx);
            }

            //初始化存放Modbus数组并且分配内存空间
            tab_register=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
            memset(tab_register,0,s.get_num()*sizeof (uint16_t));
            modbus_opc_value=(uint16_t*)malloc(s.get_num()*sizeof (uint16_t));
            memset(modbus_opc_value,0,s.get_num()*sizeof (uint16_t));
            int rc=modbus_read_input_registers(ctx,s.get_address(),s.get_num(),tab_register);
            if(rc!=s.get_num()){
                cout<<"read_bits failed!"<<endl;
                cout<<"address="<<s.get_address();
            }
            for(int i=0;i<s.get_num();i++){
                modbus_opc_value[i]=tab_register[i];
                cout<<modbus_opc_value[i]<<endl;
                UA_NodeId modbusNodeId =UA_NODEID_NUMERIC(1,s.get_address()+i);
                UA_UInt16 temp_value=modbus_opc_value[i];
                UA_Variant modbusopc;
                UA_Variant_init(&modbusopc);
                UA_Variant_setScalar(&modbusopc,&temp_value,&UA_TYPES[UA_TYPES_UINT16]);
                int check=UA_Server_writeValue(server,modbusNodeId,modbusopc);
                if(check==0)
                {
                    cout<<"write value to opc success!"<<endl;
                }

            }
            if(s.get_connection()=="TCP"){
                Sleep(1000);
            }
            else{
               Sleep(100);
            }
            modbus_free(ctx);
            modbus_close(ctx);

        }
    }
}



static volatile UA_Boolean running = true;
static void stopHandler(int sign) {

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");

    running = false;

}

int main()
{

    signal(SIGINT,stopHandler);
    signal(SIGTERM,stopHandler);
    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    s.set_user_config();    //调用用户设置函数，手动配置参数
    s.show_user_config();   //统一将用户设置的参数展现出来
    pthread_t read_write_thread;
    pthread_create(&read_write_thread,NULL,read_write,(void*)server);
//    read_write(server);  //根据用户设置的参数ID,address,F添加Modbus节点变量,根据相应的模式实现读取
    UA_StatusCode retval = UA_Server_run(server, &running);
    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;

    return 0;
}

