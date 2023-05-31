/****************************************************************************
**
** Copyright (C) 2009 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#ifndef CQLORSQLQUERYMODEL_H
#define CQLORSQLQUERYMODEL_H

#include <QtSql>

class ColorSqlQueryModel : public QSqlQueryModel
{
    QFont boldFont;
    bool m_adminFlag;
    QVariant data(const QModelIndex &index, int role) const;
public:
	ColorSqlQueryModel();
    void setAdminFlag(bool adminFlag){m_adminFlag=adminFlag;}
};

#endif
