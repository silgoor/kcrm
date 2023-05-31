/****************************************************************************
**
** Copyright (C) 2009 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#include "agreementscolorsqlquerymodel.h"
//#include "qmessagebox.h"
#include "common_inc.h"


#define COST_COLUMN 10
#define COLLECTION_COLUMN 13
//-------------------------------------------------------------------------------------
AgreementsColorSqlQueryModel::AgreementsColorSqlQueryModel()
{
    boldFont.setBold(true);
}
//-------------------------------------------------------------------------------------
QVariant AgreementsColorSqlQueryModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole && index.column() == COLLECTION_COLUMN)
    {
        int id = record(index.row()).value(0).toInt();
        double cost = record(index.row()).value(COST_COLUMN).toDouble();
        unsigned int collection = record(index.row()).value(COLLECTION_COLUMN).toUInt();
        if(encodeCollected(id+int(cost))==collection)
            return "Принято";
        else
            return "";
    }
    if(role == Qt::FontRole)
    {
        int id = record(index.row()).value(0).toInt();
        double cost = record(index.row()).value(COST_COLUMN).toDouble();
        unsigned int collection = record(index.row()).value(COLLECTION_COLUMN).toUInt();
        if(encodeCollected(id+int(cost))!=collection) // Выделяем только если не было инкассации.
        {
            if(cost>0)
                return boldFont;
        }
    }
    return QSqlQueryModel::data(index, role);
}
//-------------------------------------------------------------------------------------
