/*****************************************************************************
* libragephoto RAGE Photo Parser
* Copyright (C) 2021-2022 Syping
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

#include <RagePhoto.h>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>

bool readPhotoFile(const QString &filename, QMainWindow *mainWindow, QLabel *photoLabel)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        const QByteArray fileData = file.readAll();
        file.close();
        RagePhoto ragePhoto;
        const bool loaded = ragePhoto.load(fileData.data(), static_cast<size_t>(fileData.size()));
        if (!loaded) {
            const uint8_t error = ragePhoto.error();
            if (error <= RagePhoto::PhotoReadError) {
                QMessageBox::warning(mainWindow, "Open Photo", "Failed to read photo: " + filename);
                return false;
            }
        }
        const QByteArray photoData = QByteArray::fromRawData(ragePhoto.photoData(), ragePhoto.photoSize());
        const QImage image = QImage::fromData(photoData, "JPEG");
        photoLabel->setPixmap(QPixmap::fromImage(image));
        mainWindow->setWindowTitle(QStringLiteral("RagePhoto Qt Photo Viewer - ") + QString::fromUtf8(ragePhoto.title()));
        return true;
    }
    else {
        QMessageBox::warning(mainWindow, "Open Photo", "Failed to read file: " + filename);
    }
    return false;
}

int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x050600
#if QT_VERSION < 0x060000
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
#endif
    QApplication app(argc, argv);
    app.setApplicationName("ragephoto-qtviewer");

    QMainWindow mainWindow;
    mainWindow.setWindowTitle("RagePhoto Qt Photo Viewer");
    mainWindow.setFixedSize(400, 0);

    QWidget centralWidget(&mainWindow);
    mainWindow.setCentralWidget(&centralWidget);

    QVBoxLayout verticalLayout(&centralWidget);
    verticalLayout.setContentsMargins(0, 0, 0, 0);
    verticalLayout.setSpacing(6);

    QLabel photoLabel(&centralWidget);
    verticalLayout.addWidget(&photoLabel);

    QHBoxLayout horizontalLayout;
    horizontalLayout.setContentsMargins(6, 0, 6, 6);
    horizontalLayout.setSpacing(6);
    verticalLayout.addLayout(&horizontalLayout);

    QPushButton openButton("Open", &centralWidget);
    if (QIcon::hasThemeIcon("document-open"))
        openButton.setIcon(QIcon::fromTheme("document-open"));
    QObject::connect(&openButton, &QPushButton::clicked, &mainWindow, [&](){
        const QString filename = QFileDialog::getOpenFileName(&mainWindow, "Open Photo...", QString(), "RagePhoto compatible (PGTA5* PRDR3*)");
        if (filename.isEmpty())
            return;
        if (readPhotoFile(filename, &mainWindow, &photoLabel)) {
            QTimer::singleShot(0, &mainWindow, [&](){
                mainWindow.setFixedSize(mainWindow.sizeHint());
            });
        }
    });
    openButton.setAutoDefault(true);
    horizontalLayout.addWidget(&openButton);

    QPushButton closeButton("Close", &centralWidget);
    if (QIcon::hasThemeIcon("dialog-close"))
        closeButton.setIcon(QIcon::fromTheme("dialog-close"));
    QObject::connect(&closeButton, &QPushButton::clicked, &mainWindow, &QMainWindow::close);
    closeButton.setAutoDefault(true);
    horizontalLayout.addWidget(&closeButton);

    const QStringList args = app.arguments();
    if (args.length() == 2) {
        const QString filename = args.at(1);
        if (readPhotoFile(filename, &mainWindow, &photoLabel)) {
            QTimer::singleShot(0, &mainWindow, [&](){
                mainWindow.setFixedSize(mainWindow.sizeHint());
            });
        }
    }
    else if (args.length() > 2) {
        QMessageBox::warning(&mainWindow, "RagePhoto Qt Photo Viewer", "Can't open multiple photos at once!");
    }

    mainWindow.show();

    return app.exec();
}
