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

#include <dbus/dbus.h>

#include "accessibility_options.h"

class AccessibilityScreen :
    public PluginClassHandler <AccessibilityScreen, CompScreen>,
    public ScreenInterface,
    public AccessibilityOptions
{
    public:

        AccessibilityScreen (CompScreen *);
        ~AccessibilityScreen ();

        CompScreen *screen;
        
        
};

class AccessibilityPluginVTable :
    public CompPlugin::VTableForScreen <AccessibilityScreen>
{
    public:

        bool init();
        
};