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
	//输出到二进制文件
	void				WriteToFile(FILE* hFile);
	//输出到二进制文件
	void				WriteToFile_NoString(FILE* hFile,uint64_t& vWritePosition,uint64_t& vWriteSize);
	//输出到二进制文件
	void				WriteToFile_String(FILE* hFile);
	//从二进制文件中读取
	char*				ReadFromBuff(char* pBuff);
	//从二进制文件中读取
	char*				ReadFromBuff(char* pBuff,uint64_t& vWritePosition);
	//取得结构所占用的大小
	int					GetSize();
};

//JSON类描述节点
struct  stJsonObjectDesc
{
	string								m_Name;					//节点名称
	vector<stJsonAttribDesc>			m_AttribVec;			//属性容器
	bool								m_HasNode;				//有数据
public:
	//查找类型描述
	int					FindAttribDesc(string& strName);
	//输出到二进制文件
	void				WriteToFile(FILE* hFile);
	//输出到二进制文件
	void				WriteToFile_Desc(FILE* hFile,uint64_t& vDescPosition,uint64_t& vAttribPosition,uint64_t& vStringSize);
	//输出到二进制文件
	void				WriteToFile_Attrib(FILE* hFile,uint64_t& vDescWritePosition,uint64_t& vWriteSize);
	//输出到二进制文件
	void				WriteToFile_String(FILE* hFile);
	//输出到二进制文件
	void				WriteToFile_AttribString(FILE* hFile);
	//从二进制文件中读取
	char*				ReadFromBuff(char* pBuff);
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
	//输出到二进制文件
	void				WriteToFile(JsonLoader*		pJson,FILE* hFile,bool bIsRoot = true);
	//输出到二进制文件
	void				WriteToFile_NoString(JsonLoader*		pJson,uint64_t& vStringSize,FILE* hFile);
	//输出到二进制文件
	void				WriteChildToFile_NoString(JsonLoader*		pJson,uint64_t& vStringSize,FILE* hFile);
	//输出到二进制文件
	void				WriteToFile_String(JsonLoader*		pJson,FILE* hFile);
	//输出到二进制文件
	void				WriteChildToFile_String(JsonLoader*		pJson,FILE* hFile);
	//从二进制文件中读取
	char*				ReadFromBuff(JsonLoader*		pJson,char* pBuff,bool bIsArray,stJsonObjectDesc* pJsonObjDesc,int vAttribIndex);

	//取得所有的子结点，孙节点
	int					GetAllJsonNodes(vector<stJsonNode*>&  vAllChildVec);

};

//导出用节点属性描述
struct  stExpJsonAttribDesc
{
	Type								m_Type;					//对应的类型
	uint64_t/*char*		*/				m_szName;				//对应的名称
	uint64_t/*char*		*/				m_szDefaultValue;		//对应的默认值
public:
	//重建
	void	ReBuild(char* pStringMemoryAddr)
	{
		m_szName = m_szName + (uint64_t)pStringMemoryAddr;
		m_szDefaultValue = m_szDefaultValue + (uint64_t)pStringMemoryAddr;
	}
}
;
//导出用JSON类描述节点
struct  stExpJsonObjectDesc
{
	uint32_t							m_nAttribNum;			//属性数量
	uint64_t/*char*		*/				m_szName;				//对应的名称字符串的位置,默认为0
	uint64_t/*stExpJsonAttribDesc*	*/	m_pAttribDescArray;		//属性数组
public:
	//重建
	void	ReBuild(char* pAttribMemoryAddr,char* pStringMemoryAddr)
	{
		m_szName = m_szName + (uint64_t)pStringMemoryAddr;
		m_pAttribDescArray = m_pAttribDescArray + (uint64_t)pAttribMemoryAddr;
		stExpJsonAttribDesc* tpAttribDescArray = (stExpJsonAttribDesc*)m_pAttribDescArray;
		for(int i = 0 ; i < m_nAttribNum ; i++)
		{
			tpAttribDescArray[i].ReBuild(pStringMemoryAddr);
		}
	}

};


//JSON节点类型
struct  stExpJsonNode
{
friend class CJsonNodeExport;
friend struct stJsonNode;
protected:
	int32_t							m_ObjIndex;				//对应的物体索引
	int32_t							m_AttribIndex;			//属性的物体索引
	uint32_t						m_ChildNum;				//子节点数量
	uint64_t/*char*		*/			m_szValue;				//对应的默认值
	uint64_t/*stExpJsonNode*	*/	m_ChildArray;			//对应的子节点数组
public:

	//取类型
	Type				GetType(JsonLoader*		pJson);
	//取名称
	char*				GetName(JsonLoader*		pJson);
	//取值
	char*				GetValue();
	//取得子结点数量
	int					GetChildNum();
	//取得子结点数组
	stExpJsonNode*		GetChildArray();
public:
	//重建
	inline  void	ReBuild(char* pJsonNodeAddr,char* pStringMemoryAddr);
	//打印
	void	Print(JsonLoader*		pJson,void*	pFileName = NULL,int vLayer = 0,bool bEndNode = false,bool bParentNodeIsArray = false);
}
;

//JsonNode导出器
class	CJsonNodeExport
{
	uint32_t						m_nJsonNodeCount;
	uint32_t						m_nFillIndex;
	uint64_t/*stExpJsonNode* */		m_pJsonNodeArray;
public:
	//构造
	CJsonNodeExport();
	//析构
	~CJsonNodeExport();
public:
	//创建
	bool				CreateMemory(int vCount);
	//清空
	void				ResetFill();
	//填充根结点
	int					FillRootDate(JsonLoader*		pJson,stJsonNode*	pJsonNode,uint64_t& vStringDataAddr,bool bCopyData);
	//填充
	int					FillDate(JsonLoader*		pJson,stJsonNode*	pJsonNode,uint64_t& vStringDataAddr,bool bCopyData);
	//取得内存
	stExpJsonNode*		GetMemory();
};

//二进制头文件
struct		stCocoFileHeader
{
	char			m_FileDesc[32];
	char			m_Version[32];
	int				m_ObjectCount;
};
;
 
//二进制头文件
struct		stCocoFileHeader_Fast
{
	char			m_FileDesc[32];
	char			m_Version[32];
	uint32_t		m_nFirstUsed;
	uint32_t		m_ObjectCount;
	uint64_t		m_lAttribMemAddr;
	uint64_t		m_JsonNodeMemAddr;
	uint64_t		m_lStringMemAddr;
	
};

//JsonLoader
class JsonLoader
{
	//根结点
	stJsonNode					m_RootNode;
	//类结点容器
	vector<stJsonObjectDesc>	m_ObjectDescVec;

	//头信息
	stCocoFileHeader_Fast*		m_pFileHeader;
	//根结点
	stExpJsonNode*				m_pRootNode_Fast;
	//类结点容器
	stExpJsonObjectDesc*		m_pObjectDescArray_Fast;
	//

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
	//导出二进制文件
	bool					WriteCocoBinBuff(const char* pBinFileName,stJsonNode&	vNewJsonNode);
	//从二进制文件中读取
	bool					ReadCocoBinBuff(char* pBinBuff);

	//导出二进制文件
	bool					WriteCocoBinBuff_Fast(const char* pBinFileName,stJsonNode&	vNewJsonNode);
	//从二进制文件中读取
	bool					ReadCocoBinBuff_Fast(char* pBinBuff);


	//取得根结点
	stJsonNode&				GetRootJsonNode();
	//查询相应的类描述，	返回索引
	int						GetObjectDescIndex(string& strName);
	//取得相应的类描述
	stJsonObjectDesc&		GetObjectDesc(int vIndex);	
	//取得导出时计算的将使用的类索引
	int						GetUsedObjectDescIndex(int vIndex);

	//头信息
	stCocoFileHeader_Fast*	GetFileHeader_Fast(){return m_pFileHeader;}
	//根结点
	stExpJsonNode*			GetJsonNode_Fast(){return	m_pRootNode_Fast;}
	//类结点容器
	stExpJsonObjectDesc*	GetJsonObjectDesc_Fast(){return	m_pObjectDescArray_Fast;}
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