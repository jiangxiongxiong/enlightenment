/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

GtkWidget*
create_window (void)
{
  GtkWidget *window;
  GtkWidget *vbox1;
  GtkWidget *menubar1;
  GtkWidget *file;
  GtkWidget *file_menu;
  GtkAccelGroup *file_menu_accels;
  GtkWidget *open;
  GtkWidget *separator2;
  GtkWidget *sve;
  GtkWidget *separator1;
  GtkWidget *exit;
  GtkWidget *hpaned1;
  GtkWidget *scrolledwindow1;
  GtkWidget *list;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *vbox2;
  GtkWidget *hbox2;
  GtkWidget *label8;
  GtkWidget *type;
  GtkWidget *type_menu;
  GtkWidget *glade_menuitem;
  GtkWidget *frame1;
  GtkWidget *notebook1;
  GtkWidget *integer;
  GtkWidget *label1;
  GtkWidget *scrolledwindow2;
  GtkWidget *string;
  GtkWidget *label2;
  GtkObject *flot_adj;
  GtkWidget *flot;
  GtkWidget *label3;
  GtkWidget *label9;
  GtkWidget *label4;
  GtkWidget *empty_notebook_page;
  GtkWidget *label10;
  GtkWidget *hbox3;
  GtkWidget *add;
  GtkWidget *delete;
  GtkWidget *key;
  GtkWidget *hseparator1;
  GtkWidget *hbuttonbox2;
  GtkWidget *save;
  GtkWidget *cancel;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (window), "window", window);
  gtk_window_set_title (GTK_WINDOW (window), "E DB Editor");
  gtk_window_set_policy (GTK_WINDOW (window), FALSE, TRUE, TRUE);
  gtk_window_set_wmclass (GTK_WINDOW (window), "edb_gtk_ed", "main");

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window), vbox1);

  menubar1 = gtk_menu_bar_new ();
  gtk_widget_ref (menubar1);
  gtk_object_set_data_full (GTK_OBJECT (window), "menubar1", menubar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar1);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

  file = gtk_menu_item_new_with_label ("File");
  gtk_widget_ref (file);
  gtk_object_set_data_full (GTK_OBJECT (window), "file", file,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (file);
  gtk_container_add (GTK_CONTAINER (menubar1), file);

  file_menu = gtk_menu_new ();
  gtk_widget_ref (file_menu);
  gtk_object_set_data_full (GTK_OBJECT (window), "file_menu", file_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file), file_menu);
  file_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (file_menu));

  open = gtk_menu_item_new_with_label ("Open ...");
  gtk_widget_ref (open);
  gtk_object_set_data_full (GTK_OBJECT (window), "open", open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (open);
  gtk_container_add (GTK_CONTAINER (file_menu), open);

  separator2 = gtk_menu_item_new ();
  gtk_widget_ref (separator2);
  gtk_object_set_data_full (GTK_OBJECT (window), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (file_menu), separator2);
  gtk_widget_set_sensitive (separator2, FALSE);

  sve = gtk_menu_item_new_with_label ("Save");
  gtk_widget_ref (sve);
  gtk_object_set_data_full (GTK_OBJECT (window), "sve", sve,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (sve);
  gtk_container_add (GTK_CONTAINER (file_menu), sve);

  separator1 = gtk_menu_item_new ();
  gtk_widget_ref (separator1);
  gtk_object_set_data_full (GTK_OBJECT (window), "separator1", separator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator1);
  gtk_container_add (GTK_CONTAINER (file_menu), separator1);
  gtk_widget_set_sensitive (separator1, FALSE);

  exit = gtk_menu_item_new_with_label ("Exit");
  gtk_widget_ref (exit);
  gtk_object_set_data_full (GTK_OBJECT (window), "exit", exit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit);
  gtk_container_add (GTK_CONTAINER (file_menu), exit);

  hpaned1 = gtk_hpaned_new ();
  gtk_widget_ref (hpaned1);
  gtk_object_set_data_full (GTK_OBJECT (window), "hpaned1", hpaned1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hpaned1);
  gtk_box_pack_start (GTK_BOX (vbox1), hpaned1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hpaned1), 4);
  gtk_paned_set_gutter_size (GTK_PANED (hpaned1), 12);
  gtk_paned_set_position (GTK_PANED (hpaned1), 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_paned_pack1 (GTK_PANED (hpaned1), scrolledwindow1, TRUE, FALSE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  list = gtk_clist_new (3);
  gtk_widget_ref (list);
  gtk_object_set_data_full (GTK_OBJECT (window), "list", list,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (list);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), list);
  gtk_widget_set_usize (list, 300, -2);
  gtk_container_set_border_width (GTK_CONTAINER (list), 4);
  gtk_clist_set_column_width (GTK_CLIST (list), 0, 36);
  gtk_clist_set_column_width (GTK_CLIST (list), 1, 120);
  gtk_clist_set_column_width (GTK_CLIST (list), 2, 48);
  gtk_clist_set_selection_mode (GTK_CLIST (list), GTK_SELECTION_BROWSE);
  gtk_clist_column_titles_show (GTK_CLIST (list));

  label5 = gtk_label_new ("Type");
  gtk_widget_ref (label5);
  gtk_object_set_data_full (GTK_OBJECT (window), "label5", label5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label5);
  gtk_clist_set_column_widget (GTK_CLIST (list), 0, label5);

  label6 = gtk_label_new ("Key");
  gtk_widget_ref (label6);
  gtk_object_set_data_full (GTK_OBJECT (window), "label6", label6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label6);
  gtk_clist_set_column_widget (GTK_CLIST (list), 1, label6);

  label7 = gtk_label_new ("Value");
  gtk_widget_ref (label7);
  gtk_object_set_data_full (GTK_OBJECT (window), "label7", label7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label7);
  gtk_clist_set_column_widget (GTK_CLIST (list), 2, label7);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_paned_pack2 (GTK_PANED (hpaned1), vbox2, FALSE, FALSE);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox2);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbox2", hbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, FALSE, TRUE, 0);

  label8 = gtk_label_new ("Type:");
  gtk_widget_ref (label8);
  gtk_object_set_data_full (GTK_OBJECT (window), "label8", label8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox2), label8, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label8), 4, 4);

  type = gtk_option_menu_new ();
  gtk_widget_ref (type);
  gtk_object_set_data_full (GTK_OBJECT (window), "type", type,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (type);
  gtk_box_pack_start (GTK_BOX (hbox2), type, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (type), 4);
  type_menu = gtk_menu_new ();
  glade_menuitem = gtk_menu_item_new_with_label ("");
  gtk_widget_show (glade_menuitem);
  gtk_menu_append (GTK_MENU (type_menu), glade_menuitem);
  gtk_option_menu_set_menu (GTK_OPTION_MENU (type), type_menu);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_ref (frame1);
  gtk_object_set_data_full (GTK_OBJECT (window), "frame1", frame1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox2), frame1, TRUE, TRUE, 0);

  notebook1 = gtk_notebook_new ();
  gtk_widget_ref (notebook1);
  gtk_object_set_data_full (GTK_OBJECT (window), "notebook1", notebook1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (notebook1);
  gtk_container_add (GTK_CONTAINER (frame1), notebook1);
  gtk_container_set_border_width (GTK_CONTAINER (notebook1), 4);
  GTK_WIDGET_UNSET_FLAGS (notebook1, GTK_CAN_FOCUS);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook1), FALSE);
  gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook1), FALSE);
  gtk_notebook_set_tab_hborder (GTK_NOTEBOOK (notebook1), 0);
  gtk_notebook_set_tab_vborder (GTK_NOTEBOOK (notebook1), 0);

  integer = gtk_entry_new ();
  gtk_widget_ref (integer);
  gtk_object_set_data_full (GTK_OBJECT (window), "integer", integer,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (integer);
  gtk_container_add (GTK_CONTAINER (notebook1), integer);

  label1 = gtk_label_new ("label1");
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (window), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow2);
  gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow2", scrolledwindow2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow2);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow2);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow2), 4);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  string = gtk_text_new (NULL, NULL);
  gtk_widget_ref (string);
  gtk_object_set_data_full (GTK_OBJECT (window), "string", string,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (string);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), string);
  gtk_text_set_editable (GTK_TEXT (string), TRUE);

  label2 = gtk_label_new ("label2");
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (window), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

  flot_adj = gtk_adjustment_new (0, -1e+16, 1e+16, 1, 10, 10);
  flot = gtk_spin_button_new (GTK_ADJUSTMENT (flot_adj), 0.1, 3);
  gtk_widget_ref (flot);
  gtk_object_set_data_full (GTK_OBJECT (window), "float", flot,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (flot);
  gtk_container_add (GTK_CONTAINER (notebook1), flot);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (flot), TRUE);

  label3 = gtk_label_new ("label3");
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (window), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);

  label9 = gtk_label_new ("This value contains\nbinary data that is\nspecially encoded\nby the application\nusing this database\nand cannot be edited\nby this generic tool.");
  gtk_widget_ref (label9);
  gtk_object_set_data_full (GTK_OBJECT (window), "label9", label9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label9);
  gtk_container_add (GTK_CONTAINER (notebook1), label9);

  label4 = gtk_label_new ("label4");
  gtk_widget_ref (label4);
  gtk_object_set_data_full (GTK_OBJECT (window), "label4", label4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label4);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), label4);

  empty_notebook_page = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (empty_notebook_page);
  gtk_container_add (GTK_CONTAINER (notebook1), empty_notebook_page);

  label10 = gtk_label_new ("label10");
  gtk_widget_ref (label10);
  gtk_object_set_data_full (GTK_OBJECT (window), "label10", label10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label10);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 4), label10);

  hbox3 = gtk_hbox_new (TRUE, 0);
  gtk_widget_ref (hbox3);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbox3", hbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox3, FALSE, FALSE, 0);

  add = gtk_button_new_with_label ("Add");
  gtk_widget_ref (add);
  gtk_object_set_data_full (GTK_OBJECT (window), "add", add,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (add);
  gtk_box_pack_start (GTK_BOX (hbox3), add, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (add), 4);

  delete = gtk_button_new_with_label ("Delete");
  gtk_widget_ref (delete);
  gtk_object_set_data_full (GTK_OBJECT (window), "delete", delete,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (delete);
  gtk_box_pack_start (GTK_BOX (hbox3), delete, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (delete), 4);

  key = gtk_entry_new ();
  gtk_widget_ref (key);
  gtk_object_set_data_full (GTK_OBJECT (window), "key", key,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (key);
  gtk_box_pack_start (GTK_BOX (vbox1), key, FALSE, FALSE, 4);
  gtk_entry_set_editable (GTK_ENTRY (key), FALSE);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_ref (hseparator1);
  gtk_object_set_data_full (GTK_OBJECT (window), "hseparator1", hseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, FALSE, FALSE, 0);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox2);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbuttonbox2", hbuttonbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox2, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox2), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox2), 10);

  save = gtk_button_new_with_label ("Save");
  gtk_widget_ref (save);
  gtk_object_set_data_full (GTK_OBJECT (window), "save", save,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (save);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), save);
  GTK_WIDGET_SET_FLAGS (save, GTK_CAN_DEFAULT);

  cancel = gtk_button_new_with_label ("Quit");
  gtk_widget_ref (cancel);
  gtk_object_set_data_full (GTK_OBJECT (window), "cancel", cancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), cancel);
  GTK_WIDGET_SET_FLAGS (cancel, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                      GTK_SIGNAL_FUNC (on_window_delete_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (window), "destroy_event",
                      GTK_SIGNAL_FUNC (on_window_destroy_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (open), "activate",
                      GTK_SIGNAL_FUNC (on_open_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (sve), "activate",
                      GTK_SIGNAL_FUNC (on_save_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit), "activate",
                      GTK_SIGNAL_FUNC (on_exit_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (list), "select_row",
                      GTK_SIGNAL_FUNC (on_list_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (list), "unselect_row",
                      GTK_SIGNAL_FUNC (on_list_unselect_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (integer), "changed",
                      GTK_SIGNAL_FUNC (on_integer_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (string), "changed",
                      GTK_SIGNAL_FUNC (on_string_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (flot), "changed",
                      GTK_SIGNAL_FUNC (on_float_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (add), "clicked",
                      GTK_SIGNAL_FUNC (on_add_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (delete), "clicked",
                      GTK_SIGNAL_FUNC (on_delete_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (save), "clicked",
                      GTK_SIGNAL_FUNC (on_save_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel), "clicked",
                      GTK_SIGNAL_FUNC (on_cancel_clicked),
                      NULL);

  return window;
}

GtkWidget*
create_keyname (void)
{
  GtkWidget *keyname;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox4;
  GtkWidget *label11;
  GtkWidget *key;
  GtkWidget *dialog_action_area1;
  GtkWidget *ok;

  keyname = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (keyname), "keyname", keyname);
  gtk_window_set_title (GTK_WINDOW (keyname), "New Key Name");
  GTK_WINDOW (keyname)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_position (GTK_WINDOW (keyname), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (keyname), TRUE);
  gtk_window_set_policy (GTK_WINDOW (keyname), FALSE, FALSE, TRUE);
  gtk_window_set_wmclass (GTK_WINDOW (keyname), "edb_gtk_ed", "new_key");

  dialog_vbox1 = GTK_DIALOG (keyname)->vbox;
  gtk_object_set_data (GTK_OBJECT (keyname), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox4);
  gtk_object_set_data_full (GTK_OBJECT (keyname), "hbox4", hbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox4, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox4), 8);

  label11 = gtk_label_new ("New Key:");
  gtk_widget_ref (label11);
  gtk_object_set_data_full (GTK_OBJECT (keyname), "label11", label11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox4), label11, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label11), 8, 8);

  key = gtk_entry_new ();
  gtk_widget_ref (key);
  gtk_object_set_data_full (GTK_OBJECT (keyname), "key", key,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (key);
  gtk_box_pack_start (GTK_BOX (hbox4), key, TRUE, TRUE, 0);
  GTK_WIDGET_SET_FLAGS (key, GTK_CAN_DEFAULT);

  dialog_action_area1 = GTK_DIALOG (keyname)->action_area;
  gtk_object_set_data (GTK_OBJECT (keyname), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  ok = gtk_button_new_with_label ("OK");
  gtk_widget_ref (ok);
  gtk_object_set_data_full (GTK_OBJECT (keyname), "ok", ok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), ok, TRUE, TRUE, 0);

  gtk_signal_connect (GTK_OBJECT (ok), "clicked",
                      GTK_SIGNAL_FUNC (on_new_key_ok_clicked),
                      NULL);

  gtk_widget_grab_focus (key);
  gtk_widget_grab_default (key);
  return keyname;
}

GtkWidget*
create_fileselection (void)
{
  GtkWidget *fileselection;
  GtkWidget *ok_button1;
  GtkWidget *cancel_button1;

  fileselection = gtk_file_selection_new ("Select File");
  gtk_object_set_data (GTK_OBJECT (fileselection), "fileselection", fileselection);
  gtk_container_set_border_width (GTK_CONTAINER (fileselection), 2);
  GTK_WINDOW (fileselection)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_modal (GTK_WINDOW (fileselection), TRUE);
  gtk_window_set_wmclass (GTK_WINDOW (fileselection), "edb_gtk_ed", "file_selector");

  ok_button1 = GTK_FILE_SELECTION (fileselection)->ok_button;
  gtk_object_set_data (GTK_OBJECT (fileselection), "ok_button1", ok_button1);
  gtk_widget_show (ok_button1);
  GTK_WIDGET_SET_FLAGS (ok_button1, GTK_CAN_DEFAULT);

  cancel_button1 = GTK_FILE_SELECTION (fileselection)->cancel_button;
  gtk_object_set_data (GTK_OBJECT (fileselection), "cancel_button1", cancel_button1);
  gtk_widget_show (cancel_button1);
  GTK_WIDGET_SET_FLAGS (cancel_button1, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (fileselection), "delete_event",
                      GTK_SIGNAL_FUNC (on_fileselection_delete_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (fileselection), "destroy_event",
                      GTK_SIGNAL_FUNC (on_fileselection_destroy_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (ok_button1), "clicked",
                      GTK_SIGNAL_FUNC (on_ok_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel_button1), "clicked",
                      GTK_SIGNAL_FUNC (on_cancel_button_clicked),
                      NULL);

  return fileselection;
}

