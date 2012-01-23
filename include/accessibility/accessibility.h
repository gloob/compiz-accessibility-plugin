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

#ifdef __cplusplus
extern "C" {
#endif
#include <atspi/atspi.h>
#ifdef __cplusplus
}
#endif

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

class AccessibilityEntity
{
    public:
        typedef boost::shared_ptr<AccessibilityEntity> Ptr;

        AccessibilityEntity (AtspiAccessible *);
        virtual ~AccessibilityEntity ();

        virtual bool
        load (AtspiAccessible *);

        virtual bool
        contains (AccessibilityEntity *, int, int);

        virtual IfaceType
        is ();

		virtual AccessibilityEntity *
		clone () const;

		AtspiAccessible *
		getObject ();

    protected:
        AtspiAccessible * obj;
};

class AccessibilityComponent :
    public AccessibilityEntity
{
    public:
        typedef boost::shared_ptr<AccessibilityComponent> Ptr;
        
        AccessibilityComponent (AtspiAccessible *);

		virtual AccessibilityComponent *
		clone () const;

        CompRect
        getExtents () const;
        
        CompPoint
        getPosition () const;

        CompPoint
        getSize () const;

        virtual IfaceType
        is ();

        /* TODO: Implement based in a compiz layer type.
        CompLayer
        getLayer ();
        */
    protected:
        AtspiComponent    *component;
};

class AccessibilityText :
    public AccessibilityEntity
{
    public:
        typedef boost::shared_ptr<AccessibilityText> Ptr;
        
        AccessibilityText (AtspiAccessible *);

		virtual AccessibilityText *
		clone () const;

        CompRect
        getCharacterExtents (int) const;

        CompRect
        getRangeExtents (int) const;

        int
        getCaretOffset ();

        virtual IfaceType
        is ();

    protected:
        AtspiText          *text;
};

class AccessibleObject
{
    public:        
        typedef std::vector <AccessibilityEntity::Ptr> Entities;
        typedef std::vector <IfaceType> Interfaces;

        AccessibleObject (AtspiAccessible *);
        
        AccessibleObject::Entities
        create (AtspiAccessible *);

        AccessibilityEntity::Ptr
        get (IfaceType);

        bool
        is (IfaceType);

        AccessibilityEntity::Ptr
        getEntity (IfaceType);

    private:
        AccessibilityEntity::Ptr
        instantiate (AtspiAccessible *, IfaceType);

        static IfaceType
        enumFromStr (const char *);

        int
        getIfaceIndex (IfaceType);

    private:
        Entities            ents;
        Interfaces          interfaces;
        AtspiAccessible *   obj;
};

class AccessibilityEvent
{
    public:
        AccessibilityEvent (const AtspiEvent *);
        ~AccessibilityEvent ();

        const char *
        getType ();

        AccessibleObject *
        getAccessibleObject ();

    private:
        const AtspiEvent  *event;
        AccessibleObject  *object;

};

typedef boost::function<void (AccessibilityEvent *)> AccessibilityEventCallback;

class Accessibility
{
    public:

        Accessibility ();
        ~Accessibility ();

        bool
        start ();

        bool
        stop ();

        bool
        active ();

        bool
        registerEventHandler (const char *, AccessibilityEventCallback);

        void
        unregisterAll ();

    friend class AccessibilityScreen;
};      

#endif // _ACCESSIBILITY_H