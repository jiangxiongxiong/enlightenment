#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* Project */
#include "../include/elementaryxx/GenListItem.h"
#include "../include/elementaryxx/GenDataModel.h"

/* STD */
#include <iostream>
#include <cassert>

using namespace std;

namespace Elmxx {

GenListItem::GenListItem (Elm_Object_Item *item) :
  mItem (item)
{

}

GenListItem::~GenListItem ()
{
  cout << "GenListItem::~GenListItem" << endl;
  elm_object_item_del (mItem);
}

void GenListItem::setSelected (bool selected)
{
  elm_genlist_item_selected_set (reinterpret_cast<Elm_Object_Item *>(mItem), selected);
}

bool GenListItem::getSelected () const
{
  return elm_genlist_item_selected_get (reinterpret_cast<Elm_Object_Item *>(mItem));
}


void GenListItem::clearSubItems ()
{
  elm_genlist_item_subitems_clear (mItem);
}

void GenListItem::setExpanded (bool expanded)
{
  elm_genlist_item_expanded_set (mItem, expanded);
}

bool GenListItem::getExpanded () const
{
  return elm_genlist_item_expanded_get (mItem);
}

void GenListItem::setDisabled (bool disabled)
{
  elm_object_item_disabled_set (mItem, disabled);
}

bool GenListItem::getDisabled () const
{
  return elm_object_item_disabled_get (mItem);
}

void GenListItem::setDisplayOnly (bool displayOnly)
{
  elm_genlist_item_display_only_set (mItem, displayOnly);
}

bool GenListItem::getDisplayOnly () const
{
  return elm_genlist_item_display_only_get (mItem);
}

void GenListItem::show (Elm_Genlist_Item_Scrollto_Type type)
{
  elm_genlist_item_show (mItem, type);
}

void GenListItem::bringIn (Elm_Genlist_Item_Scrollto_Type type)
{
  elm_genlist_item_bring_in (mItem, type);
}

void GenListItem::showTop ()
{
  elm_genlist_item_top_show (mItem);
}

void GenListItem::bringInTop ()
{
  elm_genlist_item_top_bring_in (mItem);
}

void GenListItem::showMiddle ()
{
  elm_genlist_item_middle_show (mItem);
}

void GenListItem::bringInMiddle ()
{
  elm_genlist_item_middle_bring_in (mItem);
}

void GenListItem::update ()
{
  elm_genlist_item_update (mItem);
}

const Evasxx::Object *GenListItem::getEvasObject ()
{
  const Evas_Object *obj = elm_genlist_item_object_get (mItem);
  return Evasxx::Object::objectLink (obj);
}

GenListItem *GenListItem::wrap (Elm_Object_Item &item, GenDataModel &model)
{
  GenListItem *genItem = new GenListItem (&item);
  genItem->mDataModel = &model;
  //model.signalDel.connect (sigc::mem_fun (genItem, &GenItem::destroy));

  return genItem;
}

GenListItem *GenListItem::objectLink (const Elm_Object_Item *item)
{
  //GenItem *item2 = static_cast <GenItem*> (const_cast <void*> (elm_object_item_data_get(item)));
  //assert (item2);
  return NULL;//item2;
}

const void *GenListItem::getData ()
{
  return elm_object_item_data_get (mItem);
}

void GenListItem::setData (void *data)
{
  elm_object_item_data_set (mItem, data);
}
  
} // end namespace Elmxx
