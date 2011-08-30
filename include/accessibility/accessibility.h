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

#ifndef _ACCESSIBILITY_H
#define _ACCESSIBILITY_H

typedef boost::function<void (const AtspiEvent *)> AccessibilityEventCallback;

typedef int AccessibilityEventHandler;

/* Struct of a handler and list of handlers */
struct AccessibilityHandler {
    const char *                event_type;
    AtspiEventListener *        event_listener;
    AccessibilityEventCallback  cb;
    AccessibilityEventHandler   id;
};

typedef std::list<AccessibilityHandler *> AccessibilityHandlerList;

class Accessibility
{
    public:

        Accessibility ();
        ~Accessibility ();

        AccessibilityEventHandler
        registerEventHandler (const char * event_type,
                              AccessibilityEventCallback cb);
        
        void
        unregisterEventHandler (AccessibilityEventHandler handler);

        bool
        unregisterByType (const char * event_type);

    public:
        
        AccessibilityHandlerList list;
        int lastEventHandler;

    friend class AccessibilityScreen;
};      

#endif // _ACCESSIBILITY_H