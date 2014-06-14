#ifndef _JSONLOADER_H
#define _JSONLOADER_H
//=========================================================================
//File:JsonLoader.h
//Author: Bianan@chukong-inc.com
//Desc:加载JSON的导出文件的类
//=========================================================================

#include <stdio.h>
#include <vector>
#include <cstdio>
#include "rapidjson.h"
#include "document.h"

using namespace std;
using namespace rapidjson;
//const	char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
class	JsonLoader;
class   stJsonNode;
//节点属性描述
struct  stJsonAttribDesc
{
	Type				m_Type;
	string				m_Name;
	string				m_DefaultValue;
public:
	//输出到二进制文件
	void				WriteToFile(FILE* hFile);
	//从二进制文件中读取
	char*				ReadFromBuff(char* pBuff);
};

//JSON类节点
struct  stJsonObjectDesc
{
	char								m_MD5[32];			//32字符MD5码对应
	string								m_Name;				//节点名称
	vector<stJsonAttribDesc>			m_AttribVec;		//属性容器
	vector<stJsonNode*>					m_MemoryAddrVec;	//相应内存地址
public:
	//查找类型描述
	int					FindAttribDesc(string& strName);
	//输出到二进制文件
	void				WriteToFile(FILE* hFile);
	//从二进制文件中读取
	char*				ReadFromBuff(char* pBuff);
};


//JSON节点类型
struct  stJsonNode
{
	Type				m_Type;
	string				m_Name;
	string				m_Value;
	vector<stJsonNode>	m_ChildVec;
public:
	//构造
	stJsonNode();
	//打印
	void				Print(void*	pFileName = NULL,int vLayer = 0,bool bEndNode = false);
	//从外部结点中取得与当前匹配的节点
	stJsonNode			GetGoodJsonNode(JsonLoader*		pJson,bool bIsRoot = true);
	//查找子结点
	int					FindChild(const char* szName);
	//输出到二进制文件
	void				WriteToFile(FILE* hFile);
	//从二进制文件中读取
	char*				ReadFromBuff(char* pBuff);
	//取得成生二进制结构所占用的大小
	int					GetPackedSize();

};


//二进制头文件
struct		stCocoFileHeader
{
	char	m_FileDesc[32];
	char	m_Version[32];
	int		m_ObjectCount;
};


//JsonLoader
class JsonLoader
{
	//根结点
	stJsonNode					m_RootNode;
	//类结点容器
	vector<stJsonObjectDesc>	m_ObjectDescVec;
public:
	//构造
	JsonLoader();
	//析构
	~JsonLoader();

public:
	//加载JSON
	bool					ReadCocoJsonBuff(const char* pJsonBuff);
	//打印JSON
	void					PrintAllJsonNode();
	//写入二进制文件
	bool					WriteCocoBinBuff(const char* pBinFileName,stJsonNode&	vNewJsonNode);
	//从二进制文件中读取
	bool					ReadCocoBinBuff(char* pBinBuff);
	//取得根结点
	stJsonNode&				GetRootJsonNode();
	//查询相应的类描述，	返回索引
	int						GetObjectDescIndex(string& strName);
	//取得相应的类描述
	stJsonObjectDesc&		GetObjectDesc(int vIndex);	
	//清空所有的类描述上的内容节点
	void					CleanObjectMemoryAddr();
private:
	//增加一个结点
	stJsonNode&				AddJsonNode(Type vType,string strName,string strValue,stJsonNode& vParentNode);
	//增加一个结点
	stJsonNode&				AddJsonNode_ArrayItem(const rapidjson::Value &	vValue,stJsonNode& vParentNode);
	//增加一个结点
	stJsonNode&				AddJsonNode_Object(const rapidjson::Value &	vValue,stJsonNode& vParentNode);

private:
	//记录一下类结点
	void					AddObjectDesc(string strName,const rapidjson::Value &	vValue);
	//取得一个值项的字符串
	string					GetValueString(const rapidjson::Value &	vValue);
			
};

#endif