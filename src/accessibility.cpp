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

AccessibilityEventHandler
Accessibility::registerEventHandler (const char *event_type,
                                     AccessibilityEventCallback cb)
{

    AccessibilityHandler *hnd = new AccessibilityHandler();
    if (!hnd)
        return 0;

    if (!event_type || !cb)
        return 0;

    // Create event listeners
    GError *error = NULL;

    AtspiEventListenerSimpleCB cb_ref = *(cb.target<AtspiEventListenerSimpleCB>());
    
    AtspiEventListener *event_listener =
        atspi_event_listener_new_simple (cb_ref, event_listener_generic_destroy);
    
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

    return hnd->id;
}

void
Accessibility::unregisterEventHandler (AccessibilityEventHandler handler)
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
Accessibility::unregisterByType (const char * event_type) {
    //TODO
    return true;
}

AccessibilityScreen::AccessibilityScreen (CompScreen *screen) :
    PluginClassHandler <AccessibilityScreen, CompScreen> (screen),
    screen (screen)
{
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "AccessibilityScreen called.\n");

    /* TODO: Check atspi_init() code. There's a memory leak when registryd
     * isn't running.
     */
    int atspi_status = atspi_init ();
	
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "AccessibilityScreen: AT-SPI init() %d.\n", atspi_status);

    // Create event listeners
    GError *error = NULL;

    AtspiEventListener *generic_listener = atspi_event_listener_new_simple 
        (event_listener_generic, event_listener_generic_destroy);

    listener = generic_listener;
    
    if (!atspi_event_listener_register (generic_listener, "object:", &error))
    {
        compLogMessage ("Accessibility", CompLogLevelInfo,
                        "Cannot create event listener. [%s]\n", error->message);
        
        g_error_free (error);
        error = NULL;
    }

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
    GError *error = NULL;
    
    if (atspi_event_listener_deregister (listener, "object:", &error))
    {
        compLogMessage ("Accessibility", CompLogLevelInfo,
                        "Cannot unregister event listener. [%s]\n", error->message);
        
        g_error_free (error);
        error = NULL;
    }

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
event_listener_generic (const AtspiEvent *event)
{
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "event->type: %s\n", event->type);

    // object:children-changed:add
    // source: desktop frame | main
    // application: none
}

void
event_listener_generic_destroy (void *data)
{
    compLogMessage ("Accessibility", CompLogLevelInfo,
                    "event_listener_focus_destroy\n");
}
