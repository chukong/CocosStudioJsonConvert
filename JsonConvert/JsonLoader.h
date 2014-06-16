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
#include "stdint.h"
#include "rapidjson.h"
#include "document.h"
#pragma pack (4)
using namespace std;
using namespace rapidjson;
//const	char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
class	JsonLoader;
struct   stJsonNode;
//节点属性描述
struct  stJsonAttribDesc
{
	Type				m_Type;
	string				m_Name;
	string				m_DefaultValue;
public:
	//比较类型描述
	bool				CompareAttribDesc(string& strName,bool bFastCompare = true);
	//取得结构所占用的大小
	int					GetSize();
};

//JSON类描述节点
struct  stJsonObjectDesc
{
	vector<string>						m_NameVec;				//节点名称容器
	vector<stJsonAttribDesc>			m_AttribVec;			//属性容器
	bool								m_HasNode;				//有数据
public:
	//查找类型描述
	int					FindAttribDesc(string& strName,int& nNameIndex);
};


//JSON节点类型
struct  stJsonNode
{
	Type				m_Type;
	string				m_Name;
	string				m_Value;
	int32_t				m_ObjIndex;
	int32_t				m_AttribIndex;
	uint64_t			m_lChildMemAddr;
	vector<stJsonNode>	m_ChildVec;
public:
	//构造
	stJsonNode();
	//打印
	void				Print(void*	pFileName = NULL,int vLayer = 0,bool bEndNode = false,bool bParentNodeIsArray = false);
	//从外部结点中取得与当前匹配的节点
	stJsonNode			GetGoodJsonNode(JsonLoader*		pJson,bool bIsRoot = true,const char* szParentArrayName = NULL);
	//查找子结点
	int					FindChild(const char* szName);
	//取得所有的子结点，孙节点
	int					GetAllJsonNodes(vector<stJsonNode*>&  vAllChildVec);

};

//JsonLoader
class JsonLoader
{
	//根结点
	stJsonNode					m_RootNode;
	//类结点容器
	vector<stJsonObjectDesc>	m_ObjectDescVec;

	//版本
	string						m_strVersion;

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

	//取得根结点
	stJsonNode&				GetRootJsonNode();
	//查询相应的类描述，	返回索引
	int						GetObjectDescIndex(string& strName);
	//取得相应的类描述
	stJsonObjectDesc&		GetObjectDesc(int vIndex);	
	//取得导出时计算的将使用的类索引
	int						GetUsedObjectDescIndex(int vIndex);
	//取得版本
	const string&			GetVersion(){return m_strVersion;}
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
#pragma pack ()
#endif