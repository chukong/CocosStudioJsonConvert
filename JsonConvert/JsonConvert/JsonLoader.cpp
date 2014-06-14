#include "JsonLoader.h"

const	char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
char	g_Buff[2048];
//输出到二进制文件
void	stJsonAttribDesc::WriteToFile(FILE* hFile)
{
	//Type
	fwrite(&m_Type,sizeof(Type),1,hFile);

	//Name
	int  nLen = m_Name.size();
	fwrite(&nLen,sizeof(int),1,hFile);
	fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);

	//DefaultValue
	nLen = m_DefaultValue.size();
	fwrite(&nLen,sizeof(int),1,hFile);
	fwrite(m_DefaultValue.c_str(),sizeof(char),nLen,hFile);
}
//从二进制文件中读取
char*	stJsonAttribDesc::ReadFromBuff(char* pBuff)
{
	//Type
	char*	pTempBuff = pBuff;
	memcpy(&m_Type,pTempBuff,sizeof(Type));
	pTempBuff += sizeof(Type);

	//Name
	int  nLen = 0;
	memcpy(&nLen,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);

	memcpy(g_Buff,pTempBuff,nLen);
	pTempBuff += nLen;
	g_Buff[nLen]='\0';

	m_Name = g_Buff;

	//DefaultValue
	memcpy(&nLen,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);

	memcpy(g_Buff,pTempBuff,nLen);
	pTempBuff += nLen;
	g_Buff[nLen]='\0';

	m_DefaultValue = g_Buff;

	return pTempBuff ;
}

//查找类型描述
int		stJsonObjectDesc::FindAttribDesc(string& strName)
{
	vector<stJsonAttribDesc>::iterator tIter;

	const	char*	pChar = strName.c_str();
	int		nLen = strName.size();
	char*   szMemory = new char[nLen+1];
	strcpy(szMemory,pChar);
	char*   szFindStr = szMemory ;
	szFindStr = strtok(szFindStr,",");
	while(szFindStr)
	{
		for(tIter = m_AttribVec.begin();tIter != m_AttribVec.end(); tIter++)
		{
			if( 0 == strcmp(tIter->m_Name.c_str(),szFindStr) )
			{
				strName = szFindStr;
				delete[] szMemory;
				return (tIter - m_AttribVec.begin());
			}
		}
		szFindStr = strtok(NULL,",");
		if(!szFindStr)break;
	}
	delete[] szMemory;
	return -1;
}
//输出到二进制文件
void	stJsonObjectDesc::WriteToFile(FILE* hFile)
{
	//MD5
	fwrite(m_MD5,sizeof(m_MD5),1,hFile);
	//Name
	int  nLen = m_Name.size();
	fwrite(&nLen,sizeof(int),1,hFile);
	fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);

	int nAttribNum = m_AttribVec.size();
	fwrite(&nAttribNum,sizeof(int),1,hFile);
	for(int i = 0 ; i < nAttribNum ; i++)
	{
		m_AttribVec[i].WriteToFile(hFile);
	}
}
//从二进制文件中读取
char*	stJsonObjectDesc::ReadFromBuff(char* pBuff)
{
	//MD5
	char*	pTempBuff = pBuff;
	memcpy(m_MD5,pTempBuff,sizeof(m_MD5));
	pTempBuff += sizeof(m_MD5);

	//Name
	int  nLen = 0;
	memcpy(&nLen,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);

	memcpy(g_Buff,pTempBuff,nLen);
	pTempBuff += nLen;
	g_Buff[nLen]='\0';

	m_Name = g_Buff;

	int nAttribNum = 0;
	memcpy(&nAttribNum,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);
	for(int i = 0 ; i < nAttribNum ; i++)
	{
		stJsonAttribDesc	tAttribDesc;
		pTempBuff = tAttribDesc.ReadFromBuff(pTempBuff);
		m_AttribVec.push_back(tAttribDesc);
	}	

	return pTempBuff;
}
//构造
stJsonNode::stJsonNode()
{
	m_Type = kNullType ;
}
//打印
void	stJsonNode::Print(void*	pFileName,int vLayer,bool bEndNode)
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
				if(false == m_Name.empty())
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
					fprintf(pFile,"\"%s\":\n",m_Name.c_str());
				}
				vLayer++;
				for(int i = 0 ; i < vLayer; i++)
				{
					fprintf(pFile," ");
				}
				fprintf(pFile,"[\n");
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

		}
		break;
	case kStringType:
		{
			if(pFile)
			{
				if(false == m_Name.empty())
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\":\"%s\"\n",m_Name.c_str(),m_Value.c_str());
					}
					else
					{
						fprintf(pFile,"\"%s\":\"%s\",\n",m_Name.c_str(),m_Value.c_str());
					}
				}
				else
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\"\n",m_Value.c_str());
					}
					else
					{
						fprintf(pFile,"\"%s\",\n",m_Value.c_str());
					}
				}
				

			}
			else
			{
				printf("%s:String:%s\n",m_Name.c_str(),m_Value.c_str());
			}

		}
		break;
	case kNumberType:
		{
			if(pFile)
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
			tIter->Print(pFile,vLayer+1,true);
		}
		else
		{
			tIter->Print(pFile,vLayer+1);
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
	}
}

//从外部结点中取得与当前匹配的节点
stJsonNode	stJsonNode::GetGoodJsonNode(JsonLoader*		pJson,bool bIsRoot)
{
	stJsonNode	tNewJsonNode;
	tNewJsonNode.m_Type  = m_Type;
	tNewJsonNode.m_Name  = m_Name;
	tNewJsonNode.m_Value = m_Value;

	if(false == bIsRoot)
	{

		int	nObjectIndex = -1;
		vector<stJsonNode>::iterator tIter;
		for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		{
			if(0 == strcmp(tIter->m_Name.c_str(),"classname"))
			{
				nObjectIndex = pJson->GetObjectDescIndex(tIter->m_Value);
				break;
			}
			if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
			{
				nObjectIndex = pJson->GetObjectDescIndex(tIter->m_Value);
				break;
			}
		}
		if( nObjectIndex < 0 && kObjectType == m_Type)
		{
			if(m_Name.size() > 0)
			{
				nObjectIndex = pJson->GetObjectDescIndex(m_Name);
				tNewJsonNode.m_Name  = m_Name;
			}

		}
		if( nObjectIndex >= 0 )
		{
			stJsonObjectDesc&	vObjectDesc = pJson->GetObjectDesc(nObjectIndex);
			for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
			{
				int	nIndex = vObjectDesc.FindAttribDesc(tIter->m_Name);
				if(nIndex >= 0)
				{
					if(tIter->m_Type == vObjectDesc.m_AttribVec[nIndex].m_Type)
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
						tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						continue ;
					}
					else if(0 == strcmp(tIter->m_Name.c_str(),"classname"))
					{
						stJsonNode	tNewChildJsonNode;
						tNewChildJsonNode.m_Type  = tIter->m_Type;
						tNewChildJsonNode.m_Name  = tIter->m_Name;
						tNewChildJsonNode.m_Value = tIter->m_Value;
						tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						continue ;
					}
					else if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
					{
						stJsonNode	tNewChildJsonNode;
						tNewChildJsonNode.m_Type  = tIter->m_Type;
						tNewChildJsonNode.m_Name  = tIter->m_Name;
						tNewChildJsonNode.m_Value = tIter->m_Value;
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
				else
				{
					//将缺失的新属性补上
					vector<stJsonAttribDesc>::iterator tIter2;
					for(tIter2 = vObjectDesc.m_AttribVec.begin(); tIter2 != vObjectDesc.m_AttribVec.end(); tIter2++)
					{
						bool bFind = false;
						vector<stJsonNode>::iterator tIter3;
						for(tIter3 = m_ChildVec.begin(); tIter3 != m_ChildVec.end(); tIter3++)
						{
							if(0 == strcmp(tIter->m_Name.c_str(),tIter2->m_Name.c_str()))
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
							tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						}
					}
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
				tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
				continue ;
			}
		}
	}
	else
	{
		stJsonObjectDesc&	vObjectDesc = pJson->GetObjectDesc(0);

		vector<stJsonNode>::iterator tIter;
		for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		{

			int	nIndex = vObjectDesc.FindAttribDesc(tIter->m_Name);
			if(nIndex >= 0)
			{
				if(tIter->m_Type == vObjectDesc.m_AttribVec[nIndex].m_Type)
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
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
					continue ;
				}
				else if(0 == strcmp(tIter->m_Name.c_str(),"classname"))
				{
					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter->m_Type;
					tNewChildJsonNode.m_Name  = tIter->m_Name;
					tNewChildJsonNode.m_Value = tIter->m_Value;
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
					continue ;
				}
				else if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
				{
					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter->m_Type;
					tNewChildJsonNode.m_Name  = tIter->m_Name;
					tNewChildJsonNode.m_Value = tIter->m_Value;
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
				if(0 == strcmp(tIter3->m_Name.c_str(),tIter2->m_Name.c_str()))
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
//输出到二进制文件
void	stJsonNode::WriteToFile(FILE* hFile)
{
	//Type
	fwrite(&m_Type,sizeof(Type),1,hFile);

	//Name
	int  nLen = m_Name.size();
	fwrite(&nLen,sizeof(int),1,hFile);
	fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);

	//Value
	nLen = m_Value.size();
	fwrite(&nLen,sizeof(int),1,hFile);
	fwrite(m_Value.c_str(),sizeof(char),nLen,hFile);


	int nAttribNum = m_ChildVec.size();
	fwrite(&nAttribNum,sizeof(int),1,hFile);
	vector<stJsonNode>::iterator tIter;
	for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
	{
		tIter->WriteToFile(hFile);
	}

}
//从二进制文件中读取
char*	stJsonNode::ReadFromBuff(char* pBuff)
{
	//Type
	char*	pTempBuff = pBuff;
	memcpy(&m_Type,pTempBuff,sizeof(Type));
	pTempBuff += sizeof(Type);

	//Name
	int  nLen = 0;
	memcpy(&nLen,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);

	memcpy(g_Buff,pTempBuff,nLen);
	pTempBuff += nLen;
	g_Buff[nLen]='\0';

	m_Name = g_Buff;

	//DefaultValue
	memcpy(&nLen,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);

	memcpy(g_Buff,pTempBuff,nLen);
	pTempBuff += nLen;
	g_Buff[nLen]='\0';

	m_Value = g_Buff;

	int nAttribNum = 0;
	memcpy(&nAttribNum,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);

	for(int i = 0 ; i < nAttribNum ; i++)
	{
		stJsonNode	tAttrib;
		pTempBuff = tAttrib.ReadFromBuff(pTempBuff);
		m_ChildVec.push_back(tAttrib);
	}	

	return pTempBuff;
}
//取得成生二进制结构所占用的大小
int		stJsonNode::GetPackedSize()
{
	int nSize = 0;
	switch(m_Type)
	{
	case kNullType:
		{
			nSize = 0;
		}
		break;
	case kFalseType:
		{
			nSize = 1;
		}
		break;
	case kTrueType:
		{
			nSize = 1;
		}
		break;
	case kObjectType:
		{
			nSize = sizeof(int);
		}
		break;
	case kArrayType:
		{
			nSize = sizeof(int);
		}
		break;
	case kStringType:
		{
			nSize = sizeof(int);
			nSize += m_Value.size();
		}
		break;
	case kNumberType:
		{
			nSize = sizeof(float);
		}
		break;
	}
	return nSize;
}
//构造
JsonLoader::JsonLoader()
{

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

	rapidjson::Document jsonDict;

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
			AddJsonNode(kNullType,strName,"Null",m_RootNode);
			continue ;
		}
		if(itr->value.IsArray())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kArrayType,strName,"Array",m_RootNode);
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
				AddJsonNode(kFalseType,strName,"False",m_RootNode);
			}
			if(itr->value.IsTrue())
			{
				AddJsonNode(kTrueType,strName,"True",m_RootNode);
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
				sprintf(szText,"%.2f",itr->value.GetDouble());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue ;
			}
			continue ;
		}
		if(itr->value.IsObject())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kObjectType,strName,"Object",m_RootNode);
			AddJsonNode_Object(itr->value,tNewJsonNode);
			continue ;
		}
		if(itr->value.IsString())
		{
			string	strText = itr->value.GetString() ;
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
//写入二进制文件
bool	JsonLoader::WriteCocoBinBuff(const char* pBinFileName,stJsonNode&	vNewJsonNode)
{
	FILE* hFile = fopen(pBinFileName,"wb");
	if(hFile)
	{
		//头信息
		stCocoFileHeader	tCocoFileHeader;
		strcpy(tCocoFileHeader.m_FileDesc,"Cocostudio-UI");
		strcpy(tCocoFileHeader.m_Version,"V1.0.1");
		tCocoFileHeader.m_ObjectCount = m_ObjectDescVec.size();
		fwrite(&tCocoFileHeader,sizeof(stCocoFileHeader),1,hFile);
		//遍历类
		vector<stJsonObjectDesc>::iterator tIter;
		for(tIter = m_ObjectDescVec.begin();tIter != m_ObjectDescVec.end(); tIter++)
		{
			tIter->WriteToFile(hFile);
		}

		vNewJsonNode.WriteToFile(hFile);
		fclose(hFile);
	}
	return true;
}
//从二进制文件中读取
bool	JsonLoader::ReadCocoBinBuff(char* pBinBuff)
{
	//Type
	char*	pTempBuff = pBinBuff;

	//头信息
	stCocoFileHeader	tCocoFileHeader;

	memcpy(tCocoFileHeader.m_FileDesc,pTempBuff,sizeof(tCocoFileHeader.m_FileDesc));
	pTempBuff += sizeof(tCocoFileHeader.m_FileDesc);

	memcpy(tCocoFileHeader.m_Version,pTempBuff,sizeof(tCocoFileHeader.m_Version));
	pTempBuff += sizeof(tCocoFileHeader.m_Version);

	memcpy(&tCocoFileHeader.m_ObjectCount,pTempBuff,sizeof(tCocoFileHeader.m_ObjectCount));
	pTempBuff += sizeof(tCocoFileHeader.m_ObjectCount);

	//遍历类
	for(int i = 0 ; i < tCocoFileHeader.m_ObjectCount ; i++)
	{
		stJsonObjectDesc	tJsonObjDesc;
		pTempBuff = tJsonObjDesc.ReadFromBuff(pTempBuff);
		m_ObjectDescVec.push_back(tJsonObjDesc);
	}

	m_RootNode.ReadFromBuff(pTempBuff);
	return true;
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
	vParentNode.m_ChildVec.push_back(tNewJsonNode);
	return vParentNode.m_ChildVec.back();
}
//增加一个结点
stJsonNode&		JsonLoader::AddJsonNode_ArrayItem(const rapidjson::Value &	vValue,stJsonNode& vParentNode)
{
	if(vValue.IsNull())
	{
		return AddJsonNode(kNullType,"","Null",vParentNode);
	}
	if(vValue.IsArray())
	{
		stJsonNode&	tNewJsonNode = AddJsonNode(kArrayType,"","Array",vParentNode);
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
			return AddJsonNode(kFalseType,"","False",vParentNode);
		}

		return	AddJsonNode(kTrueType,"","True",vParentNode);

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
		sprintf(szText,"%.2f",vValue.GetDouble());
		string	strText = szText ;
		return AddJsonNode(kNumberType,"",strText,vParentNode);
	}
	if(vValue.IsObject())
	{
		stJsonNode&	tNewJsonNode = AddJsonNode(kObjectType,"","Object",vParentNode);
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
			stJsonNode&	tNewJsonNode = AddJsonNode(kNullType,strName,"Null",vParentNode);

			if(false == bHasObjectDesc && 0 == strcmp(szName,"classname"))
			{
				AddObjectDesc("Null",vValue);
				bHasObjectDesc = true;
			}
			continue ;
		}
		if(itr->value.IsArray())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kArrayType,strName,"Array",vParentNode);
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
				AddJsonNode(kFalseType,strName,"False",vParentNode);
			}
			if(itr->value.IsTrue())
			{
				AddJsonNode(kTrueType,strName,"True",vParentNode);
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
				sprintf(szText,"%.2f",itr->value.GetDouble());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue ;
			}
			continue ;
		}
		if(itr->value.IsObject())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kObjectType,strName,"Object",vParentNode);
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
		if(0 == strcmp(tIter->m_Name.c_str(),strName.c_str()))
		{
			return ;
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
		while(szFindStr)
		{
			stJsonObjectDesc	tNewObject;
			tNewObject.m_Name  = szFindStr ;
			for (rapidjson::Value::ConstMemberIterator itr = vValue.MemberonBegin(); itr != vValue.MemberonEnd(); ++itr)
			{
				const char*	szAttribName  = itr->name.GetString();
				if(0 == strcmp(szAttribName,"children"))
				{
					continue ;
				}
				string	strAttribName = szAttribName;
				stJsonAttribDesc	tJsonAttrib;
				tJsonAttrib.m_Name = strAttribName ;
				tJsonAttrib.m_Type = itr->value.GetType();
				tJsonAttrib.m_DefaultValue = GetValueString(itr->value);
				tNewObject.m_AttribVec.push_back(tJsonAttrib);
			}
			m_ObjectDescVec.push_back(tNewObject);

			szFindStr = strtok(NULL,",");
			if(!szFindStr)break;
		}
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
			if(0 == strcmp(tIter->m_Name.c_str(),szFindStr))
			{
				strName = szFindStr;
				delete[] szMemory;
				return int(tIter - m_ObjectDescVec.begin());
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
//清空所有的类描述上的内容节点
void	JsonLoader::CleanObjectMemoryAddr()
{
	//类结点容器
	vector<stJsonObjectDesc>::iterator tIter;
	for(tIter = m_ObjectDescVec.begin() ; tIter != m_ObjectDescVec.end() ;  tIter++)
	{
		tIter->m_MemoryAddrVec.clear();
	}
}
//取得一个值项的字符串
string	JsonLoader::GetValueString(const rapidjson::Value &	vValue)
{
	string	strResult = "Null";
	switch(vValue.GetType())
	{
	case kNullType:
		{
			strResult = "Null";
		}
		break;
	case kFalseType:
		{
			strResult = "False";
		}
		break;
	case kTrueType:
		{
			strResult = "True";
		}
		break;
	case kObjectType:
		{
			strResult = "Object";
		}
		break;
	case kArrayType:
		{
			strResult = "Array";
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
				sprintf(szText,"%.2f",vValue.GetDouble());
				strResult = szText ;
			}
		}
		break;
	}
	return strResult;
}

