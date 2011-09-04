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

typedef boost::function<void (const AtspiEvent *)> AccessibilityEventCallback;

typedef AtspiEvent A11yEvent;

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