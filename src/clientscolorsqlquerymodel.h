/****************************************************************************
**
** Copyright (C) 2009 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#ifndef CLIENTSCOLORSQLQUERYMODEL_H
#define CLIENTSCOLORSQLQUERYMODEL_H

#include <QtSql>

class ClientsColorSqlQueryModel : public QSqlQueryModel
{
    QFont boldFont;
    QVariant data(const QModelIndex &index, int role) const;
public:
    ClientsColorSqlQueryModel();
};

#endif
