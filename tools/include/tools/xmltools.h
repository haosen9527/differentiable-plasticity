#ifndef XMLTOOLS_H
#define XMLTOOLS_H

#include <tinyxml.h>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <dirent.h>

using namespace std;

class XmlTool
{
public:
  XmlTool(string XmlFileName);
  ~XmlTool();

public:

  vector<vector<string> > GetFileName(string param_dir);
  vector<string> SplitString(const string& s, const string& c);

  void CreateXML(string param_dir);//创建XML文件
  void ReadXML();//读取XML文件完整内容
  void ReadDeclaration(string &version, string &encoding, string &standalone);//读取XML声明
  bool FindNode(TiXmlElement *pRoot, const string nodeName, TiXmlElement *&pNode);//根据节点名，判断节点是否存在
  bool GetNodeText(const string nodeName, const char *&text);//根据节点名，获取该节点文本
  bool GetNodeAttribute(const string nodeName, map<string, string> &mapAttribute);//获取根据节点名， 获取节点属性
  bool DeleteNode(const string nodeName);//根据节点名，删除节点
  bool modifyText(const string nodeName, const string text);//修改节点文本
  bool modifyAttribution(const string nodeName, map<string, string> &mapAttribute);//修改节点属性
  bool AddNode(const string nodeName, string newNodeName);//添加节点

private:
  string m_XmlFileName;
  TiXmlDocument *m_ptrDocument;
  TiXmlDeclaration *m_ptrDeclaration;

public:
  vector<vector<string> > full_path_sum;
};

#endif // XMLTOOLS_H
