#include "itemmonitor.h"

#include "../libafanasy/msg.h"

#include "../libafqt/qenvironment.h"

#include "ctrlsortfilter.h"

#include <QtCore/QEvent>
#include <QtNetwork/QHostAddress>
#include <QtGui/QPainter>

#define AFOUTPUT
#undef AFOUTPUT
#include "../include/macrooutput.h"

const QString EventsName     = "Events[%1]:";
const QString UserIdName     = "User Id: %1";
const QString JobsIdsName    = "Jobs Ids[%1]:";
const QString TimeLaunch     = "L: %1";
const QString TimeRegister   = "R: %1";
const QString TimeActivity   = "A: %1";
const QString Address        = "IP=%1";

ItemMonitor::ItemMonitor( af::Monitor *monitor):
   ItemNode( (af::Monitor*)monitor)
{
   time_launch    = monitor->getTimeLaunch();
   time_register  = monitor->getTimeRegister();

   time_launch_str   = TimeLaunch     .arg( afqt::time2Qstr( time_launch  ));
   time_register_str = TimeRegister   .arg( afqt::time2Qstr( time_register));

   address_str = Address.arg( monitor->getAddress().generateIPString( true).c_str());
   engine = monitor->getEngine().c_str();

   updateValues( monitor, 0);
}

ItemMonitor::~ItemMonitor()
{
}

void ItemMonitor::updateValues( af::Node *node, int type)
{
   af::Monitor *monitor = (af::Monitor*)node;

   time_activity = monitor->getTimeActivity();
	if( time_activity )
		time_activity_str = TimeActivity.arg( afqt::time2Qstr( time_activity ));
	else
		time_activity_str = TimeActivity.arg( "no activity");

   events.clear();
   eventscount = 0;
   for( int e = 0; e < af::Monitor::EVT_COUNT; e++)
   {
      if( monitor->hasEvent(e))
      {
         events << af::Monitor::EVT_NAMES[e];
         eventscount ++;
      }
   }
   eventstitle = EventsName.arg( eventscount);
   m_height = 25 + 12*eventscount;
   if( m_height < 75) m_height = 75;

	m_user_id = monitor->getUid();
	m_user_id_str = UserIdName.arg( m_user_id);

   jobsids.clear();
   const std::list<int32_t> * jlist = monitor->getJobsIds();
   int jobsidscount = int( jlist->size());
   for( std::list<int32_t>::const_iterator it = jlist->begin(); it != jlist->end(); it++)
      jobsids += QString(" %1").arg( *it);
   jobsidstitle = JobsIdsName.arg( jobsidscount);

   tooltip = afqt::stoq( monitor->v_generateInfoString( true));
}

void ItemMonitor::paint( QPainter *painter, const QStyleOptionViewItem &option) const
{
   drawBack( painter, option, isSuperUser() ? &(afqt::QEnvironment::clr_LinkVisited.c) : NULL);

   int x = option.rect.x(); int y = option.rect.y(); int w = option.rect.width(); int h = option.rect.height();

   painter->setPen(   clrTextMain( option) );
   painter->setFont(  afqt::QEnvironment::f_name);
   painter->drawText( option.rect, Qt::AlignTop | Qt::AlignHCenter, m_name );

   painter->setPen(   clrTextInfo( option) );
   painter->setFont(  afqt::QEnvironment::f_info);
   painter->drawText( x+10, y+15, eventstitle );
   for( int e = 0; e < eventscount; e++)
      painter->drawText( x+5, y+30+12*e, events[e] );

   painter->setPen(   clrTextInfo( option) );
   painter->setFont(  afqt::QEnvironment::f_info);
   int i = y+2; int dy = 15;
   painter->drawText( x, i+=dy, w-5, h, Qt::AlignTop | Qt::AlignRight, time_launch_str );
   painter->drawText( x, i+=dy, w-5, h, Qt::AlignTop | Qt::AlignRight, time_register_str );
   painter->drawText( x, i+=dy, w-5, h, Qt::AlignTop | Qt::AlignRight, time_activity_str );

   painter->drawText( x, y, w-5, h, Qt::AlignBottom | Qt::AlignRight, address_str );

   i = y+2;
   painter->drawText( x, i+=dy, w-5, h, Qt::AlignTop | Qt::AlignHCenter, m_user_id_str );
   painter->drawText( x, i+=dy, w-5, h, Qt::AlignTop | Qt::AlignHCenter, jobsidstitle );
   painter->drawText( x, i+=dy, w-5, h, Qt::AlignTop | Qt::AlignHCenter, jobsids );

   painter->drawText( x, y+2, w-5, h, Qt::AlignTop | Qt::AlignRight, engine );
}

bool ItemMonitor::setSortType(   int type )
{
   resetSorting();
   switch( type )
   {
      case CtrlSortFilter::TNONE:
         return false;
      case CtrlSortFilter::TNAME:
         sort_str = m_name;
         break;
      case CtrlSortFilter::TTIMELAUNCHED:
         sort_int = time_launch;
         break;
      case CtrlSortFilter::TTIMEREGISTERED:
         sort_int = time_register;
         break;
      case CtrlSortFilter::TTIMEACTIVITY:
         sort_int = time_activity;
         break;
      case CtrlSortFilter::TENGINE:
         sort_str = engine;
         break;
      case CtrlSortFilter::TADDRESS:
         sort_str = address_str;
         break;
      default:
         AFERRAR("ItemMonitor::setSortType: Invalid type number = %d\n", type);
         return false;
   }
   return true;
}

bool ItemMonitor::setFilterType( int type )
{
   resetFiltering();
   switch( type )
   {
      case CtrlSortFilter::TNONE:
         return false;
      case CtrlSortFilter::TNAME:
         filter_str = m_name;
         break;
      case CtrlSortFilter::TENGINE:
         filter_str = engine;
         break;
      case CtrlSortFilter::TADDRESS:
         filter_str = address_str;
         break;
      default:
         AFERRAR("ItemMonitor::setFilterType: Invalid type number = %d\n", type);
         return false;
   }
   return true;
}
