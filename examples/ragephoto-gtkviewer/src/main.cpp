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
#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/filefilter.h>
#include <gtkmm/label.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/window.h>
#include <iostream>

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create(argc, argv, "de.syping.ragephoto.gtkviewer", Gio::APPLICATION_HANDLES_OPEN);

    Gtk::Window win;
    win.set_title("RagePhoto GTK Photo Viewer");
    win.set_default_size(400, 0);
    win.set_resizable(false);

    Gtk::Box vertical_box(Gtk::ORIENTATION_VERTICAL);
    vertical_box.set_margin_bottom(6);
    vertical_box.set_spacing(6);

    PhotoViewer photo_viewer(&win);
    vertical_box.add(photo_viewer);
    photo_viewer.show();

    Gtk::Box horizontal_box(Gtk::ORIENTATION_HORIZONTAL);
    horizontal_box.set_margin_left(6);
    horizontal_box.set_margin_right(6);
    horizontal_box.set_spacing(6);
    vertical_box.add(horizontal_box);

    Gtk::Button open_button;
    open_button.set_label("Open");
    open_button.set_hexpand(true);
    open_button.set_size_request(100);
    open_button.set_image_from_icon_name("document-open");
    open_button.signal_clicked().connect([&](){
        Gtk::FileChooserDialog dialog("Open Photo...", Gtk::FILE_CHOOSER_ACTION_OPEN);
        dialog.set_transient_for(win);

        dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        dialog.add_button("_Open", Gtk::RESPONSE_OK);

        Glib::RefPtr<Gtk::FileFilter> filter_photo = Gtk::FileFilter::create();
        filter_photo->set_name("GTA V Photo");
        filter_photo->add_pattern("PGTA5*");
        dialog.add_filter(filter_photo);

        int result = dialog.run();

        switch(result) {
        case Gtk::RESPONSE_OK: {
            std::string filename = dialog.get_filename();
            photo_viewer.open_file(filename.c_str());
            break;
        }
        default:
            break;
        }
    });
    horizontal_box.add(open_button);
    open_button.show();

    Gtk::Button close_button;
    close_button.set_label("Close");
    close_button.set_hexpand(true);
    close_button.set_size_request(100);
    close_button.set_image_from_icon_name("dialog-close");
    close_button.signal_clicked().connect([&](){
        win.close();
    });
    horizontal_box.add(close_button);
    close_button.show();

    app->signal_open().connect([&](const Gio::Application::type_vec_files &files, const Glib::ustring &hint) {
        if (files.size() == 1) {
            for (const auto &file : files) {
                photo_viewer.open_file(file->get_path().c_str());
            }
            app->add_window(win);
            win.show();
        }
        else {
            app->add_window(win);
            win.show();
            Gtk::MessageDialog msg(win, "Can't open multiple photos at once!", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
            msg.set_title("RagePhoto GTK Photo Viewer");
            msg.run();
        }
    });

    horizontal_box.show();
    vertical_box.show();

    win.add(vertical_box);

    return app->run(win);
}
