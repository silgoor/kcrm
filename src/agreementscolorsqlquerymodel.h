/****************************************************************************
**
** Copyright (C) 2009 Kaufman V.S. All rights reserved.
**
****************************************************************************/

#ifndef AGREEMENTSCOLORSQLQUERYMODEL_H
#define AGREEMENTSCOLORSQLQUERYMODEL_H

#include <QtSql>

class AgreementsColorSqlQueryModel : public QSqlQueryModel
{
    QFont boldFont;
    QVariant data(const QModelIndex &index, int role) const;
public:
    AgreementsColorSqlQueryModel();
};

#endif
