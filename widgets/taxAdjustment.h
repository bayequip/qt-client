/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef TAXADJUSTMENT_H
#define TAXADJUSTMENT_H

#include <parameter.h>
#include "ui_taxAdjustment.h"

class XTUPLEWIDGETS_EXPORT taxAdjustment : public QDialog, public Ui::taxAdjustment
{
    Q_OBJECT

public:
    taxAdjustment(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~taxAdjustment();

public slots:
    virtual void set( const ParameterList & pParams );
    virtual void sSave();
    virtual void sCheck();

protected slots:
    virtual void languageChange();

private:
  int _taxdetailid;
  int _mode;
  int _orderid;
  QString _ordertype;
  QString _table;
	
};

#endif // TAXADJUSTMENT
