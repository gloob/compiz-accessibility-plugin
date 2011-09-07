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
        char *iface = (char *) g_array_index (ifaces, gchar *,i);
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
    for (int i = 0; i < (int) ents.size(); i++)
    {
        AccessibilityEntity *e = ents[i];
        if (type == e->is())
            return ents[i];
    }

    return NULL;
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

    return position;
}

CompPoint
AccessibilityComponent::getSize () const
{
    CompPoint size;
    GError *error = NULL;

    return size;
}

IfaceType
AccessibilityComponent::is ()
{
    return Component;
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
        (*it)->cb (event);
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

    /*
    //bool b = ATSPI_TYPE_ACCESSIBLE;
    AtspiAccessible *accessible = ATSPI_ACCESSIBLE (this->obj);
    bool is_accessible = ATSPI_IS_ACCESSIBLE (this->obj);
    //ATSPI_IS_ACCESSIBLE_CLASS(class);
    AtspiAccessibleClass *accessible_class = ATSPI_ACCESSIBLE_GET_CLASS(this->obj);
    */
    
    AtspiComponent *component = atspi_accessible_get_component (this->obj);
    if (component)
    {
        GError *error;

        AtspiRect *rect = atspi_component_get_extents (component, ATSPI_COORD_TYPE_SCREEN, &error);   

        if (!rect)
        {
            g_error_free (error);
            error = NULL;
        }
        else
        {
            compLogMessage ("Accessibility", CompLogLevelInfo, "Event type: %s (Component-extents: [%d, %d] [%d, %d])\n", this->name, rect->x, rect->y, rect->width, rect->height);
        }
    }
    
    AccessibleObject *object = new AccessibleObject (this->obj);
    AccessibilityEntity *e;
    
    if ((e = object->get(Component)))
        compLogMessage ("Accessibility", CompLogLevelInfo, "[]: %d\n.", e->is()); 
    
    return true;
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
                                         event_listener_generic_destroy);
    
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

    for (it = list.begin (); it != list.end (); it++)
        if ((*it)->id == handler)
            break;

    if (it == list.end ())
        return;

    // Unregister event.
    GError *error = NULL;

    if (atspi_event_listener_deregister ((*it)->event_listener, (*it)->event_type, &error))
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
    list.erase (it);

    delete (h);
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

    for (it = list.begin (); it != list.end (); it++)
    {
        unregisterEventHandler ((*it)->id);
    }
}

void
AccessibilityScreen::handleAccessibilityEvent (const AtspiEvent *event)
{

    
    AtspiAccessible *obj = event->source;
    GValue any_data = event->any_data;
    GError *error = NULL;

    char * obj_name = atspi_accessible_get_name (obj, &error);

    if (!obj_name)
        g_error_free (error);
    else
        compLogMessage ("Accessibility", CompLogLevelInfo,
                    "::handleAccessibilityEVent event->source->get_name: %s\n", obj_name);
    
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "::handleAccessibilityEVent event->type: %s\n", event->type);
    
    if (event->detail1)
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "::handleAccessibilityEVent event->detail1: %d\n", event->detail1);
    if (event->detail2)
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "::handleAccessibilityEVent event->detail2: %d\n", event->detail2);

    AccessibilityEvent *a11yEvent = new AccessibilityEvent (event);
    
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

void
event_listener_generic_destroy (void *data)
{
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "event_listener_focus_destroy\n");
}
