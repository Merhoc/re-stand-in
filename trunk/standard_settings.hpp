/*
 * Copyright (C) 2012 Heiko Metzger <www.heikometzger.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * re-stand-in
 *
 * Heiko Metzger
 * 07.05.2012
 * rsi.cpp
 */

#ifndef STANDARD_SETTINGS_HPP
#define STANDARD_SETTINGS_HPP
#include "rsi.h"

QString SQL_standard_settings = ""
        "PRAGMA foreign_keys=OFF;!#!"
        "BEGIN TRANSACTION;!#!"
        "CREATE TABLE `settings` (    `setting`   VARCHAR(10) NOT NULL,    `data`      TEXT NOT NULL);!#!"
        "INSERT INTO \"settings\" VALUES('uw1','');!#!"
        "INSERT INTO \"settings\" VALUES('uw2','');!#!"
        "INSERT INTO \"settings\" VALUES('active','0');!#!"
        "INSERT INTO \"settings\" VALUES('sh','0');!#!"
        "INSERT INTO \"settings\" VALUES('dform','dd-mm-yyyy');!#!"
        "INSERT INTO \"settings\" VALUES('int','5');!#!"
        "INSERT INTO \"settings\" VALUES('header','<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\r\n"
        "       \"http://www.w3.org/TR/html4/loose.dtd\">\r\n"
        "<html>\r\n"
        "<head>\r\n"
        "<title>Vertretungsplan</title>\r\n"
        "<style type=\"text/css\" media=\"screen\">\r\n"
        "\r\n"
        "    .table0{\r\n"
        "	border-collapse: collapse;\r\n"
        "	/*max-width:250px;*/\r\n"
        "	font-size:1.5em;\r\n"
        "    }\r\n"
        "    th {\r\n"
        "        border: 1px solid #000000;\r\n"
        "        empty-cells: show;\r\n"
        "    }\r\n"
        "    td {\r\n"
        "        border: 1px solid #000000;\r\n"
        "        empty-cells: show;\r\n"
        "    }\r\n"
        "    .row0 {\r\n"
        "        background-color: #BBBBBB;\r\n"
        "    }\r\n"
        "    .row1 {\r\n"
        "        background-color: #FFFFFF;\r\n"
        "    }\r\n"
        "    .col0 {\r\n"
        "        /*width: 3em;*/\r\n"
        "    }\r\n"
        "    .col1 {\r\n"
        "    /*    width: 3em;*/\r\n"
        "    }\r\n"
        "    .col2 {\r\n"
        "  /*      width: 2em;*/\r\n"
        "    }\r\n"
        "    .col3 {\r\n"
        "   /*    width: 3em;*/\r\n"
        "    }\r\n"
        "    .col4 {\r\n"
        "    /*    width: 7em;*/\r\n"
        "    }\r\n"
        "    .col5 {\r\n"
        "     /*   width: 10em;*/\r\n"
        "    }\r\n"
        "    .col6 {\r\n"
        "      /*  width: 10em;*/\r\n"
        "    }\r\n"
        "</style>\r\n"
        "</head>\r\n"
        "<body>\r\n"
        "<table style=\"width: 650px;\"><tr><td style=\"border-style: none;\">');!#!\r\n"
        "INSERT INTO \"settings\" VALUES('footer','\r\n"
        "</td></tr></table>\r\n"
        "</body>\r\n"
        "</html>');!#!"
        "CREATE TABLE `files` (    `filename`   VARCHAR(128) NOT NULL,    `lastchange` VARCHAR(128) NOT NULL);!#!"
        "CREATE TABLE `static` (    `search`   TEXT NOT NULL,    `set` TEXT NOT NULL);!#!"
        "INSERT INTO \"static\" VALUES('<p><big><b>','<strong>');!#!"
        "INSERT INTO \"static\" VALUES('</b></big><br>','<strong>');!#!"
        "INSERT INTO \"static\" VALUES('ä','&auml;');!#!"
        "INSERT INTO \"static\" VALUES('ü','&uuml;');!#!"
        "INSERT INTO \"static\" VALUES('ö','&ouml;');!#!"
        "INSERT INTO \"static\" VALUES('Ä','&Auml;');!#!"
        "INSERT INTO \"static\" VALUES('Ü','&Uuml;');!#!"
        "INSERT INTO \"static\" VALUES('Ö','&Ouml;');!#!"
        "INSERT INTO \"static\" VALUES('ß','&szlig;');!#!"
        "INSERT INTO \"static\" VALUES('Turbo-Vertretung 2009<br>','');!#!"
        "INSERT INTO \"static\" VALUES('Pausenaufsicht','P-Aufsicht');!#!"
        "INSERT INTO \"static\" VALUES('<dl><dd>',' ');!#!"
        "INSERT INTO \"static\" VALUES('</dd></dl>',' ');!#!"
        "INSERT INTO \"static\" VALUES('<p><br>','<p>');!#!"
        "CREATE TABLE `dynamic` (    `search`   TEXT NOT NULL,    `set` TEXT NOT NULL,    `maxval` TINYINT NOT NULL);!#!"
        "INSERT INTO \"dynamic\" VALUES('<tr>','<tr class=\"row%1\">',1);!#!"
        "INSERT INTO \"dynamic\" VALUES('<th>','<th class=\"col%1\">',6);!#!"
        "INSERT INTO \"dynamic\" VALUES('<table>','<table class=\"table%1\">',1);!#!"
        "COMMIT;";

#endif // STANDARD_SETTINGS_HPP
