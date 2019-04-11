#include "tools/xmltools.h"

XmlTool::XmlTool(string XmlFileName)
{
  m_XmlFileName = XmlFileName;
  m_ptrDocument = NULL;
  m_ptrDeclaration = NULL;
}
XmlTool::~XmlTool()
{

}

vector<string> XmlTool::SplitString(const string& s, const string& c)
{
  vector<string> v;
  string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while(string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));

    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));

  return v;
}

vector<vector<string> > XmlTool::GetFileName(string param_dir)
{
  vector<string> name_list;
  struct dirent *ptr;
  DIR *dir;
  dir = opendir(param_dir.data());
  while ((ptr=readdir(dir))!=NULL)
  {
    if(ptr->d_name[0] == '.')
      continue;
    if(int(ptr->d_type)==4)
    {
      string dir_name = param_dir;
      string files_name = ptr->d_name;
      string search_path = dir_name+files_name+"/";

      GetFileName(search_path);
    }
    else {
      string full_path = param_dir+ptr->d_name;
      //std::cout<<full_path<<std::endl;
      string ImgName = ptr->d_name;

      ///images_background/Greek/character01/img.jpg

      string spilt_string = "/";
      name_list=SplitString(param_dir,spilt_string);
      name_list.push_back(ptr->d_name);

      full_path_sum.push_back(name_list);
    }
  }
  return full_path_sum;
}

void XmlTool::CreateXML(string param_dir)
{
  m_ptrDocument = new TiXmlDocument(m_XmlFileName);
  m_ptrDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "");
  m_ptrDocument->LinkEndChild(m_ptrDeclaration);
  //创建根节点
  string xmlFileName = m_XmlFileName;
  string rootName = (xmlFileName.substr(0, xmlFileName.find(".")));//根节点元素名为文件名去掉.xml
  TiXmlElement *pRoot = new TiXmlElement(rootName.c_str());
  if (NULL == pRoot)
  {
    return;
  }
  //关联XML文档，成为XML文档的根节点
  m_ptrDocument->LinkEndChild(pRoot);
  //创建孩子节点
  TiXmlElement *pStudent = new TiXmlElement("classname");
  if (NULL == pStudent)
  {
    return;
  }
  TiXmlText *pStuText = new TiXmlText("小学生");
  pStudent->LinkEndChild(pStuText);


  pStudent->SetAttribute("id", 1);
  pStudent->SetAttribute("age", 18);
  pStudent->SetAttribute("name", "张三");

  TiXmlElement *pYuWen  = new TiXmlElement("YuWen");
  TiXmlText *pYuWenText = new TiXmlText("98");
  pYuWen->LinkEndChild(pYuWenText);

  TiXmlElement *pMathe  = new TiXmlElement("Mathe");
  TiXmlText *pMatheText = new TiXmlText("99");
  pMathe->LinkEndChild(pMatheText);

  TiXmlElement *pEnglish = new TiXmlElement("English");
  TiXmlText *pEnglishText = new TiXmlText("100");
  pEnglish->LinkEndChild(pEnglishText);

  pStudent->LinkEndChild(pYuWen);
  pStudent->LinkEndChild(pMathe);
  pStudent->LinkEndChild(pEnglish);

  //关联根节点，成为根节点的孩子节点
  pRoot->LinkEndChild(pStudent);
  m_ptrDocument->SaveFile(m_XmlFileName);

  for(int i=0;i<full_path_sum.size(); i++)
  {
    if((full_path_sum[i].size())>=5)
    {
      std::cout<<full_path_sum[i][(full_path_sum[i].size()-4)]<<std::endl;
      std::cout<<full_path_sum[i][(full_path_sum[i].size()-3)]<<std::endl;
      std::cout<<full_path_sum[i][(full_path_sum[i].size()-2)]<<std::endl;
      std::cout<<full_path_sum[i][(full_path_sum[i].size()-1)]<<std::endl;

      TiXmlElement *pfirstElement;
      if(FindNode(pRoot,full_path_sum[i][(full_path_sum[i].size()-4)],pfirstElement))
      {
        std::cout<<"*************************"<<std::endl;
      }
      else {
        pfirstElement= new TiXmlElement(full_path_sum[i][(full_path_sum[i].size()-4)]);
      }
      if(pfirstElement == NULL)
      {
        return;
      }
    }
  }
}
void XmlTool::ReadXML()
{
  if (m_XmlFileName == "")
  {
    std::cout << " null " << std::endl;
    return;
  }
  m_ptrDocument->LoadFile(m_XmlFileName);
  m_ptrDocument->Print();
}
void XmlTool::ReadDeclaration(string &version, string &encoding, string &standalone)
{
  m_ptrDocument->LoadFile(m_XmlFileName);

  TiXmlNode *pNode = m_ptrDocument->FirstChild();
  if (NULL != pNode)
  {
    //获取声明指针
    m_ptrDeclaration = pNode->ToDeclaration();

    if (NULL != m_ptrDeclaration)
    {
      version    = m_ptrDeclaration->Version();
      encoding   = m_ptrDeclaration->Encoding();
      standalone = m_ptrDeclaration->Standalone();
    }
  }
}
bool XmlTool::FindNode(TiXmlElement *pRoot, const string nodeName, TiXmlElement *&pNode)
{
  const char *value = pRoot->Value();
  if (strcmp(pRoot->Value(), nodeName.c_str()) == 0)
  {
    pNode = pRoot;
    return true;
  }

  TiXmlElement *p = pRoot;
  for (p = p->FirstChildElement(); p != NULL; p = p->NextSiblingElement())
  {
    FindNode(p, nodeName, pNode);
    return true;
  }

  return false;
}
bool XmlTool::GetNodeText(const string nodeName, const char *&text)
{
  m_ptrDocument->LoadFile(m_XmlFileName);
  if (NULL == m_ptrDocument)
  {
    return false;
  }

  TiXmlElement *pRoot = m_ptrDocument->RootElement();
  const char *value = pRoot->Value();
  if (NULL == pRoot)
  {
    return false;
  }

  TiXmlElement *pNode = NULL;
  if (FindNode(pRoot, nodeName, pNode))
  {
    text = pNode->GetText();
    return true;
  }

  return false;
}
bool XmlTool::GetNodeAttribute(const string nodeName, map<string, string> &mapAttribute)
{
  m_ptrDocument->LoadFile(m_XmlFileName);
  if (NULL == m_ptrDocument)
  {
    return false;
  }

  TiXmlElement *pRoot = m_ptrDocument->RootElement();
  if (NULL == pRoot)
  {
    return false;
  }

  TiXmlElement *pNode = NULL;
  if (FindNode(pRoot, nodeName, pNode))
  {
    TiXmlAttribute *pAttr = NULL;
    for (pAttr = pNode->FirstAttribute(); pAttr != NULL; pAttr = pAttr->Next())
    {
      string name = pAttr->Name();
      string value = pAttr->Value();
      mapAttribute.insert(make_pair(name, value));
    }

    return true;
  }

  return false;
}
bool XmlTool::DeleteNode(const string nodeName)
{
  m_ptrDocument->LoadFile(m_XmlFileName);

  TiXmlElement *pRoot = m_ptrDocument->RootElement();
  if (NULL == pRoot)
  {
    return false;
  }

  TiXmlElement *pNode = NULL;
  if (FindNode(pRoot, nodeName, pNode))
  {
    if (pNode == pRoot)
    {//如果是根节点
      m_ptrDocument->RemoveChild(pRoot);
      m_ptrDocument->SaveFile(m_XmlFileName);
      return true;
    }
    else
    {//子节点
      TiXmlNode *parent = pNode->Parent();//找到该节点的父节点
      if (NULL == parent)
      {
        return false;
      }

      TiXmlElement *parentElem = parent->ToElement();
      if (NULL == parentElem)
      {
        return false;
      }
      parentElem->RemoveChild(pNode);
      m_ptrDocument->SaveFile(m_XmlFileName);
      return true;
    }
  }

  return false;
}
bool XmlTool::modifyText(const string nodeName, const string text)
{
  m_ptrDocument->LoadFile(m_XmlFileName);

  TiXmlElement *pRoot = m_ptrDocument->RootElement();
  if (NULL == pRoot)
  {
    return false;
  }

  TiXmlElement *pNode = NULL;
  if (FindNode(pRoot, nodeName, pNode))
  {
    //pNode->Clear();//删除原节点下的其他元素
    TiXmlText *pText = new TiXmlText(text.c_str());
    pNode->LinkEndChild(pText);
    m_ptrDocument->SaveFile(m_XmlFileName);
    return true;
  }

  return false;
}
bool XmlTool::modifyAttribution(const string nodeName, map<string, string> &mapAttribute)
{
  m_ptrDocument->LoadFile(m_XmlFileName);

  TiXmlElement *pRoot = m_ptrDocument->RootElement();
  if (NULL == pRoot)
  {
    return false;
  }

  TiXmlElement *pNode = NULL;
  if (FindNode(pRoot, nodeName, pNode))
  {
    TiXmlAttribute *pAttr = pNode->FirstAttribute();
    char *strName = NULL;
    for (; pAttr != NULL; pAttr = pAttr->Next())
    {
      strName = const_cast<char *>(pAttr->Name());
      map<string,string>::iterator it;
      for (it = mapAttribute.begin(); it != mapAttribute.end(); ++it)
      {
        if (strName == it->first)
        {
          pNode->SetAttribute(strName, it->second.c_str());
        }
      }
    }
    m_ptrDocument->SaveFile(m_XmlFileName);
    return true;
  }

  return false;
}
bool XmlTool::AddNode(const string nodeName, string newNodeName)
{
  m_ptrDocument->LoadFile(m_XmlFileName);

  TiXmlElement *pRoot = m_ptrDocument->RootElement();
  if (NULL == pRoot)
  {
    return false;
  }

  TiXmlElement *pNode = NULL;
  if (FindNode(pRoot, nodeName, pNode))
  {
    TiXmlElement *pNewNode = new TiXmlElement(newNodeName);
    //TiXmlText *pNewText = new TiXmlText(text);
    //pNewNode->LinkEndChild(pNewText);
    pNode->InsertEndChild(*pNewNode);
    m_ptrDocument->SaveFile(m_XmlFileName);
    return true;
  }

  return false;
}

int main()
{
  XmlTool xmltest("test.xml");

  xmltest.GetFileName("./");
  xmltest.CreateXML("./");
  return 0;
}
