//Autogenerated VP1 Factory Code Header File (Fri Aug 19 13:36:52 CEST 2016)

#ifndef VP1GeometryPlugin_VP1AutoFactory_H
#define VP1GeometryPlugin_VP1AutoFactory_H

#include <QObject>
#include <QStringList>

#include "VP1Base/IVP1ChannelWidgetFactory.h"

class VP1GeometryPlugin_VP1AutoFactory : public QObject, public IVP1ChannelWidgetFactory
{
  Q_OBJECT

  #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "VP1GeometryPlugin" FILE "") // apparently, FILE can be empty
  # else
    Q_EXPORT_PLUGIN2(pnp_vp1geometryplugin_vp1autofactory, VP1GeometryPlugin_VP1AutoFactory)
  #endif

  Q_INTERFACES(IVP1ChannelWidgetFactory)

public:
  virtual QStringList channelWidgetsProvided() const;
  virtual IVP1ChannelWidget * getChannelWidget(const QString & channelwidget);
};

#endif
