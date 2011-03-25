/*
    Rekollect: A note taking application
    Copyright (C) 2009, 2010, 2011  Jason Jackson <jacksonje@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/
// Local
#include "rekollectapplication.h"

// KDE
#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>

static const char description[] =
    I18N_NOOP("A rich text note taking and management tool.");

static const char version[] = "0.3.3";

int main(int argc, char **argv)
{
    // Initialize application
    KAboutData about("rekollect", 0, ki18n("Rekollect"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2009 - 2011 Jason Jackson"), KLocalizedString(),
                     0, "jacksonje@gmail.com");
    about.addAuthor(ki18n("Jason Jackson"), KLocalizedString(), "jacksonje@gmail.com");
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("n").add("new", ki18n("Open a window with a newly created note"));
    options.add("+[file]", ki18n("Display the requested note"));
    KCmdLineArgs::addCmdLineOptions(options);
    KUniqueApplication::addCmdLineOptions();

    if (!RekollectApplication::start()) {
        return 0;
    }

    RekollectApplication app;
    return app.exec();
}
