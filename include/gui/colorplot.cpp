/*
 * colorplot.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: holger
 */

#include "colorplot.hpp"


static void global_activate(GtkApplication* app, gpointer user_data)
{
  GtkWindowInterface::instance().activate(app, user_data);
}
//static gboolean render (GtkGLArea *area, GdkGLContext *context)
//{
//  return GtkWindowInterface::instance().render(area, context);
//}

int my_timer_callback(gpointer pointer)
{
  static_cast<GtkWindowInterface*>(pointer)->updateCheck();
  return true;
}

GtkWindowInterface::WindowRunner::WindowRunner(GtkWindowInterface &interface_)
  : interface(interface_)
{}

void GtkWindowInterface::WindowRunner::operator()()
{
  interface.startGtkApplication();
}

GtkWindowInterface::GtkWindowInterface() {
  pixbuf = NULL;
  imageWidget = NULL;
  bufferUpdated = false;
  runner = boost::make_shared<WindowRunner>(boost::ref(*this));
  runner_thread = boost::thread(*runner);
  bufferA = pGridType(new GridType());
  bufferB = pGridType(new GridType());
}

void GtkWindowInterface::startGtkApplication()
{
  GtkApplication *app;
  int status;

  g_timeout_add (10, my_timer_callback, this);

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (global_activate), NULL);
  status = g_application_run (G_APPLICATION (app), 0, NULL);
  g_object_unref (app);
}



GtkWindowInterface::pGridType GtkWindowInterface::getBuffer()
{
  return bufferB;
}

void GtkWindowInterface::updateBuffer() {
  boost::lock_guard<boost::mutex> guard(mtx);
  pGridType tmp = bufferB;
  bufferB = bufferA;
  bufferA = tmp;
  bufferUpdated = true;
}

void GtkWindowInterface::updateCheck() {
  bool update;
  {
    boost::lock_guard<boost::mutex> guard(mtx);
    update = bufferUpdated;
    bufferUpdated = false;
  }
  if (update) {
    render();
//    gtk_widget_queue_draw(glRenderWidget);
  }
}

void GtkWindowInterface::activate (GtkApplication* app, gpointer user_data)
{
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  //GtkWidget *label;
  gtk_container_set_border_width(GTK_CONTAINER (window), 10);
  //label = gtk_label_new("And now for something completely different ...");
  //glRenderWidget = gtk_gl_area_new();
//  g_signal_connect (glRenderWidget, "render", G_CALLBACK (render), NULL);
  //gtk_container_add(GTK_CONTAINER(window), glRenderWidget);


}

gboolean GtkWindowInterface::render() //GtkGLArea *area, GdkGLContext *context)
{
  GridType &buffer = *bufferA;
  Image::IndexType lo = buffer.getLo();
  Image::IndexType hi = buffer.getHi();
  image.resize(lo, hi);

  {
    boost::lock_guard<boost::mutex> guard(mtx);
    GridType &buffer = *bufferA;
    for (int i=lo[0]; i<=hi[0]; ++i)
      for (int j=lo[1]; j<=hi[1]; ++j)
      {
        guchar c = (int)(5+ buffer(i,j) * 245);
        pixel &p = image(i,j);
        p.c[0] = c;
        p.c[1] = int(255)-c;
        p.c[2] = c;
      }
  }


  if (pixbuf==NULL)
  {
    std::cout << "Creating window!\n==================\n";
    pixbuf = gdk_pixbuf_new_from_data(image.getRawData()->c,
                                      GDK_COLORSPACE_RGB,
                                      false,
                                      8,
                                      hi[0]-lo[0]+1,
                                      hi[1]-lo[1]+1,
                                      3*(hi[1]-lo[1]+1),
                                      NULL,
                                      NULL);
    imageWidget = (GtkImage*)gtk_image_new_from_pixbuf (pixbuf);
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(imageWidget));
    gtk_widget_show_all (window);

    std::cout << "Channels: " << gdk_pixbuf_get_n_channels(pixbuf) << "\n";
  }
  else
  {
    gtk_image_set_from_pixbuf (imageWidget, pixbuf);
  }
  gtk_widget_queue_draw(GTK_WIDGET(imageWidget));
  return true;
}



  //
  // if (image==NULL) {
  //   image = new guchar[3*dims[0]*dims[1]];
  // }
  //
  // {
  //   boost::lock_guard<boost::mutex> guard(mtx);
  //   GridType &buffer = *bufferA;
  //   for (int i=lo[0]; i<=hi[0]; ++i)
  //     for (int j=lo[1]; j<=hi[1]; ++j)
  //     {
  //       double v = i/101.;
  //       guchar c = (int)(v /* buffer(i,j) */ * 255);
  //       uchar *p = image + y * rowstride + x * n_channels;
  //       pixel &p = image(i,j);
  //       p.c[0] = c;
  //       p.c[1] = 255-c;
  //       p.c[2] = c;
  //       p.c[3] = 255;
  //     }
  // }
