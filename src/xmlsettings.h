/****************************************************************************
**
** Copyright (C) 2009 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#ifndef XMLSETTINGS_H
#define XMLSETTINGS_H
#include <QtXml>
#ifndef QT_QML_LIB
//#include <QMessageBox>
#endif

struct SettingsItem
{
public:
	SettingsItem(const QString & group,const QString & groupname,const QString & key,const QString & keyname,const QString & value){m_group=group,m_groupname=groupname,m_key=key,m_keyname=keyname,m_value=value;};
	QString m_group;
	QString m_groupname;
	QString m_key;
	QString m_keyname;
	QString m_value;
};


class XMLSettings
{
	static QString encodeToXML(QString string)
	{
		string.replace(QString("&"),QString("&amp;"));
		string.replace(QString("'"),QString("&apos;"));
		string.replace(QString("\""),QString("&quot;"));
		string.replace(QString("<"),QString("&lt;"));
		string.replace(QString(">"),QString("&gt;"));
		return string;
	}
//-------------------------------------------------------------------------------------
private:
	QList<SettingsItem> itemList;
	int level;
	QString current_group,current_groupname,current_key,current_keyname;
public:
	QString m_fileName;
	XMLSettings()
	{
		level=0;
	}
	bool load(QString filename);
    bool loadCrypted(QByteArray key,QString filename);
    bool loadFromData(QByteArray data);
	bool save(QString filename);
    bool saveCrypted(QByteArray key,QString filename);
    QByteArray saveToData();
    QString value(const QString & group,const QString & groupname,const QString & key,const QString & keyname) const;
    QString value(const QString & group,const QString & groupname,const QString & key,const QString & keyname,const QString & defaultvalue) const;
//	QString value(const QString & group,const QString & key){return value(group,"",key,"");}
//	QString value(const QString & group,const QString & key,const QString & defaultvalue){return value(group,"",key,"",defaultvalue);}
//	void setValue(const QString & group,const QString & groupname,const QString & key,const QString & value);
	void setValue(const QString & group,const QString & groupname,const QString & key,const QString & keyname,const QString & value);
//	void setValue(const QString & group,const QString & name,const QString & value){setValue(group,"",name,"",value);}
	void removeGroupname(const QString & group,const QString & groupname);
	QStringList groups();
	QStringList groupnames(QString group);
	QStringList keys(QString group,QString groupname);
	QStringList keynames(QString group,QString groupname,QString key);
    void moveToEnd(QString group,QString groupname);

private:
/*	bool startElement(const QString&, const QString&, const QString& name, const QXmlAttributes& attrs)
	{
		level++;
		if(level==2)
		{
			current_group=name;
			for(int i=0;i<attrs.count();i++)
			{
				if(attrs.localName(i) == "name")
					current_groupname=attrs.value(i);
			}
			
		}
		if(level==3)
		{
			current_key=name;
			for(int i=0;i<attrs.count();i++)
			{
				if(attrs.localName(i) == "name")
					current_keyname=attrs.value(i);
			}
			
		}
		return true;
	}
	//--------------------------------------------------------------------------------------------
	bool characters(const QString& strText)
	{
		if(level==3)
		{
			setValue(current_group,current_groupname,current_key,current_keyname,strText);
			return true;
		}
		return true;
	}
	//--------------------------------------------------------------------------------------------
	bool endElement(const QString&, const QString&, const QString&)
	{
//		QMessageBox::information(0,"Parser",name,"OK",0,0);
		if(level==2)
		{
			current_group="";
			current_groupname="";
		}
		if(level==3)
		{
			current_key="";
			current_keyname="";
		}
		level--;
		return true;
	}
	//--------------------------------------------------------------------------------------------
	bool fatalError(const QXmlParseException&)
	{
        qDebug("XML parse error");
		return true;
	}*/
	//--------------------------------------------------------------------------------------------
};


#endif
