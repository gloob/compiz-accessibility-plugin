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
 * Author: Alejandro Leiva <gloob@litio.org>
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

	virtual IfaceType is ();
	virtual bool load (AtspiAccessible *);
	virtual bool contains (AccessibilityEntity *, int, int);

	AtspiAccessible *getObject ();

    protected:

	AtspiAccessible *obj;
};

class AccessibilityComponent :
    public AccessibilityEntity
{
    public:

	typedef boost::shared_ptr<AccessibilityComponent> Ptr;

	AccessibilityComponent (AtspiAccessible *);
	
	virtual IfaceType is ();

	CompRect getExtents () const;
	CompPoint getPosition () const;
	CompPoint getSize () const;
	
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

	virtual IfaceType is ();

	CompRect getCharacterExtents (int) const;
	CompRect getRangeExtents (int) const;
	int getCaretOffset ();

    protected:

	AtspiText          *text;
};

class AccessibleObjectInterfaceTypes
{
    public:

	AccessibleObjectInterfaceTypes ()
	{
	    mMap["Accessible"] = Accessible;
	    mMap["Action"] = Action;
	    mMap["Collection"] = Collection;
	    mMap["Component"] = Component;
	    mMap["Document"] = Document;
	    mMap["EditableText"] = EditableText;
	    mMap["Hypertext"] = Hypertext;
	    mMap["Hyperlink"] = Hyperlink;
	    mMap["Image"] = Image;
	    mMap["Selection"] = Selection;
	    mMap["Table"] = Table;
	    mMap["Text"] = Text;
	    mMap["Value"] = Value;
	}

	IfaceType get (const std::string &t) { return mMap[t]; }

    private:

	std::map <std::string, IfaceType> mMap;
};


class AccessibleObject
{
    public:        

	typedef std::vector <AccessibilityEntity::Ptr> Entities;
	typedef std::vector <IfaceType> Interfaces;

	AccessibleObject (AtspiAccessible *);
        
	AccessibleObject::Entities create (AtspiAccessible *);
	AccessibilityEntity::Ptr get (IfaceType);
	bool is (IfaceType);
	AccessibilityEntity::Ptr getEntity (IfaceType);

    private:

	AccessibilityEntity::Ptr instantiate (AtspiAccessible *, IfaceType);
	int getIfaceIndex (IfaceType);

    private:

	Entities            ents;
	Interfaces          interfaces;
	AtspiAccessible *   obj;

	static AccessibleObjectInterfaceTypes accessibleInterfaceTypes;
};

class AccessibilityEvent
{
    public:

	AccessibilityEvent (const AtspiEvent *);
	~AccessibilityEvent ();

	const char *getType ();
	AccessibleObject *getAccessibleObject ();

    private:

	const AtspiEvent  *event;
	AccessibleObject  *object;
};

typedef boost::function<void (AccessibilityEvent *)> AccessibilityEventCallback;

class Accessibility
{
    public:

	Accessibility () {}

	bool registerEventHandler (const char *, AccessibilityEventCallback);
	void unregisterAll ();

    friend class AccessibilityScreen;
};      

#endif // _ACCESSIBILITY_H
