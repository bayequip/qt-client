/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */


#ifndef metrics_h
#define metrics_h

#include <QObject>
#include <QString>
#include <QMap>

class QScriptEngine;

typedef QMap<QString, QString> MetricMap;

class Parameters : public QObject
{
  Q_OBJECT

  protected:
    MetricMap _values;
    QString   _readSql;
    QString   _setSql;
    QString   _username;
    bool      _dirty;
    QString   _notifyName;

  public:
    Parameters(QObject * parent = 0);
    virtual ~Parameters() {};

    virtual void load();

    virtual QString value(const char *);
    virtual bool    boolean(const char *);

    Q_INVOKABLE virtual void set(const char *, bool);
    Q_INVOKABLE virtual void set(const QString &, bool);
    Q_INVOKABLE virtual void set(const char *, int);
    Q_INVOKABLE virtual void set(const QString &, int);
    Q_INVOKABLE virtual void set(const char *, const QString &);
    Q_INVOKABLE virtual void set(const QString &, const QString &);

    virtual QString parent(const QString &);

  public slots:
    virtual QString value(const QString &);
    virtual bool    boolean(const QString &);
    virtual void    sSetDirty(const QString &);

  protected:
    virtual void _set(const QString &, QVariant);

  signals:
    void loaded();

};

class Metrics : public Parameters
{
  Q_OBJECT

  public:
    Metrics();
};

class Preferences : public Parameters
{
  Q_OBJECT

  public:
    Preferences() {};
    Preferences(const QString &);

    void remove(const QString &);
};

class Privileges : public Parameters
{
  Q_OBJECT

  public:
    Privileges();

  public slots:
    bool check(const QString &);
    bool isDba();
};

void setupParameters(QScriptEngine *engine, QString name, Parameters *params);

#endif

