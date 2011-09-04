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

#include <core/core.h>
#include <core/pluginclasshandler.h>

#include <glibmm/main.h>

#include <accessibility/accessibility.h>
#include "accessibility_options.h"

#ifdef __cplusplus
extern "C" {
#endif

// Event listeners prototypes
void
event_listener_generic_destroy (void *data);

void
staticAccessibilityEventCallback (const AtspiEvent*);
#ifdef __cplusplus
}
#endif

typedef int AccessibilityEventHandler;

/* Struct of a handler and list of handlers */
struct AccessibilityHandler {
    const char *                event_type;
    AtspiEventListener *        event_listener;
    AccessibilityEventCallback  cb;
    AccessibilityEventHandler   id;
};

typedef std::list<AccessibilityHandler *> AccessibilityHandlerList;

class AccessibilityScreen :
    public PluginClassHandler <AccessibilityScreen, CompScreen>,
    public ScreenInterface,
    public AccessibilityOptions
{
    public:

        AccessibilityScreen (CompScreen *);
        ~AccessibilityScreen ();

        CompScreen *screen;

        AccessibilityEventHandler
        registerEventHandler (const char * event_type,
                              AccessibilityEventCallback cb);
        
        void
        unregisterEventHandler (AccessibilityEventHandler handler);

        bool
        unregisterByType (const char * event_type);

        void
        unregisterAll ();

        void
        handleAccessibilityEvent (const AtspiEvent*);

    public:
        
        AccessibilityHandlerList list;
        int lastEventHandler;

    protected:

        AtspiEventListener *listener;
        
        
};

#define ACCESSIBILITY_SCREEN(s) \
    AccessibilityScreen *as = AccessibilityScreen::get (s)

class AccessibilityPluginVTable :
    public CompPlugin::VTableForScreen <AccessibilityScreen>
{
    public:

        bool init();
        
};
