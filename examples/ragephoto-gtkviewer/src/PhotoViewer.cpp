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
#include <RagePhoto.h>
#include <iostream>

PhotoViewer::PhotoViewer(Gtk::Label *title_label, Gtk::Label *json_label) : p_title_label(title_label), p_json_label(json_label)
{
}

void PhotoViewer::open_file(const char *filename)
{
    RagePhoto ragePhoto;
    // Read file
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return;
    }
    const int fseek_end_value = fseek(file, 0, SEEK_END);
    if (fseek_end_value == -1) {
        fclose(file);
        return;
    }
    const size_t file_size = ftell(file);
    if (file_size == -1) {
        fclose(file);
        return;
    }
    const int fseek_set_value = fseek(file, 0, SEEK_SET);
    if (fseek_set_value == -1) {
        fclose(file);
        return;
    }
    char *data = static_cast<char*>(malloc(file_size));
    const size_t file_rsize = fread(data, 1, file_size, file);
    if (file_size != file_rsize) {
        fclose(file);
        return;
    }
    fclose(file);
    const bool loaded = ragePhoto.load(data, file_size);
    free(data);
    if (!loaded) {
        const RagePhoto::Error error = ragePhoto.error();
        if (error <= RagePhoto::Error::PhotoReadError)
            return;
    }

    guchar *photoData = static_cast<guchar*>(malloc(ragePhoto.photoSize()));
    if (!photoData)
        return;

    memcpy(photoData, ragePhoto.photoData(), ragePhoto.photoSize());

    GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
    gdk_pixbuf_loader_write(loader, photoData, static_cast<gsize>(ragePhoto.photoSize()), nullptr);
    GdkPixbuf *c_pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
    p_image = Glib::wrap(c_pixbuf);

    p_title_label->set_text("Title: " + ragePhoto.title());
    // p_title_label->show();

    get_parent_window()->set_title("RagePhoto GTK Photo Viewer - " + ragePhoto.title());

    free(photoData);

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
