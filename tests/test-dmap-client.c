/*
 * Copyright (C) 2006 INDT
 *  Andre Moreira Magalhaes <andre.magalhaes@indt.org.br>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include "test-dmap-db.h"
#include "test-daap-record-factory.h"
#include "test-dpap-record-factory.h"

#include <libdmapsharing/daap-connection.h>
#include <libdmapsharing/dpap-connection.h>
#include <libdmapsharing/dmap-mdns-browser.h>

enum {
    DAAP,
    DPAP
};

static GMainLoop *loop;
static guint conn_type = DAAP;

/*
static gboolean
disconnect_cb (DMAPConnection *conn)
{
    dmap_connection_disconnect (conn);
    g_object_unref (conn);
    g_main_loop_quit (loop);
    return FALSE;
}
*/

static void
print_record (gpointer id, DMAPRecord *record, gpointer user_data)
{
	gchar *artist, *title;

	g_object_get (record,
		     "artist", &artist,
		     "title",  &title,
		     NULL);

	g_print ("%d: %s %s\n", GPOINTER_TO_UINT (id), artist, title);
}

static void
connected_cb (DMAPConnection *connection,
			 gboolean        result,
			 const char     *reason,
			 DMAPDb         *db)
{
	g_print ("Connection cb., DB has %lu entries\n", dmap_db_count (db));

	dmap_db_foreach (db, print_record, NULL);

	g_main_loop_quit (loop);
}

static void
service_added_cb (DMAPMdnsBrowser *browser,
                  DMAPMdnsBrowserService *service,
                  gpointer user_data)
{
    DMAPRecordFactory *factory;
    DMAPConnection *conn;
    DMAPDb *db;

    g_debug ("service added %s:%s:%s:%d (%s)",
             service->service_name,
             service->name,
             service->host,
             service->port,
             service->password_protected ? "protected" : "not protected");

    db = DMAP_DB (test_dmap_db_new ());
    if (db == NULL) {
    	g_error ("Error creating DB");
    }

    if (conn_type == DAAP) {
        factory = DMAP_RECORD_FACTORY (test_daap_record_factory_new ());
        if (factory == NULL) {
   	    g_error ("Error creating record factory");
        }
        conn = DMAP_CONNECTION (daap_connection_new (service->name, service->host, service->port, FALSE, db, factory));
    } else {
        factory = DMAP_RECORD_FACTORY (test_dpap_record_factory_new ());
        if (factory == NULL) {
   	    g_error ("Error creating record factory");
        }
        conn = DMAP_CONNECTION (dpap_connection_new (service->name, service->host, service->port, FALSE, db, factory));
    }
    dmap_connection_connect (DMAP_CONNECTION (conn), (DMAPConnectionCallback) connected_cb, db);
}

int main(int argc, char **argv)
{
    DMAPMdnsBrowser *browser;
    GError *error = NULL;

    if (argc == 2)
        conn_type = atoi (argv[1]);

    g_type_init ();
    g_thread_init (NULL);

    loop = g_main_loop_new (NULL, FALSE);

    if (conn_type == DAAP)
        browser = dmap_mdns_browser_new (DMAP_MDNS_BROWSER_SERVICE_TYPE_DAAP);
    else
        browser = dmap_mdns_browser_new (DMAP_MDNS_BROWSER_SERVICE_TYPE_DPAP);
    g_signal_connect (G_OBJECT (browser),
                      "service-added",
                      G_CALLBACK (service_added_cb),
                      NULL);
    g_debug ("starting mdns browser");
    dmap_mdns_browser_start (browser, &error);
    if (error) {
        g_warning ("error starting browser. code: %d message: %s",
                error->code,
                error->message);
        return 1;
    }

    g_main_loop_run (loop);

    return 0;
}
