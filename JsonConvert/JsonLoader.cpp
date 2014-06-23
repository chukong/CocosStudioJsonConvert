#include "JsonLoader.h"
#include <tchar.h>
#include <windows.h>
const	char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
const   char* kObjKeyName[] = { "__type" , "classname" };
char	g_Buff[2048];

void string_replace(std::string& strBig, const std::string & strsrc, const std::string &strdst) 
{
	std::string::size_type pos = 0;
	while( (pos = strBig.find(strsrc, pos)) != string::npos)
	{
		strBig.replace(pos, strsrc.length(), strdst);
		pos += strdst.length();
	}
}


//将一个ASCII文件转化为UTF8文件
bool    ConvAsciiTxtToUTF8(const char* szAsciiFileName,const char* szUTF8TxtFile)
{
	FILE    *AsciiFile = fopen(szAsciiFileName,"rt");
	if( NULL == AsciiFile )
	{
		return false;
	}
	long    filesize = 0;
	if( fseek( AsciiFile,0,SEEK_END ))
	{
		return false;
	}
	filesize = ftell( AsciiFile );
	if( fseek( AsciiFile,0,SEEK_SET ) )
	{
		return false;
	}

	FILE    *UtfFile = fopen(szUTF8TxtFile,"wt,ccs=UTF-8");
	if( NULL == UtfFile )
	{
		return false;
	}    

	long    offset = 0;
	WCHAR   utfArr[4096];    

	while(1)
	{
		memset(g_Buff,'\0',2048);
		memset(utfArr,'\0',4096);
		int acual_count = fread( g_Buff,sizeof(char),2047,AsciiFile);

		if( acual_count <= 0 )        
			break;

		int nWCharSize = MultiByteToWideChar(CP_ACP,NULL,g_Buff,-1,utfArr,4096);
		if( 0 == nWCharSize)
		{
			return false;
		}
		
		fwrite(utfArr,  sizeof(wchar_t), nWCharSize, UtfFile);  
			
		offset += acual_count;
		if( offset >= filesize)
			break;
	}

	if( AsciiFile )
	{
		fclose( AsciiFile );
		AsciiFile = NULL ;    
	}

	if( UtfFile )
	{
		fclose(UtfFile);
		UtfFile = NULL;
	}

	return true;
}


//按优先级取出物体的索引
int     GetObjectIndex(JsonLoader*	pLoader,vector<stJsonNode>& vChildVec)
{
	for(int i = 0 ; i < 2; i++)
	{
		vector<stJsonNode>::iterator tIter;
		for(tIter = vChildVec.begin(); tIter != vChildVec.end(); tIter++)
		{
			if(0 == strcmp(tIter->m_Name.c_str(),kObjKeyName[i]))
			{
				return pLoader->GetObjectDescIndex(tIter->m_Value);
			}
		}
	}
	return -1;
}
//比较类型描述
bool	stJsonAttribDesc::CompareAttribDesc(string& strName,string& strOutName)
{
	//=====================================================
	vector<string> m_strVec2;
	const	char*	pChar2 = m_Name.c_str();
	int		nLen2 = m_Name.size();
	char*   szMemory2 = new char[nLen2+1];
	strcpy(szMemory2,pChar2);
	char*   szFindStr2 = szMemory2 ;
	szFindStr2 = strtok(szMemory2,",");
	while(szFindStr2)
	{
		string str = szFindStr2 ;
		m_strVec2.push_back(str);

		szFindStr2 = strtok(NULL,",");
		if(!szFindStr2)break;
	}
	delete[] szMemory2;

	//=====================================================
	vector<string> m_strVec1;
	const	char*	pChar = strName.c_str();
	int		nLen = strName.size();
	char*   szMemory = new char[nLen+1];
	strcpy(szMemory,pChar);
	char*   szFindStr = szMemory ;
	szFindStr = strtok(szMemory,",");
	while(szFindStr)
	{
		string str = szFindStr ;
		m_strVec1.push_back(str);

		szFindStr = strtok(NULL,",");
		if(!szFindStr)break;
	}
	delete[] szMemory;

	//=====================================================
	vector<string>::iterator tIter;
	for(tIter = m_strVec1.begin(); tIter != m_strVec1.end(); tIter++)
	{
		vector<string>::iterator tIter2;
		for(tIter2 = m_strVec2.begin(); tIter2 != m_strVec2.end(); tIter2++)
		{
			if( 0 == strcmp(tIter2->c_str(),tIter->c_str()) )
			{
				strOutName = m_strVec2.front();
				return true;
			}
		}
	}

	return false;
}

//取得结构所占用的大小
int		stJsonAttribDesc::GetSize()
{
	int nSize = sizeof(m_Type);

	nSize += sizeof(int);
	nSize += m_Name.size();

	nSize += sizeof(int);
	nSize += m_DefaultValue.size();

	return nSize;
}

//查找类型描述
int		stJsonObjectDesc::FindAttribDesc(string& strName,int& nNameIndex)
{
	vector<stJsonAttribDesc>::iterator tIter;


	//=====================================================
	vector<string> m_strVec2;
	const	char*	pChar2 = strName.c_str();
	int		nLen2 = strName.size();
	char*   szMemory2 = new char[nLen2+1];
	strcpy(szMemory2,pChar2);
	char*   szFindStr2 = szMemory2 ;
	szFindStr2 = strtok(szMemory2,",");
	while(szFindStr2)
	{
		string str = szFindStr2 ;
		m_strVec2.push_back(str);

		szFindStr2 = strtok(NULL,",");
		if(!szFindStr2)break;
	}
	delete[] szMemory2;

	nNameIndex = 0;
	vector<string>::iterator tStrIter;
	for(tStrIter = m_strVec2.begin();tStrIter != m_strVec2.end(); tStrIter++)
	{
		for(tIter = m_AttribVec.begin();tIter != m_AttribVec.end(); tIter++)
		{
			if( tIter->CompareAttribDesc(*tStrIter,strName) )
			{
				return (tIter - m_AttribVec.begin());
			}
		}
		nNameIndex++;
	}
	return -1;
}

//构造
stJsonNode::stJsonNode()
{
	m_Type		= kNullType ;
	m_ObjIndex	= -1;
	m_AttribIndex = -1;
	m_lChildMemAddr = 0;
}	

//打印
void	stJsonNode::Print(void*	pFileName,int vLayer,bool bEndNode,bool bParentNodeIsArray)
{
	FILE*	pFile = NULL;
	if(pFileName)
	{
		if(vLayer == 0)
		{
			pFile = fopen((const char*)pFileName,"wt");
			if(!pFile)return ;
			fprintf(pFile,"{\n");
		}
		else
		{
			pFile = (FILE*)pFileName;
		}
	}


	if(pFile)
	{
		for(int i = 0 ; i < vLayer ; i++)
		{
			fprintf(pFile," ");
		}
	}
	else
	{	
		for(int i = 0 ; i < vLayer ; i++)
		{
			printf(" ");
		}

	}

	bool bThieNodeIsArray = false;

	switch(m_Type)
	{
	case kNullType:
		{
			if(m_Name.size() > 0)
			{
				if(pFile)
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\":null\n",m_Name.c_str());
					}
					else
					{
						fprintf(pFile,"\"%s\":null,\n",m_Name.c_str());
					}
				}
				else
				{
					printf("%s:null\n",m_Name.c_str());
				}

			}

		}
		break;
	case kFalseType:
		{
			if(pFile)
			{
				if(bEndNode)
				{
					fprintf(pFile,"\"%s\":false\n",m_Name.c_str());
				}
				else
				{
					fprintf(pFile,"\"%s\":false,\n",m_Name.c_str());
				}

			}
			else
			{
				printf("%s:false\n",m_Name.c_str());
			}
		}
		break;
	case kTrueType:
		{
			if(pFile)
			{
				if(bEndNode)
				{
					fprintf(pFile,"\"%s\":true\n",m_Name.c_str());
				}
				else
				{
					fprintf(pFile,"\"%s\":true,\n",m_Name.c_str());
				}

			}
			else
			{
				printf("%s:true\n",m_Name.c_str());
			}

		}
		break;
	case kObjectType:
		{
			if(pFile)
			{
				if(false == m_Name.empty() && false == bParentNodeIsArray)
				{
					fprintf(pFile,"\"%s\":\n",m_Name.c_str());
				}

				vLayer++;
				for(int i = 0 ; i < vLayer; i++)
				{
					fprintf(pFile," ");
				}
				fprintf(pFile,"{\n");
			}
			else
			{
				printf("%s:\n",m_Name.c_str());

				vLayer++;
				for(int i = 0 ; i < vLayer; i++)
				{
					printf(" ");
				}
				printf("{\n");
			}

		}
		break;
	case kArrayType:
		{
			if(pFile)
			{
				if(false == m_Name.empty())
				{
					fprintf(pFile,"\"%s\":[\n",m_Name.c_str());
				}
				vLayer++;

			}
			else
			{
				printf("%s:\n",m_Name.c_str());
				vLayer++;
				for(int i = 0 ; i < vLayer; i++)
				{
					printf(" ");
				}
				printf("[\n");
			}

			bThieNodeIsArray = true;
		}
		break;
	case kStringType:
		{
			string strResult = m_Value;
			string_replace(strResult,"\\","\\\\");

			if(pFile)
			{
				if(false == m_Name.empty())
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\":\"%s\"\n",m_Name.c_str(),strResult.c_str());
					}
					else
					{
						fprintf(pFile,"\"%s\":\"%s\",\n",m_Name.c_str(),strResult.c_str());
					}
				}
				else
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\"\n",strResult.c_str());
					}
					else
					{
						fprintf(pFile,"\"%s\",\n",strResult.c_str());
					}
				}
				

			}
			else
			{
				printf("%s:String:%s\n",m_Name.c_str(),strResult.c_str());
			}

		}
		break;
	case kNumberType:
		{
			if(pFile)
			{
				if(false == m_Name.empty())
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\":%s\n",m_Name.c_str(),m_Value.c_str());
					}
					else
					{
						fprintf(pFile,"\"%s\":%s,\n",m_Name.c_str(),m_Value.c_str());
					}
				}
				else
				{
					if(bEndNode)
					{
						fprintf(pFile,"%s\n",m_Value.c_str());
					}
					else
					{
						fprintf(pFile,"%s,\n",m_Value.c_str());
					}
				}

			}
			else
			{
				printf("%s:Number:%s\n",m_Name.c_str(),m_Value.c_str());
			}

		}
		break;
	}



	vector<stJsonNode>::iterator tIter;
	for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
	{
		if(tIter == m_ChildVec.end()-1)
		{
			tIter->Print(pFile,vLayer+1,true,bThieNodeIsArray);
		}
		else
		{
			tIter->Print(pFile,vLayer+1,false,bThieNodeIsArray);
		}

	}

	switch(m_Type)
	{
	case kObjectType:
		{
			if(pFile)
			{
				for(int i = 0 ; i < vLayer; i++)
				{
					fprintf(pFile," ");
				}
				if(bEndNode)
				{
					fprintf(pFile,"}\n");
				}
				else
				{
					fprintf(pFile,"},\n");
				}

			}
			else
			{
				for(int i = 0 ; i < vLayer; i++)
				{
					printf(" ");
				}
				printf("}\n");
			}

		}
		break;
	case kArrayType:
		{
			if(pFile)
			{
				for(int i = 0 ; i < vLayer; i++)
				{
					fprintf(pFile," ");
				}
				if(bEndNode)
				{
					fprintf(pFile,"]\n");
				}
				else
				{
					fprintf(pFile,"],\n");
				}

			}
			else
			{
				for(int i = 0 ; i < vLayer; i++)
				{
					printf(" ");
				}
				printf("]\n");
			}

		}
		break;
	}

	if(vLayer == 0&&pFile)
	{
		fprintf(pFile,"}\n");
		fclose(pFile);
/*
		char szFileName[_MAX_PATH];
		sprintf(szFileName,"%s.utf8",pFileName);
		if(true == ConvAsciiTxtToUTF8((const char*)pFileName,szFileName))
		{
			DeleteFile((const char*)pFileName);
			rename(szFileName,(const char*)pFileName);
		}
*/
	}
}

//从外部结点中取得与当前匹配的节点
stJsonNode	stJsonNode::GetGoodJsonNode(JsonLoader*		pJson,bool bIsRoot,const char* szParentArrayName)
{
	stJsonNode	tNewJsonNode;
	tNewJsonNode.m_Type  = m_Type;
	tNewJsonNode.m_Name  = m_Name;
	tNewJsonNode.m_Value = m_Value;
	tNewJsonNode.m_ObjIndex = -1;
	tNewJsonNode.m_AttribIndex = -1;
	if(false == bIsRoot)
	{
		tNewJsonNode.m_ObjIndex  = GetObjectIndex(pJson,m_ChildVec);
		
		vector<stJsonNode>::iterator tIter;
		/*for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		{
			if(0 == strcmp(tIter->m_Name.c_str(),"classname"))
			{
				tNewJsonNode.m_ObjIndex  = pJson->GetObjectDescIndex(tIter->m_Value);
				break;
			}
			if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
			{
				tNewJsonNode.m_ObjIndex  = pJson->GetObjectDescIndex(tIter->m_Value);
				break;
			}
		}
		*/
		if( tNewJsonNode.m_ObjIndex  < 0 && kObjectType == m_Type)
		{
			if(m_Name.size() > 0)
			{
				tNewJsonNode.m_ObjIndex  = pJson->GetObjectDescIndex(m_Name);
				tNewJsonNode.m_Name  = m_Name;
			}
			else if(szParentArrayName)
			{
				string strParentArrayName = szParentArrayName;
				tNewJsonNode.m_ObjIndex  = pJson->GetObjectDescIndex(strParentArrayName);
				tNewJsonNode.m_Name  = szParentArrayName;
			}

		}
		if( tNewJsonNode.m_ObjIndex  >= 0 )
		{
			
			stJsonObjectDesc&	vObjectDesc = pJson->GetObjectDesc(tNewJsonNode.m_ObjIndex );


			for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
			{

				int nNameIndex = 0;
				int	nIndex = vObjectDesc.FindAttribDesc(tIter->m_Name,nNameIndex);
				if(nIndex >= 0)
				{

					bool bComType = (tIter->m_Type == vObjectDesc.m_AttribVec[nIndex].m_Type)?true:false;

					if(tIter->m_Type == kFalseType && vObjectDesc.m_AttribVec[nIndex].m_Type == kTrueType)bComType = true;
					if(tIter->m_Type == kTrueType && vObjectDesc.m_AttribVec[nIndex].m_Type == kFalseType)bComType = true;
					if(tIter->m_Type == kNullType && vObjectDesc.m_AttribVec[nIndex].m_Type == kStringType )
					{
						tIter->m_Type = kStringType;
						tIter->m_Value="null";
						bComType = true;
					}
					if(tIter->m_Type == kObjectType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType)bComType = true;
					if(tIter->m_Type == kArrayType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType)bComType = true;

					//if(tIter->m_Type == kStringType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType )bComType = true;
					if(true == bComType)
					{

						if(tIter->m_Type == kObjectType)
						{
							stJsonNode	tNewObjectJsonNode = tIter->GetGoodJsonNode(pJson,false);
							tNewJsonNode.m_ChildVec.push_back(tNewObjectJsonNode);
							continue ;

						}
						if(tIter->m_Type == kArrayType)
						{
							stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
							tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
							continue ;
						}

						stJsonNode	tNewChildJsonNode;
						tNewChildJsonNode.m_Type  = tIter->m_Type;
						tNewChildJsonNode.m_Name  = tIter->m_Name;
						tNewChildJsonNode.m_Value = tIter->m_Value;

					
						if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
						{
							if(1 < vObjectDesc.m_NameVec.size())
							{
								tNewChildJsonNode.m_Value  = vObjectDesc.m_NameVec[nNameIndex];
							}
						}
						if(0 == strcmp(tIter->m_Name.c_str(),"version"))
						{
							
							if(kStringType == tIter->m_Type)
							{
								tNewChildJsonNode.m_Value = pJson->GetVersion();
							}
							else
							{	
								FLOAT fVersion = pJson->GetAniFloatVersion();
								char  szFloatVersion[10];
								if(int(fVersion) == fVersion)
								{
									sprintf(szFloatVersion,"%.2f",fVersion);
									tNewChildJsonNode.m_Value = szFloatVersion;
								}
								else
								{
									sprintf(szFloatVersion,"%.4f",fVersion);
									tNewChildJsonNode.m_Value = szFloatVersion;
								}
							}
						}
						else
						{
							if(0 == strcmp(tIter->m_Name.c_str(),"strVersion"))
							{
								tNewChildJsonNode.m_Value = pJson->GetAniVersion();
							}
							else
							{
								tNewChildJsonNode.m_Value = tIter->m_Value;
							}
						}
						tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
						tNewChildJsonNode.m_AttribIndex = nIndex;
						tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						continue ;
					}
					else if(0 == strcmp(tIter->m_Name.c_str(),"classname"))
					{
						stJsonNode	tNewChildJsonNode;
						tNewChildJsonNode.m_Type  = tIter->m_Type;
						tNewChildJsonNode.m_Name  = tIter->m_Name;
						tNewChildJsonNode.m_Value = tIter->m_Value;
						tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
						tNewChildJsonNode.m_AttribIndex = nIndex;
						tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						continue ;
					}
					else if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
					{
						stJsonNode	tNewChildJsonNode;
						tNewChildJsonNode.m_Type  = tIter->m_Type;
						tNewChildJsonNode.m_Name  = tIter->m_Name;
						tNewChildJsonNode.m_Value = tIter->m_Value;
						tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
						tNewChildJsonNode.m_AttribIndex = nIndex;
						tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						continue ;
					}
					else if(tIter->m_Type == kNullType && (vObjectDesc.m_AttribVec[nIndex].m_Type == kObjectType||vObjectDesc.m_AttribVec[nIndex].m_Type == kArrayType))
					{
						stJsonNode	tNewChildJsonNode;
						tNewChildJsonNode.m_Type  = tIter->m_Type;
						tNewChildJsonNode.m_Name  = tIter->m_Name;
						tNewChildJsonNode.m_Value = tIter->m_Value;
						tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
						tNewChildJsonNode.m_AttribIndex = nIndex;
						tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						continue ;
					}
				}
				else if(0 == strcmp(tIter->m_Name.c_str(),"children"))
				{
					stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
					tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
					continue ;
				}
				else if(tIter->m_Type == kObjectType)
				{
					int	nAttribIndex = pJson->GetObjectDescIndex(tIter->m_Name);
					if( nAttribIndex >= 0 )
					{
						stJsonNode	tNewObjectJsonNode = tIter->GetGoodJsonNode(pJson,false);
						tNewJsonNode.m_ChildVec.push_back(tNewObjectJsonNode);
						continue ;
					}
				}
			}
			//将缺失的新属性补上
			vector<stJsonAttribDesc>::iterator tIter2;
			for(tIter2 = vObjectDesc.m_AttribVec.begin(); tIter2 != vObjectDesc.m_AttribVec.end(); tIter2++)
			{
				bool bFind = false;
				vector<stJsonNode>::iterator tIter3;
				for(tIter3 = m_ChildVec.begin(); tIter3 != m_ChildVec.end(); tIter3++)
				{
					string strOutName ;
					if(tIter2->CompareAttribDesc(tIter3->m_Name,strOutName))
					{
						bFind = true;
						break;
					}
				}

				if(false == bFind)
				{
					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter2->m_Type;
					tNewChildJsonNode.m_Name  = tIter2->m_Name;
					tNewChildJsonNode.m_Value = tIter2->m_DefaultValue;
					tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
					tNewChildJsonNode.m_AttribIndex = int(tIter2 - vObjectDesc.m_AttribVec.begin());
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
				}
			}

		}
		//else if(0 == strcmp(m_Name.c_str(),"children"))
		//{
		//	for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		//	{
		//		if(tIter->m_Type == kObjectType)
		//		{
		//			stJsonNode	tNewObjectJsonNode = tIter->GetGoodJsonNode(pJson,false);
		//			tNewJsonNode.m_ChildVec.push_back(tNewObjectJsonNode);
		//			continue ;

		//		}
		//		if(tIter->m_Type == kArrayType)
		//		{
		//			stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
		//			tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
		//			continue ;
		//		}
		//	}
		//}
		else if(m_Type == kArrayType)
		{
			for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
			{
				if(tIter->m_Type == kObjectType)
				{
					stJsonNode	tNewObjectJsonNode = tIter->GetGoodJsonNode(pJson,false,m_Name.c_str());
					tNewJsonNode.m_ChildVec.push_back(tNewObjectJsonNode);
					continue ;

				}
				if(tIter->m_Type == kArrayType)
				{
					stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
					tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
					continue ;
				}

				stJsonNode	tNewChildJsonNode;
				tNewChildJsonNode.m_Type  = tIter->m_Type;
				tNewChildJsonNode.m_Name  = tIter->m_Name;
				tNewChildJsonNode.m_Value = tIter->m_Value;
				tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
				tNewChildJsonNode.m_AttribIndex = int(tIter - m_ChildVec.begin());
				tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
				continue ;
			}
		}
	}
	else
	{
		tNewJsonNode.m_ObjIndex = 0;
		stJsonObjectDesc&	vObjectDesc = pJson->GetObjectDesc(tNewJsonNode.m_ObjIndex);

		vector<stJsonNode>::iterator tIter;
		for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		{

			int nNameIndex = 0;
			int	nIndex = vObjectDesc.FindAttribDesc(tIter->m_Name,nNameIndex);
			if(nIndex >= 0)
			{			
				bool bComType = (tIter->m_Type == vObjectDesc.m_AttribVec[nIndex].m_Type)?true:false;
				if(tIter->m_Type == kFalseType && vObjectDesc.m_AttribVec[nIndex].m_Type == kTrueType)bComType = true;
				if(tIter->m_Type == kTrueType && vObjectDesc.m_AttribVec[nIndex].m_Type == kFalseType)bComType = true;
				if(tIter->m_Type == kNullType && vObjectDesc.m_AttribVec[nIndex].m_Type == kStringType )
				{
					tIter->m_Type = kStringType;
					tIter->m_Value="null";
					bComType = true;
				}
				if(tIter->m_Type == kObjectType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType)bComType = true;
				if(tIter->m_Type == kArrayType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType)bComType = true;

				//if(tIter->m_Type == kStringType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType )bComType = true;
				if(bComType)
				{
					if(tIter->m_Type == kObjectType)
					{
						stJsonNode	tNewObjectJsonNode = tIter->GetGoodJsonNode(pJson,false);
						tNewJsonNode.m_ChildVec.push_back(tNewObjectJsonNode);
						continue ;

					}
					if(tIter->m_Type == kArrayType)
					{
						stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
						tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
						continue ;
					}

					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter->m_Type;
					tNewChildJsonNode.m_Name  = tIter->m_Name;

					if(0 == strcmp(tIter->m_Name.c_str(),"version"))
					{
						tNewChildJsonNode.m_Value = pJson->GetVersion();
					}
					else
					{
						if(0 == strcmp(tIter->m_Name.c_str(),"strVersion"))
						{
							tNewChildJsonNode.m_Value = pJson->GetAniVersion();
						}
						else
						{
							tNewChildJsonNode.m_Value = tIter->m_Value;
						}
					}


					if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
					{
						if(1 < vObjectDesc.m_NameVec.size())
						{
							tNewChildJsonNode.m_Value  = vObjectDesc.m_NameVec[nNameIndex];
						}
					}

					tNewChildJsonNode.m_ObjIndex = 0;
					tNewChildJsonNode.m_AttribIndex = nIndex;
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
					continue ;
				}
				else if(0 == strcmp(tIter->m_Name.c_str(),"classname"))
				{
					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter->m_Type;
					tNewChildJsonNode.m_Name  = tIter->m_Name;
					tNewChildJsonNode.m_Value = tIter->m_Value;
					tNewChildJsonNode.m_ObjIndex = 0;
					tNewChildJsonNode.m_AttribIndex = nIndex;
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
					continue ;
				}
				else if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
				{
					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter->m_Type;
					tNewChildJsonNode.m_Name  = tIter->m_Name;
					tNewChildJsonNode.m_Value = tIter->m_Value;
					tNewChildJsonNode.m_ObjIndex = 0;
					tNewChildJsonNode.m_AttribIndex = nIndex;
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
					continue ;
				}
				else if(tIter->m_Type == kNullType && (vObjectDesc.m_AttribVec[nIndex].m_Type == kObjectType||vObjectDesc.m_AttribVec[nIndex].m_Type == kArrayType))
				{
					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter->m_Type;
					tNewChildJsonNode.m_Name  = tIter->m_Name;
					tNewChildJsonNode.m_Value = tIter->m_Value;
					tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
					tNewChildJsonNode.m_AttribIndex = nIndex;
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
					continue ;
				}
			}
			else if(0 == strcmp(tIter->m_Name.c_str(),"children"))
			{
				stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
				tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
				continue ;
			}
		}

		//将缺失的新属性补上
		vector<stJsonAttribDesc>::iterator tIter2;
		for(tIter2 = vObjectDesc.m_AttribVec.begin(); tIter2 != vObjectDesc.m_AttribVec.end(); tIter2++)
		{
			bool bFind = false;
			vector<stJsonNode>::iterator tIter3;
			for(tIter3 = m_ChildVec.begin(); tIter3 != m_ChildVec.end(); tIter3++)
			{
				string strOutName ;
				if(tIter2->CompareAttribDesc(tIter3->m_Name,strOutName))
				{
					bFind = true;
					break;
				}
			}

			if(false == bFind)
			{
				stJsonNode	tNewChildJsonNode;
				tNewChildJsonNode.m_Type  = tIter2->m_Type;
				tNewChildJsonNode.m_Name  = tIter2->m_Name;
				tNewChildJsonNode.m_Value = tIter2->m_DefaultValue;
				tNewChildJsonNode.m_ObjIndex = 0;
				tNewChildJsonNode.m_AttribIndex = tIter2 - vObjectDesc.m_AttribVec.begin();
				tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
			}
		}

	}

	

	return tNewJsonNode ;
}

//查找子结点
int		stJsonNode::FindChild(const char* szName)
{
	int nIndex = 0;
	vector<stJsonNode>::iterator tIter;
	for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
	{
		if( 0 == strcmp ( tIter->m_Name.c_str() , szName ))
		{
			return nIndex ;
		}
		nIndex++;
	}
	return -1;
}

//取得所有的子结点，孙节点
int		stJsonNode::GetAllJsonNodes(vector<stJsonNode*>&  vAllChildVec)
{
	int nChildCount = 0;
	//所有的
	if(false == m_ChildVec.empty())
	{
		nChildCount += m_ChildVec.size();
		vector<stJsonNode>::iterator tIter;
		for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		{
			if(false == tIter->m_ChildVec.empty())
			{
				vAllChildVec.push_back(&(*tIter));
			}
		}

		for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		{
			nChildCount += tIter->GetAllJsonNodes(vAllChildVec);
		}
	}

	return nChildCount;
}

//构造
JsonLoader::JsonLoader()
{
	m_strVersion = "";
	m_AniVersion = "";
	m_fAniVersion = - 1.0f;
}
//析构
JsonLoader::~JsonLoader()
{

}

//加载
bool	JsonLoader::ReadCocoJsonBuff(const char* pJsonBuff)
{
	std::string clearData(pJsonBuff);
	size_t pos = clearData.rfind("}");
	clearData = clearData.substr(0, pos+1);
	jsonDict.Clear();
	jsonDict.Parse<0>(clearData.c_str());
	if(jsonDict.HasParseError())
	{
		printf("GetParseError : %s\n",jsonDict.GetParseError());
		return false;
	}

	stJsonObjectDesc		tNewRootObject;

	for (rapidjson::Document::ConstMemberIterator itr = jsonDict.MemberonBegin(); itr != jsonDict.MemberonEnd(); ++itr)
	{
		const char*	szName  = itr->name.GetString();
		string	strName = szName;

		stJsonAttribDesc	tJsonAttrib;
		tJsonAttrib.m_Name = strName ;
		tJsonAttrib.m_Type = itr->value.GetType();
		tNewRootObject.m_AttribVec.push_back(tJsonAttrib);

		if(itr->value.IsNull())
		{
			AddJsonNode(kNullType,strName,"",m_RootNode);
			continue ;
		}
		if(itr->value.IsArray())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kArrayType,strName,"",m_RootNode);
			for (rapidjson::SizeType i = 0; i < itr->value.Size(); i++)
			{
				const rapidjson::Value &val = itr->value[i]; 
				AddJsonNode_ArrayItem(val,tNewJsonNode);
			}
			continue ;
		}
		if(itr->value.IsBool())
		{
			if(itr->value.IsFalse())
			{
				AddJsonNode(kFalseType,strName,"0",m_RootNode);
			}
			if(itr->value.IsTrue())
			{
				AddJsonNode(kTrueType,strName,"1",m_RootNode);
			}
			continue ;
		}

		if(itr->value.IsNumber())
		{
			
			if(itr->value.IsInt())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetInt());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue;
			}
			if(itr->value.IsInt64())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetInt64());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue;
			}
			if(itr->value.IsUint())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetUint());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue ;
			}
			if(itr->value.IsUint64())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetUint64());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue ;
			}
			if(itr->value.IsDouble())
			{
				char  szText[100];
				sprintf(szText,"%.4f",itr->value.GetDouble());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue ;
			}
			continue ;
		}
		if(itr->value.IsObject())
		{
			//根结点
			stJsonNode&	tNewJsonNode = AddJsonNode(kObjectType,strName,"",m_RootNode);
			AddJsonNode_Object(itr->value,tNewJsonNode);
			continue ;
		}
		if(itr->value.IsString())
		{
			string	strText = itr->value.GetString() ;
			if(0 == strcmp(strName.c_str(),"version"))
			{
				m_strVersion = strText;
			}
			if(0 == strcmp(strName.c_str(),"strVersion"))
			{
				m_AniVersion = strText;
			}
			AddJsonNode(kStringType,strName,strText,m_RootNode);
			continue ;
		}

	}

	if(m_ObjectDescVec.empty())
	{
		m_ObjectDescVec.push_back(tNewRootObject);
	}
	else
	{
		m_ObjectDescVec.insert(m_ObjectDescVec.begin(),tNewRootObject);
	}

	return true;
}
//打印JSON
void	JsonLoader::PrintAllJsonNode()
{
	m_RootNode.Print();
}	

//取得根结点
stJsonNode&	JsonLoader::GetRootJsonNode()
{
	return m_RootNode;
}
//增加一个结点
stJsonNode&		JsonLoader::AddJsonNode(Type vType,string strName,string strValue,stJsonNode& vParentNode)
{
	stJsonNode	tNewJsonNode;
	tNewJsonNode.m_Type = vType ;
	tNewJsonNode.m_Name = strName ;
	tNewJsonNode.m_Value= strValue ;
	tNewJsonNode.m_ObjIndex = -1;
    tNewJsonNode.m_AttribIndex = -1;
	vParentNode.m_ChildVec.push_back(tNewJsonNode);
	return vParentNode.m_ChildVec.back();
}
//增加一个结点
stJsonNode&		JsonLoader::AddJsonNode_ArrayItem(const rapidjson::Value &	vValue,stJsonNode& vParentNode)
{
	if(vValue.IsNull())
	{
		return AddJsonNode(kNullType,"","",vParentNode);
	}
	if(vValue.IsArray())
	{
		stJsonNode&	tNewJsonNode = AddJsonNode(kArrayType,"","",vParentNode);
		for (rapidjson::SizeType i = 0; i < vValue.Size(); i++)
		{
			const rapidjson::Value &val = vValue[i]; 
			AddJsonNode_ArrayItem(val,tNewJsonNode);
		}
		return tNewJsonNode;
	}
	if(vValue.IsBool())
	{
		if(vValue.IsFalse())
		{
			return AddJsonNode(kFalseType,"","0",vParentNode);
		}

		return	AddJsonNode(kTrueType,"","1",vParentNode);

	}

	if(vValue.IsNumber())
	{
		
		if(vValue.IsInt())
		{
			char  szText[100];
			sprintf(szText,"%d",vValue.GetInt());
			string	strText = szText ;
			return AddJsonNode(kNumberType,"",strText,vParentNode);
		}
		if(vValue.IsInt64())
		{
			char  szText[100];
			sprintf(szText,"%d",vValue.GetInt64());
			string	strText = szText ;
			return AddJsonNode(kNumberType,"",strText,vParentNode);
		}
		if(vValue.IsUint())
		{
			char  szText[100];
			sprintf(szText,"%d",vValue.GetUint());
			string	strText = szText ;
			return AddJsonNode(kNumberType,"",strText,vParentNode);
		}
		if(vValue.IsUint64())
		{
			char  szText[100];
			sprintf(szText,"%d",vValue.GetUint64());
			string	strText = szText ;
			return AddJsonNode(kNumberType,"",strText,vParentNode);
		}

		char  szText[100];
		sprintf(szText,"%.4f",vValue.GetDouble());
		string	strText = szText ;
		return AddJsonNode(kNumberType,"",strText,vParentNode);
	}
	if(vValue.IsObject())
	{
		stJsonNode&	tNewJsonNode = AddJsonNode(kObjectType,"","",vParentNode);
		//在数组中
		if(tNewJsonNode.m_Name.size() == 0)
		{
			AddObjectDesc(vParentNode.m_Name,vValue);
			tNewJsonNode.m_Name = vParentNode.m_Name;
		}
		AddJsonNode_Object(vValue,tNewJsonNode);
		return tNewJsonNode;
	}

	string	strText = vValue.GetString() ;
	return	AddJsonNode(kStringType,"",strText,vParentNode);
}
//增加一个结点
stJsonNode&		JsonLoader::AddJsonNode_Object(const rapidjson::Value &	vValue,stJsonNode& vParentNode)
{
	bool  bHasObjectDesc = false;

	for (rapidjson::Value::ConstMemberIterator itr = vValue.MemberonBegin(); itr != vValue.MemberonEnd(); ++itr)
	{
		const char*	szName  = itr->name.GetString();
		string	strName = szName;
		if(itr->value.IsNull())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kNullType,strName,"",vParentNode);

			if(false == bHasObjectDesc && 0 == strcmp(szName,"classname") )
			{
				if(vParentNode.m_Name.size() == 0)
				{
					AddObjectDesc("",vValue);
					bHasObjectDesc = true;
				}
				else
				{
					AddObjectDesc(vParentNode.m_Name,vValue);
					bHasObjectDesc = true;
				}

			}
			continue ;
		}
		if(itr->value.IsArray())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kArrayType,strName,"",vParentNode);
			for (rapidjson::SizeType i = 0; i < itr->value.Size(); i++)
			{
				const rapidjson::Value &val = itr->value[i]; 
				AddJsonNode_ArrayItem(val,tNewJsonNode);
			}
			continue ;
		}
		if(itr->value.IsBool())
		{
			if(itr->value.IsFalse())
			{
				AddJsonNode(kFalseType,strName,"0",vParentNode);
			}
			if(itr->value.IsTrue())
			{
				AddJsonNode(kTrueType,strName,"1",vParentNode);
			}
			continue ;
		}

		if(itr->value.IsNumber())
		{

			if(itr->value.IsInt())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetInt());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue;
			}
			if(itr->value.IsInt64())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetInt64());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue;
			}
			if(itr->value.IsUint())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetUint());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue ;
			}
			if(itr->value.IsUint64())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetUint64());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue ;
			}
			if(itr->value.IsDouble())
			{
				char  szText[100];
				sprintf(szText,"%.4f",itr->value.GetDouble());

				if(0 == strcmp(strName.c_str(),"version"))
				{
					m_fAniVersion = atof(szText);
				}

				string	strText = szText ;		
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue ;
			}
			continue ;
		}
		if(itr->value.IsObject())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kObjectType,strName,"",vParentNode);
			AddJsonNode_Object(itr->value,tNewJsonNode);
			continue ;
		}
		if(itr->value.IsString())
		{
			string	strText = itr->value.GetString() ;
			stJsonNode&	tNewJsonNode = AddJsonNode(kStringType,strName,strText,vParentNode);
			if(false == bHasObjectDesc)
			{
				if(0 == strcmp(szName,"classname"))
				{
					AddObjectDesc(strText,vValue);
					bHasObjectDesc = true;
				}
				if(0 == strcmp(szName,"__type"))
				{
					AddObjectDesc(strText,vValue);
					bHasObjectDesc = true;
				}
			}
			if(0 == strcmp(strName.c_str(),"strVersion"))
			{
				m_AniVersion = strText;
			}
			continue ;
		}
	}
	if(false == bHasObjectDesc && vParentNode.m_Name.size() > 0)
	{
		AddObjectDesc(vParentNode.m_Name,vValue);
		bHasObjectDesc = true;
	}

	return vParentNode;
}

//记录一下类结点
void	JsonLoader::AddObjectDesc(string strName,const rapidjson::Value &	vValue)
{
	//类结点容器
	vector<stJsonObjectDesc>::iterator tIter;
	for(tIter = m_ObjectDescVec.begin() ; tIter != m_ObjectDescVec.end() ;  tIter++)
	{
		vector<string>::iterator tIter2;
		for(tIter2 = tIter->m_NameVec.begin() ; tIter2 != tIter->m_NameVec.end() ;  tIter2++)
		{
			if(0 == strcmp(tIter2->c_str(),strName.c_str()))
			{

				unsigned int nSize = vValue.MemberonEnd() - vValue.MemberonBegin();
				unsigned int nAttribSize = tIter->m_AttribVec.size();

				for (rapidjson::Value::ConstMemberIterator itr = vValue.MemberonBegin(); itr != vValue.MemberonEnd(); ++itr)
				{
					const char*	szAttribName  = itr->name.GetString();
					vector<stJsonAttribDesc>::iterator tAttribIter;
					bool bFind = false;
					for(tAttribIter = tIter->m_AttribVec.begin();tAttribIter != tIter->m_AttribVec.end(); tAttribIter++)
					{
						if( 0 == strcmp(szAttribName,tAttribIter->m_Name.c_str()))
						{
							/*
							if(tAttribIter->m_Type != itr->value.GetType())
							{
							tAttribIter->m_Type = itr->value.GetType();
							}
							*/
							bFind = true;
							break;
						}
					}

					if(false == bFind)
					{
						stJsonAttribDesc tAttribDesc;
						tAttribDesc.m_Name = szAttribName ;
						tAttribDesc.m_DefaultValue = "";
						tAttribDesc.m_Type = itr->value.GetType();
						tIter->m_AttribVec.push_back(tAttribDesc);

						if( tAttribDesc.m_Type == kObjectType )
						{
							AddObjectDesc(tAttribDesc.m_Name,itr->value);
						}

						if( tAttribDesc.m_Type == kArrayType && itr->value.Empty() == false)
						{
							rapidjson::SizeType i = 0;
							const rapidjson::Value &val = itr->value[i]; 
							if( val.GetType() == kObjectType )
							{
								AddObjectDesc(tAttribDesc.m_Name,val);
							}
						}
					}
				}	

				return ;

			}
		}
	}

	if(strName.empty() == false)
	{
		const	char*	pChar = strName.c_str();
		int		nLen = strName.size();
		char*   szMemory = new char[nLen+1];
		strcpy(szMemory,pChar);
		char*   szFindStr = szMemory ;
		szFindStr = strtok(szFindStr,",");

		stJsonObjectDesc	tNewObject;
		while(szFindStr)
		{
			string strTempName = szFindStr ;
			tNewObject.m_NameVec.push_back(strTempName);

			if(tNewObject.m_AttribVec.empty())
			{
				for (rapidjson::Value::ConstMemberIterator itr = vValue.MemberonBegin(); itr != vValue.MemberonEnd(); ++itr)
				{
					const char*	szAttribName  = itr->name.GetString();
					/*
					if(0 == strcmp(szAttribName,"children"))
					{
					continue ;
					}
					*/
					string	strAttribName = szAttribName;
					stJsonAttribDesc	tJsonAttrib;
					tJsonAttrib.m_Name = strAttribName ;
					tJsonAttrib.m_Type = itr->value.GetType();
					tJsonAttrib.m_DefaultValue = GetValueString(itr->value);
					tNewObject.m_AttribVec.push_back(tJsonAttrib);
				}
			}
			
			
			szFindStr = strtok(NULL,",");
			if(!szFindStr)break;
		}
		m_ObjectDescVec.push_back(tNewObject);
		delete[] szMemory;
	}

}
//查询相应的类描述，返回索引
int		JsonLoader::GetObjectDescIndex(string& strName)
{

	const	char*	pChar = strName.c_str();
	int		nLen = strName.size();
	char*   szMemory = new char[nLen+1];
	strcpy(szMemory,pChar);
	char*   szFindStr = szMemory ;
	szFindStr = strtok(szFindStr,",");
	while(szFindStr)
	{
		//类结点容器
		vector<stJsonObjectDesc>::iterator tIter;
		for(tIter = m_ObjectDescVec.begin() ; tIter != m_ObjectDescVec.end() ;  tIter++)
		{
			vector<string>::iterator tIter2;
			for(tIter2 = tIter->m_NameVec.begin() ; tIter2 != tIter->m_NameVec.end() ;  tIter2++)
			{
				if(0 == strcmp(tIter2->c_str(),szFindStr))
				{
					strName = tIter->m_NameVec.front();
					delete[] szMemory;
					return int(tIter - m_ObjectDescVec.begin());
				}
			}
		}

		szFindStr = strtok(NULL,",");
		if(!szFindStr)break;
	}
	delete[] szMemory;
	return -1;
}
//取得相应的类描述
stJsonObjectDesc&	JsonLoader::GetObjectDesc(int vIndex)
{
	return m_ObjectDescVec[vIndex];
}
//取得导出时计算的将使用的类索引
int		JsonLoader::GetUsedObjectDescIndex(int vIndex)
{
	//类结点容器
	int  nObjectIndex = -1;
	int  nUsedObjectIndex = -1;
	vector<stJsonObjectDesc>::iterator tIter;
	for(tIter = m_ObjectDescVec.begin() ; tIter != m_ObjectDescVec.end() ;  tIter++)
	{
		if(tIter->m_HasNode)
		{
			nUsedObjectIndex++;
		}

		nObjectIndex++;

		if(vIndex == nObjectIndex)
		{
			return nUsedObjectIndex;
		}
	}

	return nUsedObjectIndex;
}
//取得一个值项的字符串
string	JsonLoader::GetValueString(const rapidjson::Value &	vValue)
{
	string	strResult = "";
	switch(vValue.GetType())
	{
	case kNullType:
		{
			strResult = "";
		}
		break;
	case kFalseType:
		{
			strResult = "0";
		}
		break;
	case kTrueType:
		{
			strResult = "1";
		}
		break;
	case kObjectType:
		{
			strResult = "";
		}
		break;
	case kArrayType:
		{
			strResult = "";
		}
		break;
	case kStringType:
		{
			strResult = vValue.GetString() ;
		}
		break;
	case kNumberType:
		{
			if(vValue.IsInt())
			{
				char  szText[100];
				sprintf(szText,"%d",vValue.GetInt());
				strResult = szText ;
			}
			if(vValue.IsInt64())
			{
				char  szText[100];
				sprintf(szText,"%d",vValue.GetInt64());
				strResult = szText ;
			}
			if(vValue.IsUint())
			{
				char  szText[100];
				sprintf(szText,"%d",vValue.GetUint());
				strResult = szText ;
			}
			if(vValue.IsUint64())
			{
				char  szText[100];
				sprintf(szText,"%d",vValue.GetUint64());
				strResult = szText ;
			}
			if(vValue.IsDouble())
			{
				char  szText[100];
				sprintf(szText,"%.4f",vValue.GetDouble());
				strResult = szText ;
			}
		}
		break;
	}
	return strResult;
}

