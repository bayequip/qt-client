/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2014 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "incidentCategory.h"

#include <QMessageBox>
#include <QSqlError>
#include <QValidator>
#include <QVariant>
#include <errorReporter.h>
#include "guiErrorCheck.h"

incidentCategory::incidentCategory(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : XDialog(parent, name, modal, fl)
{
    setupUi(this);

    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(sSave()));
    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(_name, SIGNAL(editingFinished()), this, SLOT(sCheck()));
    
    if (!_metrics->boolean("EnableBatchManager"))
    {
      _ediprofile->hide();
      _ediprofileLit->hide();
    }
    else
      _ediprofile->populate("SELECT ediprofile_id, ediprofile_name "
                            "FROM xtbatch.ediprofile "
                            "WHERE (ediprofile_type='email');");

    _template->populate("SELECT tasktmpl_id, tasktmpl_name FROM tasktmpl ORDER BY tasktmpl_name;");
}

incidentCategory::~incidentCategory()
{
}

void incidentCategory::languageChange()
{
    retranslateUi(this);
}

enum SetResponse incidentCategory::set(const ParameterList &pParams)
{
  XDialog::set(pParams);
  QVariant param;
  bool     valid;

  param = pParams.value("incdtcat_id", &valid);
  if (valid)
  {
    _incdtcatId = param.toInt();
    populate();
  }

  param = pParams.value("mode", &valid);
  if (valid)
  {
    if (param.toString() == "new")
    {
      _mode = cNew;
    }
    else if (param.toString() == "edit")
    {
      _mode = cEdit;
    }
    else if (param.toString() == "view")
    {
      _mode = cView;

      _name->setEnabled(false);
      _order->setEnabled(false);
      _descrip->setEnabled(false);
      _buttonBox->clear();
      _buttonBox->addButton(QDialogButtonBox::Close);
    }
  }

  return NoError;
}

void incidentCategory::sCheck()
{
  XSqlQuery incidentCheck;
  _name->setText(_name->text().trimmed());
  if ( (_mode == cNew) && (_name->text().length()) )
  {
    incidentCheck.prepare( "SELECT incdtcat_id "
               "FROM incdtcat "
               "WHERE (UPPER(incdtcat_name)=UPPER(:incdtcat_name));" );
    incidentCheck.bindValue(":incdtcat_name", _name->text());
    incidentCheck.exec();
    if (incidentCheck.first())
    {
      _incdtcatId = incidentCheck.value("incdtcat_id").toInt();
      _mode = cEdit;
      populate();

      _name->setEnabled(false);
    }
  }
}

void incidentCategory::sSave()
{
  XSqlQuery incidentSave;
  QList<GuiErrorCheck> errors;
  errors << GuiErrorCheck(_name->text().trimmed().length() == 0, _name, 
                          tr("You must enter a valid Name for this Incident Category."))
  ;

  if (GuiErrorCheck::reportErrors(this, tr("Cannot Save Incident Category"), errors))
      return;

  if (_mode == cNew)
  {
    incidentSave.prepare( "INSERT INTO incdtcat "
               "(incdtcat_name, incdtcat_order, incdtcat_descrip, incdtcat_ediprofile_id, incdtcat_tasktmpl_id)"
               " VALUES "
               "(:incdtcat_name, :incdtcat_order, :incdtcat_descrip, :incdtcat_ediprofile_id, :template ) "
               "RETURNING incdtcat_id; " );
  }
  else if (_mode == cEdit)
  {
    incidentSave.prepare( "SELECT incdtcat_id "
               "FROM incdtcat "
               "WHERE ( (UPPER(incdtcat_name)=UPPER(:incdtcat_name))"
               " AND (incdtcat_id<>:incdtcat_id) );" );
    incidentSave.bindValue(":incdtcat_id", _incdtcatId);
    incidentSave.bindValue(":incdtcat_name", _name->text().trimmed());
    incidentSave.exec();
    if (incidentSave.first())
    {
      QMessageBox::warning( this, tr("Cannot Save Incident Category"),
                            tr("You may not rename this Incident Category with "
			       "the entered value as it is in use by another "
			       "Incident Category.") );
      return;
    }
    else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Saving Incident Category Information"),
                                  incidentSave, __FILE__, __LINE__))
    {
      return;
    }

    incidentSave.prepare( "UPDATE incdtcat "
               "SET incdtcat_name=:incdtcat_name, "
	       "    incdtcat_order=:incdtcat_order, "
	       "    incdtcat_descrip=:incdtcat_descrip, "
	       "    incdtcat_tasktmpl_id=:template, "
               "    incdtcat_ediprofile_id=:incdtcat_ediprofile_id "
               "WHERE (incdtcat_id=:incdtcat_id);" );
  }

  incidentSave.bindValue(":incdtcat_id", _incdtcatId);
  incidentSave.bindValue(":incdtcat_name", _name->text());
  incidentSave.bindValue(":incdtcat_order", _order->value());
  incidentSave.bindValue(":incdtcat_descrip", _descrip->toPlainText());
  if (_ediprofile->id() != -1)
    incidentSave.bindValue(":incdtcat_ediprofile_id", _ediprofile->id());
  if (_template->isValid())
    incidentSave.bindValue(":template", _template->id());

  incidentSave.exec();
  if (incidentSave.first())
    _incdtcatId = incidentSave.value("incdtcat_id").toInt();
  else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Saving Incident Category Information"),
                                incidentSave, __FILE__, __LINE__))
  {
    return;
  }

  done(_incdtcatId);
}

void incidentCategory::populate()
{
  XSqlQuery incidentpopulate;
  incidentpopulate.prepare( "SELECT *,COALESCE(incdtcat_ediprofile_id,-1) AS ediprofile "
             "FROM incdtcat "
             "WHERE (incdtcat_id=:incdtcat_id);" );
  incidentpopulate.bindValue(":incdtcat_id", _incdtcatId);
  incidentpopulate.exec();
  if (incidentpopulate.first())
  {
    _name->setText(incidentpopulate.value("incdtcat_name").toString());
    _order->setValue(incidentpopulate.value("incdtcat_order").toInt());
    _descrip->setText(incidentpopulate.value("incdtcat_descrip").toString());
    _template->setId(incidentpopulate.value("incdtcat_tasktmpl_id").toInt());
    _ediprofile->setId(incidentpopulate.value("ediprofile").toInt());
  }
  else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Incident Category Information"),
                                incidentpopulate, __FILE__, __LINE__))
  {
    return;
  }
}
