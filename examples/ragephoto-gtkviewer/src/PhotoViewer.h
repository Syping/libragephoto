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

#ifndef PHOTOVIEWER_H
#define PHOTOVIEWER_H

#include <gtkmm/drawingarea.h>
#include <gtkmm/label.h>
#include <gdkmm/pixbuf.h>

class PhotoViewer : public Gtk::DrawingArea
{
public:
    PhotoViewer(Gtk::Label *title_label, Gtk::Label *json_label);
    void open_file(const char *filename);

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    Glib::RefPtr<Gdk::Pixbuf> p_image;
    Gtk::Label *p_title_label;
    Gtk::Label *p_json_label;
};

#endif // PHOTOVIEWER_H
