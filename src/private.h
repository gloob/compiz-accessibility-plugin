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


/*
 * Class hierarchy that hold and manage differents accessible objects.
 */
enum IfaceType
{
    Accessible = 0,
    Action,
    Collection,
    Component,
    Document,
    EditableText,
    Hypertext,
    Hyperlink,
    Image,
    Selection,
    Table,
    Text,
    Value,
};

static const char *IfaceTypeStr[] =
{
    "Accessible",
    "Action",
    "Collection",
    "Component",
    "Document",
    "EditableText",
    "Hypertext",
    "Hyperlink",
    "Image",
    "Selection",
    "Table",
    "Text",
    "Value",
};

#define NUM_IFACES_SUPPORTED 13

class AccessibilityObjectInterface {};

class AccessibilityEntity
{
    public:
        AccessibilityEntity (AtspiAccessible *);
        ~AccessibilityEntity ();

        virtual bool
        load (AtspiAccessible *);

        virtual bool
        contains (AccessibilityEntity *, int, int);

        virtual IfaceType
        is ();

    protected:
        AtspiAccessible   *obj;
};

class AccessibilityComponent :
    public AccessibilityEntity,
    public AccessibilityObjectInterface
{
    public:
        AccessibilityComponent (AtspiAccessible *);
        ~AccessibilityComponent ();

        CompRect
        getExtents () const;
        
        CompPoint
        getPosition () const;

        CompPoint
        getSize () const;

        IfaceType
        is ();

        /* TODO: Implement based in a compiz layer type.
        CompLayer
        getLayer ();
        */
    protected:
        AtspiComponent    *component;
};

class AccessibilityText :
    public AccessibilityEntity,
    public AccessibilityObjectInterface
{
    public:
        AccessibilityText (AtspiAccessible *);
        ~AccessibilityText ();

        CompRect
        getCharacterExtents (int) const;

        CompRect
        getRangeExtents (int) const;

        IfaceType
        is ();

    protected:
        AtspiText          *text;
};

class AccessibleObject
{
    public:        
        typedef std::vector <AccessibilityEntity *> Entities;
        typedef std::vector <IfaceType> Interfaces;

        AccessibleObject (AtspiAccessible *);
        
        AccessibleObject::Entities
        create (AtspiAccessible *);

        AccessibilityEntity *
        get (IfaceType);

        bool
        is (IfaceType);

    private:
        static AccessibilityEntity *
        instantiate (AtspiAccessible *, char *);

        static IfaceType
        enumFromStr (const char *);

        int
        getIfaceIndex (IfaceType);

    private:
        Entities     ents;
        Interfaces   interfaces;
};

class AccessibilityEvent
{
    public:
        //AccessibilityEvent ();
        AccessibilityEvent (const AtspiEvent *);
        ~AccessibilityEvent ();

        bool
        loadEvent (const AtspiEvent *);

        AccessibleObject *
        getAccessibleObject ();

    public:
        AtspiAccessible   *obj;
        char              *name;
        char              *type;
        int                detail1;
        int                detail2;

    private:
        const AtspiEvent  *event;
        AccessibleObject  *object;

};

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
        handleAccessibilityEvent (AccessibilityEvent *);

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
