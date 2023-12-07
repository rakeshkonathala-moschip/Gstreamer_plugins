/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2023 Rakesh Konathala <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-logoimpose
 *
 * FIXME:Describe logoimpose here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! logoimpose ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gst/gst.h>
#include <gst/video/video.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <cairo/cairo.h>
#include <cairo/cairo-gobject.h>
#include <gtk/gtk.h>
#include "gstlogoimpose.h"
#include "gstlogoimposemacros.h"
#include<math.h>
GST_DEBUG_CATEGORY_STATIC (gst_logoimpose_debug);
#define GST_CAT_DEFAULT gst_logoimpose_debug
/* Filter signals and args */
enum
{
	/* FILL ME */
	LAST_SIGNAL
};
//all the required properties for logoimpose plugin
enum
{
	PROP_0,
	PROP_XCO,
	PROP_YCO,
	PROP_PATH,
	PROP_ALPHA,
	PROP_SPEED,
	PROP_ANIMATION,
	PROP_DIRECTION
};
/* the capabilities of the inputs and outputs.
 * describe the real formats here.
 * defining the static pad templates
 * creates src and sink templates*/
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",GST_PAD_SINK,GST_PAD_ALWAYS,GST_STATIC_CAPS ("ANY"));
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",GST_PAD_SRC,GST_PAD_ALWAYS,GST_STATIC_CAPS ("ANY"));
/* the properties and behaviour of parent class is inherited to our plugin using this macro*/
#define gst_logoimpose_parent_class parent_class
//it sets up the type hierarchy, initializes function pointers,and provides default implementations for object initialization and class initialization.
G_DEFINE_TYPE (GstLogoimpose, gst_logoimpose, GST_TYPE_ELEMENT);
//used to define new gstreamer element
GST_ELEMENT_REGISTER_DEFINE (logoimpose, "logoimpose", GST_RANK_NONE,GST_TYPE_LOGOIMPOSE);

//Function signatures.....
static void gst_logoimpose_set_property (GObject * object,guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_logoimpose_get_property (GObject * object,guint prop_id, GValue * value, GParamSpec * pspec);
static gboolean gst_logoimpose_sink_event (GstPad * pad,GstObject * parent, GstEvent * event);
static GstFlowReturn gst_logoimpose_chain (GstPad * pad,GstObject * parent, GstBuffer * buf);
void rotation_functionality(guint8 *y_pixels, guint8 *uv_pixels, guint y_stride, guint uv_stride, guint width, guint height, const gchar *png_path, guint rotation_angle,guint64 frameno,const gchar *speed,guint xco,guint yco,const gchar *direction,double alpha_factor);
void imposelogo_scroll(guint8 *y_pixels, guint8 *uv_pixels, guint y_stride, guint uv_stride, guint width, guint height, const gchar *png_path,guint xco,guint yco,guint64 frameno,double alpha_factor,const gchar *speed,const gchar* direction);
void imposelogo_normal(guint8 *y_pixels, guint8 *uv_pixels, guint y_stride, guint uv_stride, guint width, guint height, const gchar *png_path,guint xco,guint yco,guint64 frameno,double alpha_factor,const gchar *direction);


//______________________________________________________________________________________________________________________
/* it  GObject vmethod implementations 
 * initialize the logoimpose's class
 * Second entry point of every plugin:class init
 * It is the GObjectClass  variable is a pointer to the class structure of a GObject-derived class. 
 * It represents the blueprint or definition of a class and contains function pointers, class-specific data. 	 
 * information necessary for working with instances of that class. 
 */
static void gst_logoimpose_class_init (GstLogoimposeClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;
	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;
	gobject_class->set_property = gst_logoimpose_set_property;
	gobject_class->get_property = gst_logoimpose_get_property;


	g_object_class_install_property (gobject_class, PROP_XCO,g_param_spec_int ("xco", "X-co", "required point on x-axis,it must be in boundary as per framesize",
				LOGOIMPOSE_XCO_MIN,LOGOIMPOSE_XCO_MAX,LOGOIMPOSE_XCO_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_YCO,g_param_spec_int ("yco", "Y-co", "required point on y-axis,it must be in boundary as per framesize",
				LOGOIMPOSE_YCO_MIN,LOGOIMPOSE_YCO_MAX,LOGOIMPOSE_YCO_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_ALPHA,
			g_param_spec_double ("alpha", "Alpha", "the opacity  percentage of logo imposed on frame:generally between 0 to 100 (increarse in perecentage reduces transperency",LOGOIMPOSE_ALPHA_MIN,LOGOIMPOSE_ALPHA_MAX,LOGOIMPOSE_ALPHA_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_PATH,g_param_spec_string ("path", "Path", "the path of image  which is t=o be imposed on videoframe check wheather the image meets following requirements\n                        1.image is of png format\n                         2.it is better to have image below 200x200","default" , G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_DIRECTION,g_param_spec_string ("direction", "Direction", "the direction in which the animation of image should be up,down,right,left","right" , G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_SPEED,g_param_spec_string ("speed", "speed", "the speed of rotation you want in the followed fast,medium,slow","slow" , G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_ANIMATION,g_param_spec_string ("animation", "animation", "the type of animation wheather to scroll or roatate by mentioning options scroll,rotate,normal\n                        directions for animations\n                        scroll : left,right,up,down\n                        rotate : left,right\n                        normal : it is steady at particular location","normal" , G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	gst_element_class_set_details_simple (gstelement_class,
			"Logoimpose",
			"FIXME:Generic",
			"FIXME:Generic Template Element", "Rakesh Konathala <<user@hostname.org>>");

	gst_element_class_add_pad_template (gstelement_class,gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (gstelement_class,gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void gst_logoimpose_init (GstLogoimpose * filter)
{
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	gst_pad_set_event_function (filter->sinkpad,GST_DEBUG_FUNCPTR (gst_logoimpose_sink_event));
	gst_pad_set_chain_function (filter->sinkpad,GST_DEBUG_FUNCPTR (gst_logoimpose_chain));
	GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	GST_PAD_SET_PROXY_CAPS (filter->srcpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

	filter->xco= LOGOIMPOSE_XCO_DEFAULT;
	filter->yco= LOGOIMPOSE_YCO_DEFAULT;
	filter->alpha=LOGOIMPOSE_ALPHA_DEFAULT;
	strcpy(filter->path,"default");
	strcpy(filter->direction,"right");
	strcpy(filter->speed,"slow");
	strcpy(filter->animation,"normal");
}
/*THIS FUNCTION GETS VALUE FROM STRUCTURE AND PRINTS ON CONSOLE*/
static void gst_logoimpose_set_property (GObject * object, guint prop_id,const GValue * value, GParamSpec * pspec)
{
	GstLogoimpose *filter = GST_LOGOIMPOSE (object);
	switch (prop_id) {
		case PROP_XCO:
			if(g_value_get_int(value) < 0)
			{
				g_warning("Negative values should not give as coordinates\n");
				exit(1);
			}
			if(g_value_get_int(value) > 1920)
			{
				g_warning(" out of framesize\n");
				exit(1);
			}
			filter->xco = g_value_get_int (value);
			break;
		case PROP_YCO:
			if(g_value_get_int(value) <0)
			{
				g_warning("Negative values should not give as coordinates\n");
				exit(1);
			}
			if(g_value_get_int(value) > 1080)
			{
				g_warning(" out of framesize\n");
				exit(1);
			}
			filter->yco = g_value_get_int (value);
			break;
		case PROP_ALPHA:
			filter->alpha = g_value_get_double (value);
			break;
		case PROP_PATH:
			GST_DEBUG_OBJECT(filter,"The path in set property:%s\n",g_value_get_string(value));
			strcpy(filter->path,g_value_get_string(value));
			break;
		case PROP_DIRECTION:
			GST_DEBUG_OBJECT(filter,"in set property %s\n",g_value_get_string(value));
			strcpy(filter->direction,g_value_get_string(value));
			break;
		case PROP_SPEED:
			strcpy(filter->speed,g_value_get_string(value));
			break;
		case PROP_ANIMATION:
			strcpy(filter->animation,g_value_get_string(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}
/*THIS FUNCTION TO SET VALUE GVALUE STRUCTURE WHICH IS INITIALIZED BEFORE*/
static void gst_logoimpose_get_property (GObject * object, guint prop_id,GValue * value, GParamSpec * pspec)
{
	GstLogoimpose *filter = GST_LOGOIMPOSE (object);

	switch (prop_id)
	{
		case PROP_XCO:
			g_value_set_int (value ,filter->xco);
			break;
		case PROP_YCO:
			g_value_set_int (value ,filter->yco);
			break;
		case PROP_ALPHA:
			g_value_set_double (value ,filter->alpha);
			break;
		case PROP_PATH:
			g_value_set_string(value,filter->path);
			break;
		case PROP_DIRECTION:
			g_value_set_string(value,filter->direction);
			break;
		case PROP_SPEED:
			g_value_set_string(value,filter->speed);
			break;
		case PROP_ANIMATION:
			g_value_set_string(value,filter->animation);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}
/* GstElement vmethod implementations */
/* this function handles sink events */
static gboolean gst_logoimpose_sink_event (GstPad * pad, GstObject * parent,GstEvent * event)
{
	GstLogoimpose *filter;
	gboolean ret;
	filter = GST_LOGOIMPOSE (parent);
	GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,GST_EVENT_TYPE_NAME (event), event);

	switch (GST_EVENT_TYPE (event)) {
		case GST_EVENT_CAPS:
			{
				GstCaps *caps;
				gst_event_parse_caps (event, &caps);
				/* do something with the caps */
				ret = gst_pad_event_default (pad, parent, event);
				break;
			}
		default:
			ret = gst_pad_event_default (pad, parent, event);
			break;
	}
	return ret;
}
/* chain function
 * this function does the actual processing
 */
static GstFlowReturn gst_logoimpose_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	GstLogoimpose *filter;
	//declaring the video information and videoframe with thier respective datatypes
	GstVideoInfo video_info;
	GstVideoFrame vframe;
	filter = GST_LOGOIMPOSE (parent);
	if(!(strcmp(filter->path,"default")))
	{
		char cwd[100];
		getcwd(cwd, sizeof(cwd));
		char def[20]="/default.png";
		strcat(cwd,def);
		GST_DEBUG_OBJECT(filter,"Current working dir: %s\n", cwd);
		strcpy(filter->path,cwd);
	}
	//timestamp and duration by using the default macros from gstreamer library
	GstClockTime timestamp = GST_BUFFER_TIMESTAMP(buf);
	GstClockTime duration = GST_BUFFER_DURATION(buf);
	// Calculate frame number from the timestamp and duration
	guint64 frameNumber = GST_TIME_AS_MSECONDS(timestamp) / GST_TIME_AS_MSECONDS(duration);
	GST_DEBUG_OBJECT(filter,"Frame Number: %" G_GUINT64_FORMAT "\n", frameNumber);
	////function to give permissions to the buffer
	buf=gst_buffer_make_writable(buf); 
	if(buf == NULL)
	{
		g_print("unable to make buffer writable\n");
		exit(0);
	}
	if(!(gst_video_info_from_caps(&video_info,gst_pad_get_current_caps(pad))))
	{
		g_print("failed to retrieve information from caps\n");
		exit(0);
	}
	if(!(gst_video_frame_map(&vframe,&video_info,buf,GST_MAP_WRITE)))
	{
		g_print("failed to map the video frame\n");
		exit(0);
	}
	if(filter->alpha > 100)
	{
		g_warning("give the value of alpha in range\n");
		exit(0);
	}
	double alpha=(filter->alpha/100);
	guint8 *y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0); // Y plane
	guint8 *uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 1); // UV plane (interleaved)
	guint y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0); // Y plane stride
	guint uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1); // UV plane stride,pixel_stride is 2; // In NV12, UV values are interleaved every 2 bytes
	//retreiving the height and wiidth of frames by macros..
	gint width = GST_VIDEO_FRAME_WIDTH(&vframe);
	gint height = GST_VIDEO_FRAME_HEIGHT(&vframe);
	GdkPixbuf* logo_pixbuf = gdk_pixbuf_new_from_file(filter->path, NULL);
	if (!logo_pixbuf)
	{
		g_warning("Error loading the logo image.\n");
		exit(1);
	}
	gint logo_width = gdk_pixbuf_get_width(logo_pixbuf);
	gint logo_height = gdk_pixbuf_get_height(logo_pixbuf);
	guint xcor=filter->xco;
	guint ycor=filter->yco;
	if((xcor >= width-logo_width) && (ycor >= height-logo_height))
	{
		g_warning("out of boundary condition\n");
		exit(1);
	}
	guint r_angle=ROTATION_ANGLE;
	//calling the functions..
	if(!(strcmp("scroll",filter->animation)))
		imposelogo_scroll(y_pixels, uv_pixels, y_stride, uv_stride, width, height,filter->path,xcor,ycor,frameNumber,alpha,filter->speed,filter->direction);
	else if(!(strcmp("rotate",filter->animation)))
		rotation_functionality(y_pixels, uv_pixels, y_stride, uv_stride, width, height, filter->path,r_angle,frameNumber,filter->speed,xcor,ycor,filter->direction,alpha);
	else if(!(strcmp("normal",filter->animation)))
		imposelogo_normal(y_pixels, uv_pixels, y_stride, uv_stride, width, height,filter->path,xcor,ycor,frameNumber,alpha,filter->direction);
	else{
		g_print("<<<select the required animation..>>>\n");
		exit(1);
	}
	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, buf);
}
/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean logoimpose_init (GstPlugin * logoimpose)
{
	/* debug category for filtering log messages
	 *
	 * exchange the string 'Template logoimpose' with your description
	 */
	GST_DEBUG_CATEGORY_INIT (gst_logoimpose_debug, "logoimpose",0, "Template logoimpose");
	return GST_ELEMENT_REGISTER (logoimpose, logoimpose);
}
/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstlogoimpose"
#endif

/* gstreamer looks for this structure to register logoimposes
 *
 * exchange the string 'Template logoimpose' with your logoimpose description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		logoimpose,
		"logoimpose",
		logoimpose_init, 
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
		
		
//______________________________________________________________________________________________________________________________________________________________________


void rotation_functionality(guint8 *y_pixels, guint8 *uv_pixels, guint y_stride, guint uv_stride, guint width, guint height, const gchar *png_path, guint rotation_angle,guint64 frameno,const gchar *speed,guint xco,guint yco,const gchar *direction,double alpha_factor) 
{
	/* Load the PNG image and  rotating a PNG image and overlaying it onto a YUV frame using Cairo(2d graphics library package */
	cairo_surface_t *png_surface = cairo_image_surface_create_from_png(png_path);
	//calcualtes width and height of logo..
	guint logo_width = cairo_image_surface_get_width(png_surface);
	guint logo_height = cairo_image_surface_get_height(png_surface);
	guint8 *logo_data = cairo_image_surface_get_data(png_surface);
	// Rotation center at the center of the image ,here coordinataes taken as centre of image dimensions
	double rotation_center_x = logo_width/2;
	double rotation_center_y = logo_height/2;
	if(!(strcmp("fast",speed)))
		rotation_angle=frameno*10;
	if(!(strcmp("medium",speed)))
		rotation_angle=frameno/5;
	if(!(strcmp("slow",speed)))
		rotation_angle=frameno/10;
	guint h, w;
	double rotated_x,rotated_y;
	// Rotate the PNG image and overlay onto the YUV frame
	for ( h = 0 ; h < logo_height && h+ xco < height; ++h)
	{
		for ( w =0; w < logo_width && w+ yco < width; ++w)
		{
			// Calculate rotated coordinates using mathamatical formulas
			if(!(strcmp("right",direction)))
			{
				//mathamatical formulas to calculate the coordinates for rotating
				//rotated_x = cos(rotation_angle) * (w - rotation_center_x) - sin(rotation_angle) * (h - rotation_center_y) + rotation_center_x + xco;
				//rotated_y = sin(rotation_angle) * (w - rotation_center_x) + cos(rotation_angle) * (h - rotation_center_y) + rotation_center_y + yco;
				rotated_x = cos(rotation_angle) * (w - rotation_center_x) - sin(rotation_angle) * (h - rotation_center_y)+rotation_center_x+xco;
                                rotated_y = sin(rotation_angle) * (w - rotation_center_x) + cos(rotation_angle) * (h - rotation_center_y) + rotation_center_y+yco;
			}
			else
			{
				//rotated_x = cos(rotation_angle) * (w - rotation_center_x) + sin(rotation_angle) * (h - rotation_center_y) + xco;
				//rotated_y = -sin(rotation_angle) * (w - rotation_center_x) + cos(rotation_angle) * (h - rotation_center_y) + yco;
				rotated_x = cos(rotation_angle) * (w - rotation_center_x) + sin(rotation_angle) * (h - rotation_center_y)+rotation_center_x+xco;
                                rotated_y = -sin(rotation_angle) * (w - rotation_center_x) + cos(rotation_angle) * (h - rotation_center_y) + rotation_center_y+yco;

			}
			// Check if the rotated coordinates are within the frame bounds
			if (rotated_x >= 0 && rotated_x < width && rotated_y >= 0 && rotated_y < height)
			{
				//accessing address of pixel values
				guint8 *y_pixel = y_pixels + (guint)rotated_y * y_stride + (guint)rotated_x;
				guint8 *uv_pixel = uv_pixels + ((guint)rotated_y / 2) * uv_stride + ((guint)rotated_x / 2) * 2;
				//accessing address of each color channel  in bitformat of each pixel in frame by usinf logo_data 
				guint8 r = logo_data[(h * logo_width + w) * 4];
				guint8 g = logo_data[(h * logo_width + w) * 4 + 1];
				guint8 b = logo_data[(h * logo_width + w) * 4 + 2];
				guint8 a = logo_data[(h * logo_width + w) * 4 + 3]*alpha_factor;
				// Calculate the final YUV values based on alpha blending
				guint8 y = (((255 - a) * (*y_pixel) + a * ((0.257 * r) + (0.504 * g) + (0.098 * b) + 16)) / 255);  // Y component
				guint8 u = (((255 - a) * (uv_pixel[0]) + a * ((0.439 * r) - (0.368 * g) - (0.071 * b) + 128)) / 255);  // U component
				guint8 v = (((255 - a) * (uv_pixel[1]) + a * (-(0.148 * r) - (0.291 * g) + (0.439 * b) + 128)) / 255);  // V component
                                //assigining yuv values to respective addresses
				*y_pixel = y;
				uv_pixel[0] = u;
				uv_pixel[1] = v;
			}
		}
	}

	cairo_surface_destroy(png_surface);
}

//______________________________________________________________________________________________________________________________________________________________________
void imposelogo_scroll(guint8 *y_pixels, guint8 *uv_pixels, guint y_stride, guint uv_stride, guint width, guint height, const gchar *png_path,guint xco,guint yco,guint64 frameno,double alpha_factor,const gchar *speed,const gchar* direction)
{
	// Load the PNG image using Cairo
	cairo_surface_t *png_surface = cairo_image_surface_create_from_png(png_path);
	//cairo_format_t  cairo_image_surface_get_format (cairo_surface_t *png_surface);
	guint logo_width = cairo_image_surface_get_width(png_surface);
	guint logo_height = cairo_image_surface_get_height(png_surface);
	guint8 *logo_data = cairo_image_surface_get_data(png_surface);
	guint s=0;
	 if(!(strcmp("fast",speed)))
                s=10;
        if(!(strcmp("medium",speed)))
                s=6;
        if(!(strcmp("slow",speed)))
                s=2;
	/*frameno is dynamic for every frame in a video so we are using frameno for animation purpose*/
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	if (!(strcmp("down", direction))) {
                xco = (frameno * s) % (width-logo_height)/2;
		g_print("xco down %d\n",xco);
        }
        if (!(strcmp("up", direction))) {
                xco =(height-logo_height) - ((frameno * s) % (height-logo_height));
		g_print("xco up  %d\n",xco);
        }
        if (!(strcmp("right", direction))) {
                yco = frameno * s % width;
		g_print("yco %d\n",yco);
        }
        if (!(strcmp("left", direction))) {
                yco = (width + logo_width) - (frameno * s % (width + logo_width));
		g_print("yco left %d\n",yco);
        }
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	guint h,w;
	// Overlay the PNG image onto the YUV frame
	for ( h = 0 ; h < logo_height && h+xco < height; ++h)
	{
		for ( w =0; w < logo_width && w+yco < width; ++w)
		{
			//accessing address of pixel values
			guint8 *y_pixel = y_pixels + (h+xco) * y_stride + (w+yco);
			guint8 *uv_pixel = uv_pixels + ((h+xco) / 2) * uv_stride + ((w+yco) / 2) * 2;
			//accessing address of each color channel  in bitformat of each pixel in frame by usinf logo_data 
			guint8 r = logo_data[(h * logo_width + w) * 4];
			guint8 g = logo_data[(h * logo_width + w) * 4 + 1];
			guint8 b = logo_data[(h * logo_width + w) * 4 + 2];
			guint8 a = logo_data[(h * logo_width + w) * 4 + 3]*alpha_factor;
			// Calculate the final YUV values based on alpha blending
			guint8 y = (((255 - a) * (*y_pixel) + a * ((0.257 * r) + (0.504 * g) + (0.098 * b) + 16)) / 255);  // Y component
			guint8 u = (((255 - a) * (uv_pixel[0]) + a * ((0.439 * r) - (0.368 * g) - (0.071 * b) + 128)) / 255);  // U component
			guint8 v = (((255 - a) * (uv_pixel[1]) + a * (-(0.148 * r) - (0.291 * g) + (0.439 * b) + 128)) / 255);	// V component
																//assigining yuv values to respective addresses
			*y_pixel = y;
			uv_pixel[0] = u;
			uv_pixel[1] = v;
		}
	}
	cairo_surface_destroy(png_surface);
}
//______________________________________________________________________________________________________________________
void imposelogo_normal(guint8 *y_pixels, guint8 *uv_pixels, guint y_stride, guint uv_stride, guint width, guint height, const gchar *png_path,guint xco,guint yco,guint64 frameno,double alpha_factor,const gchar *direction)
{
	// Load the PNG image using Cairo
	cairo_surface_t *png_surface = cairo_image_surface_create_from_png(png_path);
	//cairo_format_t  cairo_image_surface_get_format (cairo_surface_t *png_surface);
	//---------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------
	guint logo_width = cairo_image_surface_get_width(png_surface);
	guint logo_height = cairo_image_surface_get_height(png_surface);
	guint8 *logo_data = cairo_image_surface_get_data(png_surface);
	//g_print("xco=%d yco=%d width=%d height=%d logo_w=%d logo_h=%d\n",xco,yco,width,height,logo_width,logo_height);
	if((xco == width) && (yco >= height))
	{
		g_warning("out of boundary condition\n");
		exit(1);
	}
	guint h,w;
	// Overlay the PNG image onto the YUV frame
	for ( h = 0 ; h < logo_height && h+xco < height; ++h)
	{
		for ( w =0; w < logo_width && w+yco < width; ++w)
		{
			//accessing address of pixel values
			guint8 *y_pixel = y_pixels + (h+xco) * y_stride + (w+yco);
			guint8 *uv_pixel = uv_pixels + ((h+xco) / 2) * uv_stride + ((w+yco) / 2) * 2;
			//accessing address of each color channel  in bitformat of each pixel in frame by usinf logo_data 
			guint8 r = logo_data[(h * logo_width + w) * 4];
			guint8 g = logo_data[(h * logo_width + w) * 4 + 1];
			guint8 b = logo_data[(h * logo_width + w) * 4 + 2];
			/*alpha channel calculation to impose with transperency*/
			guint8 a = logo_data[(h * logo_width + w) * 4 + 3]*alpha_factor;
			// Calculate the final YUV values based on alpha blending
			guint8 y = (((255 - a) * (*y_pixel) + a * ((0.257 * r) + (0.504 * g) + (0.098 * b) + 16)) / 255);  // Y component
			guint8 u = (((255 - a) * (uv_pixel[0]) + a * ((0.439 * r) - (0.368 * g) - (0.071 * b) + 128)) / 255);  // U component
			guint8 v = (((255 - a) * (uv_pixel[1]) + a * (-(0.148 * r) - (0.291 * g) + (0.439 * b) + 128)) / 255);  // V component
																//assigining yuv values to respective addresses
			*y_pixel = y;
			uv_pixel[0] = u;
			uv_pixel[1] = v;
		}
	}
	cairo_surface_destroy(png_surface);

}
const char* format_to_string(cairo_format_t format)
{
	switch (format)
	{
		case CAIRO_FORMAT_ARGB32:
			return "ARGB32";
		case CAIRO_FORMAT_RGB24:
			return "RGB24";
		case CAIRO_FORMAT_A8:
			return "A8";
			// Add more cases as needed for other formats
		default:
			return "Unknown Format";
	}
}
