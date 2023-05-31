/****************************************************************************
**
** Copyright (C) 2009 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#include "colorsqlquerymodel.h"
#include "qmessagebox.h"
#include "common_inc.h"


#define COST_COLUMN 11
#define PERFORMER_COLUMN 12
#define COLLECTION_COLUMN 15

extern unsigned int globaluid;

//-------------------------------------------------------------------------------------
ColorSqlQueryModel::ColorSqlQueryModel()
{
    boldFont.setBold(true);
}
//-------------------------------------------------------------------------------------
QVariant ColorSqlQueryModel::data(const QModelIndex &index, int role) const
{

    if(role == Qt::DisplayRole) // Текст колонки
    {
        if(index.column() == COLLECTION_COLUMN) // Текст колонки инкассации
        {
    //        int id = record(index.row()).value(0).toInt();
    //        double cost = record(index.row()).value(COST_COLUMN).toDouble();
            unsigned int collection = record(index.row()).value(COLLECTION_COLUMN).toUInt();
            if(collection)//encodeCollected(id+int(cost))==collection)
                return "Принято";
            else
                return "";
        }
        if(!m_adminFlag && index.column() == COST_COLUMN) // Текст колонки цены.
        {
            unsigned int collection = record(index.row()).value(COLLECTION_COLUMN).toUInt();
            if(collection) // Прячем цену для инкассированных, если не админ.
                return "";
        }
    }
    else if(role == Qt::FontRole)
    {
//        int id = record(index.row()).value(0).toInt();
        double cost = record(index.row()).value(COST_COLUMN).toDouble();
        unsigned int collection = record(index.row()).value(COLLECTION_COLUMN).toUInt();
        if(!collection)//encodeCollected(id+int(cost))!=collection) // Выделяем только если не было инкассации.
        {
            if(cost>0)
                return boldFont;
        }
    }
    else if(role == Qt::BackgroundRole)
	{
        // Проверка заполнения полей заявки. Если не заполнена, подсвечиваем серым.
        if(record(index.row()).value(4).toString().isEmpty() || record(index.row()).value(10).toString().isEmpty())
        {
            return QVariant(QColor(127,127,127));
        }
        // Проверка инкассации
//        int id = record(index.row()).value(0).toInt();
        double cost = record(index.row()).value(COST_COLUMN).toDouble();
        unsigned int collection = record(index.row()).value(COLLECTION_COLUMN).toUInt();
        if(!collection)//encodeCollected(id+int(cost))!=collection) // Выделяем только если не было инкассации.
        {
            QString performerName = record(index.row()).value(PERFORMER_COLUMN).toString();
            if(cost<300)
                return QVariant(QColor(255,50,50));
            if(performerName.isEmpty())
                return QVariant(QColor(255,255,50));
        }
    }
    else if(role == Qt::ForegroundRole)
    {
//        int id = record(index.row()).value(0).toInt();
        double cost = record(index.row()).value(COST_COLUMN).toDouble();
        unsigned int collection = record(index.row()).value(COLLECTION_COLUMN).toUInt();
        if(!collection)//encodeCollected(id+int(cost))!=collection) // Выделяем только если не было инкассации.
        {
            if(cost<300)
                return QVariant(QColor(255,255,255));
        }

    }
    return QSqlQueryModel::data(index, role);
}
//-------------------------------------------------------------------------------------
