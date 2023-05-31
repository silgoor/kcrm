/****************************************************************************
**
** Copyright (C) 2009 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#include "xmlsettings.h"
#include "aes.h"
#include "common_inc.h"

extern GlobalClass glo;

//XMLSettings settings;
//-------------------------------------------------------------------------------------
bool XMLSettings::load(QString filename)
{
// Read settings from XML.
	QFile file(filename);
	if(!file.exists())
	{
		return false;
	}
	if(!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	return loadFromData(file.readAll());
//	QXmlInputSource source(&file);
//	QXmlSimpleReader reader;
//	reader.setContentHandler(this);
//	reader.parse(source);
	return true;
}
//-------------------------------------------------------------------------------------
bool XMLSettings::loadCrypted(QByteArray key,QString filename)
{
// Read settings from XML.
	QFile file(filename);
    if(!file.exists())
	{
        return false;
	}
    if(!file.open(QIODevice::ReadOnly))
	{
        return false;
	}
    Aes aes(128);
    return loadFromData(aes.decrypt(key,file.readAll()));
}
//-------------------------------------------------------------------------------------
bool XMLSettings::loadFromData(QByteArray data)
{
	itemList.clear();
/*	QXmlInputSource source;
	source.setData(data);
	QXmlSimpleReader reader;
	reader.setContentHandler(this);
	return reader.parse(source);*/
	QXmlStreamReader reader(data);
	while (!reader.atEnd())
	{
		reader.readNext();
		// делаем обработку
		if(reader.isStartElement())
		{
			level++;
//			qDebug()<<"StartElement: " << reader.name() << " level " << level;
			QXmlStreamAttributes attrs=reader.attributes();
			if(level==2)
			{
				current_group=reader.name().toString();
				if(attrs.hasAttribute("name"))
				{
					current_groupname=attrs.value("name").toString();
				}

			}
			if(level==3)
			{
				current_key=reader.name().toString();
				if(attrs.hasAttribute("name"))
				{
					current_keyname=attrs.value("name").toString();
				}
			}
		}
		if(reader.isCharacters())
		{
			if(level==3)
			{
//				qDebug()<<"value "<<reader.text();
				setValue(current_group,current_groupname,current_key,current_keyname,reader.text().toString());
			}
		}
		if(reader.isEndElement())
		{
//			qDebug()<<"EndElement: " << reader.name() << " level " << level;
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
		}
	}
	if (reader.hasError())
	{
		qDebug()<<reader.errorString();
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------------------
bool XMLSettings::save(QString filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
        return false;
    file.write(saveToData());
    return true;
}
//-------------------------------------------------------------------------------------
bool XMLSettings::saveCrypted(QByteArray key,QString filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
        return false;
    Aes aes(128);
    if(file.write(aes.crypt(key,saveToData()))==-1)
        return false;
    return true;
}
//-------------------------------------------------------------------------------------
QByteArray XMLSettings::saveToData()
{
	QStringList groupList=groups();
	QString xmlContent="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
	xmlContent+="<settings>\r\n";
	for(int groupIndex=0;groupIndex<groupList.size();groupIndex++)
	{
		if(!groupList.at(groupIndex).isEmpty())// Группа обязательна!
		{
			QStringList groupNameList=groupnames(groupList.at(groupIndex));
			for(int groupNameIndex=0;groupNameIndex<groupNameList.size();groupNameIndex++)
			{
//				QMessageBox::warning(0,PROGNAME,groupList.at(groupIndex)+" "+groupNameList.at(groupNameIndex));
				QString groupName="";
				if(!groupNameList.at(groupNameIndex).isEmpty())
					groupName=" name=\""+groupNameList.at(groupNameIndex)+"\"";
				xmlContent+="	<"+groupList.at(groupIndex)+groupName+">\r\n";
				for(int j=0;j<itemList.size();j++)
				{
					if(itemList.at(j).m_group==groupList.at(groupIndex) && itemList.at(j).m_groupname==groupNameList.at(groupNameIndex))
					{
						QString keyName="";
						if(!itemList.at(j).m_keyname.isEmpty())
							keyName=" name=\""+itemList.at(j).m_keyname+"\"";
						xmlContent+=QString("		<")+itemList.at(j).m_key+keyName+">"+encodeToXML(itemList.at(j).m_value)+"</"+itemList.at(j).m_key+">\r\n";
					}
				}
				xmlContent+="	</"+groupList.at(groupIndex)+">\r\n";
			}
		}
	}
	xmlContent+="</settings>\r\n";
    return xmlContent.toUtf8();
}
//-------------------------------------------------------------------------------------
QString XMLSettings::value(const QString & group,const QString & groupname,const QString & key,const QString & keyname) const
{
	return value(group,groupname,key,keyname,"");
}
//-------------------------------------------------------------------------------------
QString XMLSettings::value(const QString & group,const QString & groupname,const QString & key,const QString & keyname,const QString & defaultvalue) const
{
	for(int i=0;i<itemList.size();i++)
	{
		if(group==itemList.at(i).m_group && groupname==itemList.at(i).m_groupname && key==itemList.at(i).m_key && keyname==itemList.at(i).m_keyname)
		{
//			Superlog.AddString(0,"get value: "+cname+" = "+itemList.at(i).value.toString());
			return itemList.at(i).m_value;
		}
	}
//	Superlog.AddString(0,"Error: invalid setting name: "+cname);
	return defaultvalue;
}
//-------------------------------------------------------------------------------------
void XMLSettings::setValue(const QString & group,const QString & groupname,const QString & key,const QString & keyname,const QString & value)
{
	int itemscount=itemList.size();
	for(int i=0;i<itemscount;i++)
	{
		if(group==itemList.at(i).m_group && groupname==itemList.at(i).m_groupname && key==itemList.at(i).m_key && keyname==itemList.at(i).m_keyname)
		{
//			Superlog.AddString(0,"SET value: "+cname+" = "+cvalue.toString());
			itemList[i].m_value=value;
//			Superlog.AddString(0,"stored: "+itemList[i].value.toString());
			return;
		}
	}
	itemList.append(SettingsItem(group,groupname,key,keyname,value));
}
//-------------------------------------------------------------------------------------
void XMLSettings::removeGroupname(const QString & group,const QString & groupname)
{
	for(int i=0;i<itemList.size();i++)
	{
		if(group==itemList.at(i).m_group && groupname==itemList.at(i).m_groupname)
		{
			itemList.removeAt(i);
			i--;
		}
	}
}
//-------------------------------------------------------------------------------------
QStringList XMLSettings::groups()
{
	QStringList groups;
	for(int i=0;i<itemList.size();i++)
	{
		bool finded=false;
		for(int j=0;j<groups.size();j++)
		{
			if(groups.at(j)==itemList.at(i).m_group)
			{
				finded=true;
				break;
			}
		}
		if(!finded)
			groups.append(itemList.at(i).m_group);
	}
	return groups;
}
//-------------------------------------------------------------------------------------
QStringList XMLSettings::groupnames(QString group)
{
	QStringList groupnames;
	for(int i=0;i<itemList.size();i++)
	{
		if(group==itemList.at(i).m_group)
		{
			bool finded=false;
			for(int j=0;j<groupnames.size();j++)
			{
				if(groupnames.at(j)==itemList.at(i).m_groupname)
				{
					finded=true;
					break;
				}
			}
			if(!finded)
				groupnames.append(itemList.at(i).m_groupname);
		}
	}
	return groupnames;
}
//-------------------------------------------------------------------------------------
QStringList XMLSettings::keys(QString group,QString groupname)
{
	QStringList keys;
	for(int i=0;i<itemList.size();i++)
	{
		if(group==itemList.at(i).m_group && groupname==itemList.at(i).m_groupname)
			keys.append(itemList.at(i).m_key);
	}
	return keys;
}
//-------------------------------------------------------------------------------------
QStringList XMLSettings::keynames(QString group,QString groupname,QString key)
{
	QStringList keynames;
	for(int i=0;i<itemList.size();i++)
	{
		if(group==itemList.at(i).m_group && groupname==itemList.at(i).m_groupname && key==itemList.at(i).m_key)
		{
			bool finded=false;
			for(int j=0;j<keynames.size();j++)
			{
				if(keynames.at(j)==itemList.at(i).m_keyname)
				{
					finded=true;
					break;
				}
			}
			if(!finded)
				keynames.append(itemList.at(i).m_keyname);
		}
	}
	return keynames;
}
//-------------------------------------------------------------------------------------
void XMLSettings::moveToEnd(QString group,QString groupname)
{
    QStringList keys;
    for(int i=itemList.size()-1;i>=0;i--)
    {
        if(group==itemList.at(i).m_group && groupname==itemList.at(i).m_groupname)
        {
            SettingsItem si=itemList.at(i);
            itemList.removeAt(i);
            itemList.append(si);
        }
    }
}
//-------------------------------------------------------------------------------------
