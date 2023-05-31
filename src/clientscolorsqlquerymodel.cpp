/****************************************************************************
**
** Copyright (C) 2009 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#include "clientscolorsqlquerymodel.h"
#include "common_inc.h"


#define CALL_RESULT_COLUMN 7
//-------------------------------------------------------------------------------------
ClientsColorSqlQueryModel::ClientsColorSqlQueryModel()
{
    boldFont.setBold(true);
}
//-------------------------------------------------------------------------------------
QVariant ClientsColorSqlQueryModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::BackgroundRole)
    {
        int callResult=record(index.row()).value(CALL_RESULT_COLUMN).toInt();
//        qDebug()<<record(index.row()).value(CALL_RESULT_COLUMN).toString();
        if(callResult==CALL_RESULT_MAINTENANCE_SCHEDULED)
        {
            return QVariant(QColor(50,255,50));
        }
        if(callResult==CALL_RESULT_STULL)
        {
            return QVariant(QColor(255,255,50));
        }
        if(callResult==CALL_RESULT_REJECTED)
        {
            return QVariant(QColor(255,50,50));
        }
        if(callResult==CALL_RESULT_DONT_CALL)
        {
            return QVariant(QColor(0,0,0));
        }
		if(callResult==CALL_RESULT_DOESNT_ANSWER)
		{
			return QVariant(QColor(255,255,0));
		}
	}
    else if(role == Qt::ForegroundRole)
    {
        int callResult=record(index.row()).value(CALL_RESULT_COLUMN).toInt();
        if(callResult==CALL_RESULT_DONT_CALL || callResult==CALL_RESULT_REJECTED)
        {
            return QVariant(QColor(255,255,255));
        }
    }
    return QSqlQueryModel::data(index, role);
}
//-------------------------------------------------------------------------------------
