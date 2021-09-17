/*****************************************************************************
* libragephoto RAGE Photo Parser
* Copyright (C) 2021 Syping
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* This software is provided as-is, no warranties are given to you, we are not
* responsible for anything with use of the software, you are self responsible.
*****************************************************************************/

#include "PhotoViewer.h"
#include <cairomm/context.h>
#include <gdkmm/general.h>
#include <gtkmm/messagedialog.h>
#include <RagePhoto.h>
#include <fstream>
#include <iostream>
#include <iterator>

PhotoViewer::PhotoViewer(Gtk::Window *win) : p_win(win)
{
}

void PhotoViewer::open_file(const char *filename)
{
    if (p_image)
        p_image.clear();

    RagePhoto ragePhoto;

    // Read file
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        Gtk::MessageDialog msg(*p_win, "Failed to open file: " + Glib::ustring(filename), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
        msg.set_title("Open Photo");
        msg.run();
        return;
    }
    std::string data(std::istreambuf_iterator<char>{ifs}, {});
    ifs.close();

    // Load Photo
    const bool loaded = ragePhoto.load(data);
    if (!loaded) {
        const RagePhoto::Error error = ragePhoto.error();
        if (error <= RagePhoto::Error::PhotoReadError) {
            Gtk::MessageDialog msg(*p_win, "Failed to read photo: " + Glib::ustring(filename), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
            msg.set_title("Open Photo");
            msg.run();
            return;
        }
    }

    GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
    gdk_pixbuf_loader_write(loader, reinterpret_cast<const guchar*>(ragePhoto.photoData()), ragePhoto.photoSize(), NULL);
    GdkPixbuf *c_pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
    gdk_pixbuf_loader_close(loader, NULL);
    p_image = Glib::wrap(c_pixbuf);

    p_win->set_title("RagePhoto GTK Photo Viewer - " + ragePhoto.title());

    if (p_image)
        set_size_request(p_image->get_width(), p_image->get_height());
}

bool PhotoViewer::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (!p_image)
        return false;

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    Gdk::Cairo::set_source_pixbuf(cr, p_image, (width - p_image->get_width()) / 2, (height - p_image->get_height()) / 2);
    cr->paint();

    return true;
}
