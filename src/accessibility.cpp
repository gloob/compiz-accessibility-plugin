/*
 * Compiz Accessibility PLugin
 *
 * Copyright (c) 2011 F123 Consulting & Mais Diferenças
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Author: Alejandro Leiva <aleiva@emergya.com>
 *
 */

#include "private.h"

COMPIZ_PLUGIN_20090315 (accessibility, AccessibilityPluginVTable);

AccessibleObject::AccessibleObject (AtspiAccessible *object)
{
    create (object);
}

AccessibleObject::Entities
AccessibleObject::create (AtspiAccessible *object)
{
    GArray * ifaces = atspi_accessible_get_interfaces (object);

    int len = (int) ifaces->len;

    for (int i = 0; i < len; i++) {

        char *iface = (char *) g_array_index (ifaces, gchar *, i);

        interfaces.push_back (enumFromStr (iface));
        ents.push_back (instantiate (object, iface));
    }

    g_array_free (ifaces, TRUE);

    return ents;
}

AccessibilityEntity *
AccessibleObject::instantiate (AtspiAccessible *object, char *iface)
{
    AccessibilityEntity *entity;
    int iface_id = (int) enumFromStr(iface);
    
    switch (iface_id) {

        case Component:
            entity = new AccessibilityComponent (object);
        break;

        case Accessible:
        case Action:
        case Collection:
        case Document:
        case EditableText:
        case Hypertext:
        case Hyperlink:
        case Image:
        case Selection:
        case Table:
        case Text:
        case Value:
        default:
            entity = new AccessibilityEntity (object);
    }

    return entity;
}

IfaceType
AccessibleObject::enumFromStr (const char *str)
{

    for (int i = 0; i < NUM_IFACES_SUPPORTED; i++)
        if (!strcmp (IfaceTypeStr[i], str))
            return (IfaceType) i;
    
    return Accessible;
}

AccessibilityEntity *
AccessibleObject::get (IfaceType type)
{
    int index = getIfaceIndex (type);

    if (index < 0)
        return NULL;

    return ents[index];
}

bool
AccessibleObject::is (IfaceType type)
{
    int index = getIfaceIndex (type);

    if (index < 0)
        return false;

    return true;
}

int
AccessibleObject::getIfaceIndex (IfaceType type)
{
    for (int i = 0; i < (int) ents.size(); i++)
    {
        AccessibilityEntity *e = ents[i];
        if (type == e->is())
            return i;
    }

    return -1;
}


AccessibilityEntity::AccessibilityEntity (AtspiAccessible *object)
{
    obj = object;
}

bool
AccessibilityEntity::load (AtspiAccessible *object)
{
    return true;
}

bool
AccessibilityEntity::contains (AccessibilityEntity *entity, int x, int y)
{
    return true;
}

IfaceType
AccessibilityEntity::is ()
{
    return Accessible;
}


AccessibilityComponent::AccessibilityComponent (AtspiAccessible *obj) :
    AccessibilityEntity (obj)
{
    component = atspi_accessible_get_component (obj);
}

CompRect
AccessibilityComponent::getExtents () const
{   
    CompRect rect;
    GError *error = NULL;

    AtspiRect *component_rect =
        atspi_component_get_extents (component, ATSPI_COORD_TYPE_SCREEN, &error);   
    
    if (!component_rect)
        g_error_free (error);
    else
        rect = CompRect (component_rect->x,
                         component_rect->y,
                         component_rect->width,
                         component_rect->height);

    return rect;
}

CompPoint
AccessibilityComponent::getPosition () const
{
    CompPoint position;
    GError *error = NULL;

    AtspiPoint *component_position =
        atspi_component_get_position (component, ATSPI_COORD_TYPE_SCREEN, &error);

    if (!component_position)
        g_error_free (error);
    else
        position = CompPoint (component_position->x, component_position->y);
       
    return position;
}

CompPoint
AccessibilityComponent::getSize () const
{
    CompPoint size;
    GError *error = NULL;

    AtspiPoint *component_size = atspi_component_get_size (component, &error);

    if (!component_size)
        g_error_free (error);
    else
        size = CompPoint (component_size->x, component_size->y);

    return size;
}

IfaceType
AccessibilityComponent::is ()
{
    return Component;
}


AccessibilityText::AccessibilityText (AtspiAccessible *obj) :
    AccessibilityEntity (obj)
{
    text = atspi_accessible_get_text (obj);
}

CompRect
AccessibilityText::getCharacterExtents (int offset) const
{
    CompRect rect;
    GError *error = NULL;

    AtspiRect *character_rect = atspi_text_get_character_extents
        (text, offset, ATSPI_COORD_TYPE_SCREEN, &error);

    if (!character_rect)
        g_error_free (error);
    else
        rect = CompRect (character_rect->x,
                         character_rect->y,
                         character_rect->width,
                         character_rect->height);

    return rect;
}

CompRect
AccessibilityText::getRangeExtents (int offset) const
{
    CompRect rect;
    GError *error = NULL;

    AtspiRect *range_rect = atspi_text_get_range_extents
        (text, 0, 0, ATSPI_COORD_TYPE_SCREEN, &error);

    if (!range_rect)
        g_error_free (error);
    else
        rect = CompRect (range_rect->x,
                         range_rect->y,
                         range_rect->width,
                         range_rect->height);

    return rect;
}

int
AccessibilityText::getCaretOffset ()
{
    GError *error = NULL;
    
    int caret_offset = atspi_text_get_caret_offset (text, &error);

    if (!caret_offset)
        g_error_free (error);

    return caret_offset;
}

IfaceType
AccessibilityText::is ()
{
    return Text;
}


Accessibility::Accessibility ()
{
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "Accessibility constructor called.\n");

}

Accessibility::~Accessibility ()
{
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "Accessibility destructor called.\n");

}

bool
Accessibility::start ()
{
    return true;
}

bool
Accessibility::stop ()
{
    return true;
}

bool
Accessibility::active ()
{
    return true;
}

bool
Accessibility::registerEventHandler (const char *event_type, AccessibilityEventCallback cb)
{
    ACCESSIBILITY_SCREEN (screen);

    as->registerEventHandler (event_type, cb);

    return true;
}

void
Accessibility::unregisterAll ()
{
    ACCESSIBILITY_SCREEN (screen);

    as->unregisterAll ();
}

void
staticAccessibilityEventCallback (const AtspiEvent *event)
{
    ACCESSIBILITY_SCREEN (screen);

    AccessibilityHandlerList list = as->list;

    std::list<AccessibilityHandler *>::iterator it;

    for (it = list.begin (); it != list.end (); it++)
    {
        compLogMessage ("Accessibility", CompLogLevelInfo,
                        "Delegating (%s) to -> functor [%d][%s]\n",
                        event->type, 
                        (*it)->id, (*it)->event_type);
        AccessibilityEvent *e = new AccessibilityEvent (event);
        (*it)->cb (e);
    }
}

void
staticAccessibilityEventDestroyCallback (void *data)
{
    ACCESSIBILITY_SCREEN (screen);

    AccessibilityHandlerList list = as->list;

    std::list<AccessibilityHandler *>::iterator it;

    for (it = list.begin (); it != list.end (); it++)
    {
        compLogMessage ("Accessibility", CompLogLevelInfo,
                        "Delegating destroy to -> functor [%d][%s]\n",
                        (*it)->id);
        // TODO: Implement callback mechanism for handles destroy.
    }
}

AccessibilityEvent::AccessibilityEvent (const AtspiEvent *event)
{
    this->loadEvent (event);    
}

AccessibilityEvent::~AccessibilityEvent ()
{
    ;
}

bool
AccessibilityEvent::loadEvent (const AtspiEvent *event)
{

    this->event = event;
    this->type = event->type;
    this->obj = event->source;

    GError *error = NULL;
    this->name = atspi_accessible_get_name (this->obj, &error);

    if (!this->name)
        g_error_free (error);    
    
    this->detail1 = event->detail1;
    this->detail2 = event->detail2;
    //this->any_data = GValue any_data = event->any_data;
        
    this->object = new AccessibleObject (this->obj);
    
    /*
    if (object->is (Component))
    {
        AccessibilityComponent *e = dynamic_cast<AccessibilityComponent *> (object->get (Component));
        compLogMessage ("Accessibility", CompLogLevelInfo, "[]: %d\n.", e->is ());
        CompRect component_rect = e->getExtents ();
        compLogMessage ("Accessibility", CompLogLevelInfo, "Event type: %s (Component-extents: [%d, %d] [%d, %d])\n", this->name, component_rect.x1(), component_rect.y1(), component_rect.x2(), component_rect.y2());
    }
    */
    
    return true;
}

AccessibleObject *
AccessibilityEvent::getAccessibleObject ()
{
    return object;
}

AccessibilityEventHandler
AccessibilityScreen::registerEventHandler (const char *event_type,
                                           AccessibilityEventCallback cb)
{

    AccessibilityHandler *hnd = new AccessibilityHandler ();

    if (!hnd)
        return 0;

    if (!event_type || !cb)
        return 0;

    // Create event listeners
    GError *error = NULL;

    AtspiEventListener *event_listener =
        atspi_event_listener_new_simple (staticAccessibilityEventCallback,
                                         staticAccessibilityEventDestroyCallback);
    
    if (!atspi_event_listener_register (event_listener, event_type, &error))
    {
        compLogMessage ("Accessibility", CompLogLevelInfo,
                        "Cannot create event listener (%s). [%s]\n",
                        event_type, 
                        error->message);
        
        g_error_free (error);
        error = NULL;
    }

    hnd->event_type     = event_type;
    hnd->event_listener = event_listener;
    hnd->cb             = cb;
    hnd->id             = lastEventHandler++;

    list.push_front (hnd);

    compLogMessage ("Accessibility", CompLogLevelInfo, "Registered new listener (%d): (%s))\n", hnd->id, hnd->event_type);
    
    return hnd->id;
}

void
AccessibilityScreen::unregisterEventHandler (AccessibilityEventHandler handler)
{
    
    std::list<AccessibilityHandler *>::iterator it;
    AccessibilityHandler *h;

    if (list.size() < 0)
        return;
        
    for (it = list.begin (); it != list.end (); it++)
        if ((*it)->id == handler)
            break;

    if (it == list.end ())
        return;

    // Unregister event.
    GError *error = NULL;

    if (!atspi_event_listener_deregister ((*it)->event_listener, (*it)->event_type, &error))
    {
        compLogMessage ("Accessibility", CompLogLevelInfo,
                        "Cannot unregister event listener (%s). [%s]\n",
                        (*it)->event_type,
                        error->message);
        
        g_error_free (error);
        error = NULL;
    }

    // Free and erase from the list.
    h = (*it);
    delete (h);
    
    list.erase (it);
}


bool
AccessibilityScreen::unregisterByType (const char * event_type)
{
    //TODO
    return true;
}

void
AccessibilityScreen::unregisterAll ()
{
    std::list<AccessibilityHandler *>::iterator it;

    if (list.size () < 0)
        return;

    for (it = list.begin (); it != list.end (); it++)
    {
        unregisterEventHandler ((*it)->id);
    }
}

void
AccessibilityScreen::handleAccessibilityEvent (AccessibilityEvent *event)
{

    //AccessibleObject *object = event->getAccessibleObject ();

    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "::handleAccessibilityEvent name: %s\n", event->name);
    
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "::handleAccessibilityEvent type: %s\n", event->type);
    
    if (event->detail1)
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "::handleAccessibilityEvent detail1: %d\n", event->detail1);
    if (event->detail2)
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "::handleAccessibilityEvent detail2: %d\n", event->detail2);
}

AccessibilityScreen::AccessibilityScreen (CompScreen *screen) :
    PluginClassHandler <AccessibilityScreen, CompScreen> (screen),
    screen (screen),
    lastEventHandler (0)
{
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "AccessibilityScreen called.\n");

    /* TODO: Check atspi_init() code. There's a memory leak when registryd
     * isn't running.
     */
    int atspi_status = atspi_init ();
	
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "AccessibilityScreen: AT-SPI init() %d.\n", atspi_status);

    registerEventHandler ("object:", boost::bind (
                    &AccessibilityScreen::handleAccessibilityEvent, this, _1));

    compLogMessage ("Accessibility", CompLogLevelInfo, "Running!\n");
        
    /*
    // Launch main event atspi loop
    // This is not needed because compiz private screen launches its own
    // Glib MainLoop
    */
    /*

    atspi_event_main();
    */
}

AccessibilityScreen::~AccessibilityScreen ()
{

    unregisterAll ();
    
    //atspi_event_quit();
    int atspi_status = atspi_exit ();

    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "~AccessibilityScreen called. Exit value: %d\n", atspi_status);
}

bool
AccessibilityPluginVTable::init ()
{
    if (!CompPlugin::checkPluginABI ("core", CORE_ABIVERSION))
    {
        compLogMessage ("Accessibility", CompLogLevelInfo,
                        "compiz core not in sync.\n");
        return false;
    }

    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "Running Accessibility plugin.\n");

    return true;
}
